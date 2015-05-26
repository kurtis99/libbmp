#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <endian.h>

#include <assert.h>

#include "bmp.h"
#include "debug.h"

void BMP_destroy(struct BMP *b)
{
	if (b == NULL)
		return;

	free(b->color_table);
	free(b->pixels);
	free(b);
}

static uint8_t rgb_to_grey(uint32_t rgb)
{
	int r, g, b;
	float grey;

	r = (rgb & 0xff0000) >> 16;
	g = (rgb & 0xff00) >> 8;
	b = rgb & 0xff;

	grey = r * 0.2898 + g * 0.5870 + b * 0.1140;

	return grey;
}

/* To convert following formula is used:
 * grey = R * 0x2989 + G * 0.5870 + B * 0.1140
 * */
void BMP_to_greyscale(struct BMP *b)
{
	uint8_t *colors;
	size_t i;

	colors = malloc(b->DIB.colors);
	if (!colors)
		fprintf(stderr, "Failed to allocate greyscale map");

	for (i = 0; i < b->DIB.colors; i++)
		colors[i] = rgb_to_grey(b->color_table[i]);

	fprintf(stderr, "Print colors table\n");
	dump_array(stderr, colors, b->DIB.colors, sizeof(uint8_t));

	for (i = 0; i < b->DIB.image_size; i++)
		b->pixels[i] = colors[b->pixels[i]];

	free(colors);
}

/* Functions returns number of zeros on the right side */
static int ntz(uint32_t v)
{
	int n;

	if (v == 0) return 32;

	n = 0;
	if ((v & 0x0000FFFF)== 0) { n += 16; v >>= 16;}
	if ((v & 0x000000FF)== 0) { n += 8; v >>= 8;}
	if ((v & 0x0000000F)== 0) { n += 4; v >>= 4;}
	if ((v & 0x00000003)== 0) { n += 2; v >>= 2;}

	return n + !(v&0x1);
}

static int pop(const uint32_t v)
{
	int n;

	n = v;

	n = (n & 0x55555555) + ((n >> 1) & 0x55555555);
	n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
	n = (n & 0x0F0F0F0F) + ((n >> 4) & 0x0F0F0F0F);
	n = (n & 0x00FF00FF) + ((n >> 8) & 0x00FF00FF);
	n = (n & 0x0000FFFF) + ((n >> 16) & 0x0000FFFF);

	return n;
}

/* Align number up to multiple of 2 */
static int align_up(const int v, const int mul)
{
	if (v == 0) return mul;
	return (v + mul - 1) & -mul;
}

#define ASSERT_BPP(_bpp) \
	assert(_bpp == 1 || _bpp == 2 || _bpp == 4 || _bpp == 8 || _bpp == 16 || \
			_bpp == 24 || _bpp == 32);

/* Return number of bytes that will allocate pix pixels in bpp format with mul
 * alignment */
static int align_up_bpp(int pix, const int bpp, const int mul)
{
	ASSERT_BPP(bpp)
	int bytes;
	bytes = (pix * bpp + 7) / 8;
	return align_up(bytes, mul);
}

static unsigned int _do_mask(unsigned char nones)
{
	return (1UL << nones) - 1;
}

/* Extract num-th color which is bpp bits long from input buffer
 *
 * Buffer overflow issue should be checked from caller, if @buf is
 * pointer to last element in array and @bpp is more that 1 byte then buffer
 * overflow will occur.
 * */
static unsigned int get_at_offset(const uint8_t *buf, const int num, const int bpp)
{
	ASSERT_BPP(bpp)
	assert(num >= 0);
	assert(buf != NULL);

	int nbytes, bits, out, i;

	/* Bytes that contains start of required pixel (or whole pixel) */
	nbytes = (num * bpp) / 8;
	/* Bit offset where pixel starts */
	bits = (num * bpp) % 8;

	for (out = i = 0; i <= (bpp / 8 - !(bpp % 8)); i++)
		out = ((buf[nbytes + i] >> bits) & _do_mask(bpp)) + (out << (8*!!i));

	return out;
}

static uint32_t get_point_bitmap(const struct BMP *b, const size_t x, const size_t y)
{
	int scanline;
	uint32_t pixel;
	int bpp;

	bpp = b->DIB.bits_per_pixel;

	scanline = align_up_bpp(b->DIB.width, bpp, 4);
	pixel = get_at_offset(&b->pixels[scanline * y], x, bpp);

	return pixel;
}

struct b_offset {
	int offset;
	int len;
};

struct ColorOffsets {
	struct b_offset red;
	struct b_offset green;
	struct b_offset blue;
};

struct ColorOffsets c16 = {
	.red = {0, 5},
	.green = {5, 5},
	.blue = {10, 5}
};

struct ColorOffsets cXX = {
	.red = {0, 8},
	.green = {8, 8},
	.blue = {16, 8}
};

static uint32_t _get_bits(const uint32_t word, const int offset, const int len)
{
	return (word >> offset) & _do_mask(len);
}

/* gets bits from @word that are at offset @offset and @len bits long */
static uint32_t get_bits(const uint32_t word, struct b_offset *b)
{
	return _get_bits(word, b->offset, b->len);
}


static struct b_offset get_bitoffsets(const uint32_t mask)
{
	struct b_offset bo;

	bo.offset = ntz(mask);
	bo.len = pop(mask);

	return bo;
}

static struct Color pixel_to_RGB(const struct BMP *b, uint32_t pixel)
{
	int bpp;
	struct Color c;
	struct ColorOffsets c_tmp;

	memset(&c, '\0', sizeof(struct Color));

	bpp = b->DIB.bits_per_pixel;
	ASSERT_BPP(bpp);

	if (b->DIB.compression == BI_RGB) {
		switch (bpp) {
			case 16:
				c_tmp = c16; break;
			default:
				c_tmp = cXX; break;
		}
	} else if (b->DIB.compression == BI_BITFIELDS) {
		c_tmp.red = get_bitoffsets(be32toh(b->DIB.red_mask));
		c_tmp.green = get_bitoffsets(be32toh(b->DIB.green_mask));
		c_tmp.blue = get_bitoffsets(be32toh(b->DIB.blue_mask));
	} else {
		assert(0);
	}

	c.blue = get_bits(pixel, &c_tmp.blue);
	c.green = get_bits(pixel, &c_tmp.green);
	c.red = get_bits(pixel, &c_tmp.red);

	return c;
}

struct Color BMP_get_pixel(const struct BMP *b, const size_t x, const size_t y)
{
	struct Color c;
	uint32_t pixel;

	/* Supported only BI_RGB and BI_BITFIELDS */
	assert(b->DIB.compression == BI_RGB || b->DIB.compression == BI_BITFIELDS);

	pixel = get_point_bitmap(b, x, y);
	c = pixel_to_RGB(b, b->DIB.colors == 0 ? pixel : b->color_table[pixel]);

	return c;
}

/* Prints structure field and its integer value */
#define dump_dib(x, t) printf("%-20s" ": %u\n", t, b->DIB.x);
/* Prints structure field and string representation of its integer value */
#define dump_dib_str(x, func, t) printf("%-20s" ": %s (%d)\n", t, func(b->DIB.x), b->DIB.x);

static const char* _get_compression_str(const int v)
{
	const char *str;

	switch (v) {
		case BI_RGB:       str = "BI_RGB"; break;
		case BI_RLE8:      str = "BI_RLE8"; break;
		case BI_RLE4:      str = "BI_RLE4"; break;
		case BI_BITFIELDS: str = "BI_BITFIELDS"; break;
		case BI_JPEG:      str = "BI_JPEG"; break;
		case BI_PNG:       str = "BI_PNG"; break;
		case BI_ALPHABITFIELDS: str = "BI_ALPHABITFIELDS"; break;
		case BI_CMYK:     str = "BI_CMYK"; break;
		case BI_CMYKRLE8: str = "BI_CMYKRLE8"; break;
		case BI_CMYKTLE4: str = "BI_CMYKTLE4"; break;
		default: str = "UNKNOWN"; break;
	}

	return str;
}

static int min(const int a, const int b)
{
	return a >= b ? b : a;
}

void BMP_dump_color(const struct Color *c)
{
	printf("Color[RGB]: %d %d %d\n", c->red, c->green, c->blue);
}

void BMP_dump(const struct BMP *b)
{
	int i;

	printf("BMP magic: %c%c\n", b->Header.magic[0], b->Header.magic[1]);
	printf("BMP size: %u\n", b->Header.size);
	printf("BMP pixel offset: %u\n", b->Header.pix_offset);

	switch (b->DIB.DIB_type) {
		case BITMAPINFOHEADER: printf("Header type: BITMAPINFOHEADER\n"); break;
		case BITMAPV4HEADER: printf("Header type: BITMAPV4HEADER\n"); break;
		case BITMAPV5HEADER: printf("Header type: BITMAPV5HEADER\n"); break;
		default: printf("Header type: UNKNOWN\n"); break;
	}

	dump_dib(size, "size")
	dump_dib(width, "width")
	dump_dib(height, "height")
	dump_dib(color_planes, "color_planes")
	dump_dib(bits_per_pixel, "bits_per_pixel")
	dump_dib_str(compression, _get_compression_str, "compression")
	dump_dib(image_size, "image_size")
	dump_dib(h_resolution, "h_resolution")
	dump_dib(v_resolution, "v_resolution")
	dump_dib(colors, "colors")
	dump_dib(important_colors, "important_colors")
	dump_dib(red_mask, "red_mask")
	dump_dib(green_mask, "green_mask")
	dump_dib(blue_mask, "blue_mask")
	dump_dib(alpha_mask, "alpha_mask")
	dump_dib(color_space_type, "color_space_type")
	dump_dib(red_gamma, "red_gamma")
	dump_dib(green_gamma, "green_gamma")
	dump_dib(blue_gamma, "blue_gamma")
	dump_dib(intent, "intent")
	dump_dib(ICC_profile_data, "ICC_profile_data")
	dump_dib(ICC_profile_size, "ICC_profile_size")
	dump_dib(reserved, "reserved")

	printf("Dumps pixels buffer\n");
	for (i = 0; i < min(256, b->DIB.image_size); i++) {
		if (i%32 == 0 && i > 0)
			printf("\n");
		printf("%02x ", b->pixels[i]);
	}
	printf("\n");
}
