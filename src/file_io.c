#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bmp.h"
#include "debug.h"

static uint32_t _get32(const uint8_t *addr)
{
	return addr[0] | addr[1] << 8 | addr[2] << 16 | addr[3] << 24;
}

static uint16_t _get16(const uint8_t *addr)
{
	return addr[0] | addr[1] << 8;
}

static void _set32(uint8_t *const addr, const uint32_t v)
{
	addr[0] =  v & 0x000000FF;
	addr[1] = (v & 0x0000FF00) >> 8;
	addr[2] = (v & 0x00FF0000) >> 8;
	addr[3] = (v & 0xFF000000) >> 8;
}

static void _set16(uint8_t *const addr, const uint32_t v)
{
	addr[0] = v & 0xFF;
	addr[1] = (v & 0xFF00) >> 8;
}

/* Returns size of header that was read */
static int BMP_read_header(int fd, struct Header *h)
{
	unsigned char buf[BMP_HEADER_SIZE];
	int r;

	r = read(fd, buf, BMP_HEADER_SIZE);
	if (r != BMP_HEADER_SIZE) {
		perror("Failed to read file\n");
	}

	h->magic[0] = buf[0];
	h->magic[1] = buf[1];

	h->size = _get32(&buf[2]);
	h->pix_offset = _get32(&buf[10]);

	return r;
}

static int BMP_read_dib(int fd, struct DIB *d)
{
	unsigned char buf[BITMAPV5HEADER_SIZE];
	int r;
	uint32_t size;
	const char *dib_type = NULL;

	memset(buf, '\0', ARRAY_SIZE(buf));

	r = read(fd, buf, sizeof(uint32_t));
	if (r != sizeof(uint32_t)) {
		perror("Failed to read file\n");
	}

	size = _get32(&buf[0]);

	r = read(fd, buf + sizeof(size), size - sizeof(size));
	if (r != (size - sizeof(uint32_t))) {
		perror("Failed to read file\n");
	}

	switch (size) {
	case BITMAPV5HEADER_SIZE:
		if (!dib_type) dib_type = "BITMAPV5HEADER";

		d->DIB_type = BITMAPV5HEADER;

		d->intent		= _get32(&buf[108]);
		d->ICC_profile_data	= _get32(&buf[112]);
		d->ICC_profile_size	= _get32(&buf[116]);
		d->reserved		= _get32(&buf[120]);

	case BITMAPV4HEADER_SIZE:
		if (!dib_type) dib_type = "BITMAPV4HEADER";

		d->DIB_type = BITMAPV4HEADER;

		d->red_mask		= _get32(&buf[40]);
		d->green_mask		= _get32(&buf[44]);
		d->blue_mask		= _get32(&buf[48]);
		d->alpha_mask		= _get32(&buf[52]);
		d->color_space_type	= _get32(&buf[56]);

		d->red_gamma		= _get32(&buf[96]);
		d->green_gamma		= _get32(&buf[100]);
		d->blue_gamma		= _get32(&buf[104]);

		memcpy(d->color_space_endpoints, &buf[60], 36);

	case BITMAPINFOHEADER_SIZE:
		if (!dib_type) dib_type = "BITMAPINFOHEADER";

		d->DIB_type = BITMAPINFOHEADER;

		d->size			= size;
		d->width		= _get32(&buf[4]);
		d->height		= _get32(&buf[8]);
		d->color_planes		= _get16(&buf[12]);
		d->bits_per_pixel	= _get16(&buf[14]);
		d->compression		= _get32(&buf[16]);
		d->image_size		= _get32(&buf[20]);
		d->h_resolution		= _get32(&buf[24]);
		d->v_resolution		= _get32(&buf[28]);
		d->colors		= _get32(&buf[32]);
		d->important_colors	= _get32(&buf[36]);
		break;
	default:
		dib_type = "UNKNOWN";
		break;
	}
	printf("Found DIB header type: %s\n", dib_type);

	return r + sizeof(uint32_t);
}

static int BMP_read_colortable(int fd, struct BMP *b)
{
	int r;
	int to_read;

	to_read = b->DIB.colors * sizeof(uint32_t);
	r = read(fd, b->color_table, to_read);
	if (r != to_read) {
		perror("Failed to read color table from file\n");
	}

	return r;
}

static int BMP_read_pixels(int fd, struct BMP *b)
{
	int r;
	int to_read;

	to_read = b->DIB.image_size;
	r = read(fd, b->pixels, to_read);
	if (r != to_read) {
		perror("Failed to read pixels from file\n");
	}

	return r;
}

/**
 * \brief Reads BMP at given path
 *
 * This functions read BMP File at provided path and returns intermediate
 * repsentation of BMP that was read
 *
 * \param path path to BMP file
 */
struct BMP* BMP_from_file(const char *path)
{
	int fd = 0;
	struct stat sb;
	int r;
	struct BMP *b;

	b = malloc(sizeof(struct BMP));
	if (b == NULL) {
		fprintf(stderr, "Failed to allocate memory for BMP struct\n");
		return NULL;
	}

	memset(b, '\0', sizeof(struct BMP));

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror(path);
		return NULL;
	}

	if (fstat(fd, &sb) == -1) {
		perror("fstat");
	}

	/* TODO: Do i realy need bytes read as return code? */
	r = BMP_read_header(fd, &b->Header);
	if (b->Header.size != sb.st_size)
		fprintf(stderr, "Wrong file size\n");

	r += BMP_read_dib(fd, &b->DIB);

	b->color_table = malloc(b->DIB.colors * sizeof(uint32_t));
	if (b->color_table == NULL)
		fprintf(stderr, "Failed to allocate memory\n");

	b->pixels = malloc(b->DIB.image_size);
	if (b->pixels == NULL)
		fprintf(stderr, "Failed to allocate memory\n");

	BMP_read_colortable(fd, b);
	BMP_read_pixels(fd, b);

        dump_array(stderr, (const uint8_t*)b->color_table, 1024, sizeof(uint8_t));
        dump_array(stderr, b->pixels, 1024, sizeof(uint8_t));

	return b;
}

static void BMP_write_header(int fd, const struct BMP *b)
{
	unsigned char buf[BMP_HEADER_SIZE];
	int r;

	memset(buf, '\0', ARRAY_SIZE(buf));

	buf[0] = 'B';
	buf[1] = 'M';

	_set32(&buf[2], b->Header.size);
	_set32(&buf[10], b->Header.pix_offset);

	r = write(fd, buf, BMP_HEADER_SIZE);
	if (r != BMP_HEADER_SIZE)
		perror("Failed to write file");
}

static void BMP_write_dib(int fd, const struct BMP *b)
{
	unsigned char buf[BITMAPV5HEADER_SIZE];
	int r;

	memset(buf, '\0', ARRAY_SIZE(buf));

	switch (b->DIB.DIB_type) {
	case BITMAPV5HEADER:

		_set32(&buf[108], b->DIB.intent);
		_set32(&buf[112], b->DIB.ICC_profile_data);
		_set32(&buf[116], b->DIB.ICC_profile_size);
		_set32(&buf[120], b->DIB.reserved);

	case BITMAPV4HEADER:

		_set32(&buf[40], b->DIB.red_mask);
		_set32(&buf[44], b->DIB.green_mask);
		_set32(&buf[48], b->DIB.blue_mask);
		_set32(&buf[52], b->DIB.alpha_mask);
		_set32(&buf[56], b->DIB.color_space_type);
		_set32(&buf[96], b->DIB.red_gamma);
		_set32(&buf[100], b->DIB.green_gamma);
		_set32(&buf[104], b->DIB.blue_gamma);

		memcpy(&buf[60], b->DIB.color_space_endpoints, 36);

	case BITMAPINFOHEADER:

		_set32(&buf[0], b->DIB.size);
		_set32(&buf[4], b->DIB.width);
		_set32(&buf[8], b->DIB.height);
		_set16(&buf[12], b->DIB.color_planes);
		_set16(&buf[14], b->DIB.bits_per_pixel);
		_set32(&buf[16], b->DIB.compression);
		_set32(&buf[20], b->DIB.image_size);
		_set32(&buf[24], b->DIB.h_resolution);
		_set32(&buf[28], b->DIB.v_resolution);
		_set32(&buf[32], b->DIB.colors);
		_set32(&buf[36], b->DIB.important_colors);

		break;
	default:
		break;
	}

	r = write(fd, buf, b->DIB.size);
	if (r != b->DIB.size)
		perror("Failed to write file");
}

static void BMP_write_colortable(int fd, const struct BMP *b)
{
	int r;
	int to_write;

	to_write = b->DIB.colors * sizeof(uint32_t);
	r = write(fd, b->color_table, to_write);
	if (r != to_write) {
		perror("Failed to write color table from file\n");
	}
}

static void BMP_write_pixels(int fd, const struct BMP *b)
{
	int r;
	int to_write;

	to_write = b->DIB.image_size;
	r = write(fd, b->pixels, to_write);
	if (r != to_write) {
		perror("Failed to write pixels from file\n");
	}
}

/**
 * \brief Writes intermediate state to file at given path
 *
 * Write content on struct BMP into file that is provided in functions
 * arguments. Should create file with properites represented by struct BMP.
 * When file is missing create it. If file is present, truncate it and fill
 * with values.
 *
 * \param path path to BMP file to write
 * \param BMP struct with intermediate representation
 */
int BMP_to_file(const char *path, const struct BMP *b)
{
	int fd;

	printf("Opening %s\n", path);

	// open file and get file descriptor
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror(path);
		return -1;
	}

	BMP_write_header(fd, b);
	BMP_write_dib(fd, b);
	BMP_write_colortable(fd, b);
	BMP_write_pixels(fd, b);

	// check which BMP header we are going to use
	// by default  create BITMAPINFOHEADER

}
