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
