#ifndef _BITMAP_H
#define _BITMAP_H

struct BM {
	size_t width;
	size_t height;
	size_t bpp;

	struct Color *pix;
};

#endif /* _BITMAP_H */
