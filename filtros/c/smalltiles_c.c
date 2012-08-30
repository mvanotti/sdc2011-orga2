#include <stdio.h>

void smalltiles(unsigned char *src, unsigned char *dst, int h, int w, int row_size);

void smalltiles_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {
    int ih;
    int idh;
    w = (w%2) ? w-1 : w;
    h = (h%2) ? h-1 : h;
    for (ih = idh = 0; ih < h*src_row_size; ih += 2*src_row_size, idh += dst_row_size) {
        int iw;
        int idw;
        for (iw = idw = 0; iw < w*4; iw = iw+8, idw = idw+4) {
            smalltiles(&src[ih+iw], &dst[idh+idw], h, w, dst_row_size);
        }
    }
}

inline void smalltiles(unsigned char *src, unsigned char *dst, int h, int w, int row_size) {
    int i;
    for (i = 0; i < 3; i++) {
        dst[i] = src[i];
        dst[2*w+i] = src[i];
        dst[row_size*h/2 + 2*w + i] = src[i];
        dst[row_size*h/2 + i] = src[i];
    }
}
