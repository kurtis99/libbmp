#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

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

#if 0
/* Functions returns number of zeros on the right side */
static int post_zeros(uint32_t v)
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
#endif

/* Align number up to multiple of 2 */
static int align_up(const int v, const int mul)
{
	if (v == 0) return mul;
	return (v + mul - 1) & -mul;
}

/* Return number of bytes that will allocate pix pixels in bpp format with mul
 * alignment */
static int align_up_bpp(int pix, const int bpp, const int mul)
{
	int bytes;
	bytes = (pix * bpp + 7) / 8;
	return align_up(bytes, mul);
}

/* Return data */
static unsigned int get_at_offset(const uint8_t *buf, const int num, const int bpp)
{
	int bytes, bits;

	/* Bytes that contains start of required pixel (or whole pixel) */
	bytes = (pix * bpp) / 8;
	/* Bit offset where pixel starts */
	bits = (pix * bpp) % 8;

	/* TODO */
}

static uint32_t fetch_bitmap(const struct BMP *b, const size_t x, const size_t y)
{
	int scanline;
	uint32_t pixel;
	int bpp;

	bpp = b->DIB.bits_per_pixel;
	scanline = align_up_bpp(b->DIB.width, bpp, 4);

	switch (bpp) {
		pixel = b->pixels[bpp * (x + y * scanline)]
	}



	/* if color table exists, take pixel from there */
	if (b->DIB.colors > 0)
		return b->color_table[pixel];
	else
		return pixel;
}

struct Color BMP_get_pixel(const struct BMP *b, const size_t x, const size_t y)
{
	struct Color c;
//	int bpp;
	uint32_t pixel;

	/* BPP 1,4,8 - color table is MUST */
	/* BPP 16, 24, 32 - color table might be (check) */
//	bpp = b->DIB.bits_per_pixel;

	pixel = fetch_bitmap(b, x, y);

	c.red = pixel + x + y + align_up_bpp(1,2,3);
#if 0
	uint32_t pixel;
	int scanline;
	scanline = align_up(b->DIB.width, 4);

	/* If colors is zero then every pixel is by itself and it shuold be
	 * parsed according to its format */
	if (b->DIB.colors == 0)
		pixel = b->color_table[x + y * scanline];

	/* Code below works only for compression == 3 */
	/* This works only for 32 RGB */
	assert(b->DIB.compression == BI_BITFIELDS);
	pixel = b->color_table[(x + y * scanline) * (b->DIB.bits_per_pixel / 8)];
	c.red   = (pixel & b->DIB.red_mask) >> post_zeros(b->DIB.red_mask);
	c.green = (pixel & b->DIB.green_mask) >> post_zeros(b->DIB.green_mask);
	c.blue  = (pixel & b->DIB.blue_mask) >> post_zeros(b->DIB.blue_mask);
	c.alpha = (pixel & b->DIB.alpha_mask) >> post_zeros(b->DIB.alpha_mask);
#endif

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

void BMP_dump(const struct BMP *b)
{
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
}
