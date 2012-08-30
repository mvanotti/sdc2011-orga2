#include <stdio.h>

void rotar(unsigned char *src, unsigned char *dst);

void rotar_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {
    int ih;
    int idh;
    for (ih = idh = 0; ih < h*src_row_size; ih += src_row_size, idh += dst_row_size) {
        int iw;
        for (iw = 0; iw < w*4; iw = iw+4) {
            rotar(&src[ih+iw], &dst[idh+iw]);
        }
    }
}

inline void rotar(unsigned char *src, unsigned char *dst) {
    dst[0] = src[1];
    dst[1] = src[2];
    dst[2] = src[0];
}
