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
	assert(b != NULL);

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

#define dump_dib(x, t) printf("%-20s" ": %u\n", t, b->DIB.x);

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
	dump_dib(compression, "compression")
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
