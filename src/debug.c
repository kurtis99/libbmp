#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void dump_array(FILE *fd, const uint8_t *data, size_t len, size_t elem_size)
{
	char *buf;
	int p;
	size_t buf_size, i;

	if (fileno(fd) == -1) {
		fprintf(stderr, "Not valid file descriptor\n");
		return;
	}

	buf_size = len * (elem_size << 1) * 2;
	buf = malloc(buf_size);
	if (buf == NULL) {
		fprintf(stderr, "Failed to allocate memory\n");
		return;
	}

	for (p = i = 0; i < len; i++) {
		p += snprintf(buf + p, buf_size, "%s%02x", (i % 16) == 0 ? "\n" : " ", data[i]);
	}

	fprintf(fd, "%s\n", buf);

	free(buf);
}
