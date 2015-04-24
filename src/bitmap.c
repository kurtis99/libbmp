#include "bitmap.h"




struct BM *BMP_to_BM(const struct BMP *bmp)
{
	struct BM *b;
	int i, j;

	b = malloc(sizeof(struct BM));
	/* TODO: error check */

	b->width = bmp->DIB.width;
	b->height = bmp->DIB.height;
	b->bpp = bmp->DIB.bits_per_pixel;

	/* Allocate array to store whole bitmap */
	b->pix = malloc(sizeof(struct Color) * b->width * b->height);
	/* TODO: error check */

	/* Read */
	for (i = 0; i < b->width, i++) {
		for (j = 0; j < b->height; j++) {
			b->pix[i][j] = BMP_get_pixel(bmp, i, j);
		}
	}
}
