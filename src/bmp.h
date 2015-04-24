#ifndef _BMP_H
#define _BMP_H

#include <stdint.h>

#define BMP_HEADER_SIZE 14

#define BITMAPINFOHEADER_SIZE	40
#define BITMAPV2INFOHEADER_SIZE	52
#define BITMAPV3INFOHEADER_SIZE	56
#define BITMAPV4HEADER_SIZE	108
#define BITMAPV5HEADER_SIZE	124

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

/* This structures are not binary compatible and are not aligned in any way */
struct Header {
	uint8_t magic[2];
	uint32_t size;
	uint16_t reserved[2];
	uint32_t pix_offset;
};

enum DIB_type {
	BITMAPINFOHEADER,
	BITMAPV4HEADER,
	BITMAPV5HEADER,
	BITMAPHEADER_UNKNOWN,
};

enum DIB_Compression {
	BI_RGB = 0,
	BI_RLE8 = 1,
	BI_RLE4 = 2,
	BI_BITFIELDS = 3,
	BI_JPEG = 4,
	BI_PNG = 5,
	BI_ALPHABITFIELDS = 6,
	BI_CMYK = 11,
	BI_CMYKRLE8 = 12,
	BI_CMYKTLE4 = 13
};

/* implements BITMAPV5HEADER */
struct DIB {
	enum DIB_type DIB_type;

	/* BITMAPINFOHEADER starts */
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint16_t color_planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t image_size;
	uint32_t h_resolution;
	uint32_t v_resolution;
	uint32_t colors;
	uint32_t important_colors;
	/* BITMAPINFOHEADER ends */

	/* BITMAPV4HEADER starts */
	uint32_t red_mask;
	uint32_t green_mask;
	uint32_t blue_mask;
	uint32_t alpha_mask;
	uint32_t color_space_type;
	uint8_t color_space_endpoints[36];

	uint32_t red_gamma;
	uint32_t green_gamma;
	uint32_t blue_gamma;
	/* BITMAPV4HEADER ends */

	/* BITMAPV5HEADER starts */
	uint32_t intent;
	uint32_t ICC_profile_data;
	uint32_t ICC_profile_size;
	uint32_t reserved;
	/* BITMAPV5HEADER ends */
};

struct Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

struct BMP {
	struct Header Header;
	struct DIB DIB;
	uint32_t *color_table;
	uint8_t *pixels;
};

struct BMP* BMP_from_file(const char *);
int BMP_to_file(const char *, const struct BMP *);
void BMP_dump(const struct BMP *);
void BMP_destroy(struct BMP *);

void BMP_to_greyscale(struct BMP *);

#endif /* _BMP_H */
