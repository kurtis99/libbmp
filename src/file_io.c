#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"
#include "debug.h"

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
	unsigned char buf[BITMAPV5HEADER];
	int r;
	uint32_t size;
	const char *dib_type = NULL;

	memset(buf, '\0', BITMAPV5HEADER);

	r = read(fd, &size, sizeof(uint32_t));
	if (r != sizeof(uint32_t)) {
		perror("Failed to read file\n");
	}

	r = read(fd, buf + sizeof(uint32_t), size - sizeof(uint32_t));
	if (r != (size - sizeof(uint32_t))) {
		perror("Failed to read file\n");
	}

	switch (size) {
	case BITMAPV5HEADER:
		if (!dib_type) dib_type = "BITMAPV5HEADER";

		d->intent		= _get32(&buf[108]);
		d->ICC_profile_data	= _get32(&buf[112]);
		d->ICC_profile_size	= _get32(&buf[116]);
		d->reserved		= _get32(&buf[120]);

	case BITMAPV4HEADER:
		if (!dib_type) dib_type = "BITMAPV4HEADER";

		d->red_mask		= _get32(&buf[40]);
		d->green_mask		= _get32(&buf[44]);
		d->blue_mask		= _get32(&buf[48]);
		d->alpha_mask		= _get32(&buf[52]);
		d->color_space_type	= _get32(&buf[56]);

		d->red_gamma		= _get32(&buf[96]);
		d->green_gamma		= _get32(&buf[100]);
		d->blue_gamma		= _get32(&buf[104]);

		memcpy(d->color_space_endpoints, &buf[60], 36);

	case BITMAPINFOHEADER:
		if (!dib_type) dib_type = "BITMAPINFOHEADER";

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
	struct BMP *BMP;

	BMP = malloc(sizeof(struct BMP));
	if (BMP == NULL) {
		fprintf(stderr, "Failed to allocate memory for BMP struct\n");
		return NULL;
	}

	memset(BMP, '\0', sizeof(struct BMP));

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror(path);
		return NULL;
	}

	if (fstat(fd, &sb) == -1) {
		perror("fstat");
	}

	r = BMP_read_header(fd, &BMP->Header);
	if (BMP->Header.size != sb.st_size)
		fprintf(stderr, "Wrong file size\n");

	r += BMP_read_dib(fd, &BMP->DIB);

	BMP->color_table = malloc(BMP->DIB.colors * sizeof(uint32_t));
	if (BMP->color_table == NULL)
		fprintf(stderr, "Failed to allocate memory\n");

	BMP->pixels = malloc(BMP->DIB.image_size);
	if (BMP->pixels == NULL)
		fprintf(stderr, "Failed to allocate memory\n");

	BMP_read_colortable(fd, BMP);
	BMP_read_pixels(fd, BMP);

        dump_array(stderr, (const uint8_t*)BMP->color_table, 1024, sizeof(uint8_t));
        dump_array(stderr, BMP->pixels, 1024, sizeof(uint8_t));

	return BMP;
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
	// TODO
}