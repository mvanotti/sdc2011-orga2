#include <stdio.h>

void invertir(unsigned char *src, unsigned char *dst);

void invertir_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {
    int ih;
    int idh;
    for (ih = idh = 0; ih < h*src_row_size; ih += src_row_size, idh += dst_row_size) {
        int iw;
        for (iw = 0; iw < w*4; iw = iw+4) {
            invertir(&src[ih+iw], &dst[idh+iw]);
        }
    }
}

inline void invertir(unsigned char *src, unsigned char *dst) {
    dst[0] = 255 - src[0];
    dst[1] = 255 - src[1];
    dst[2] = 255 - src[2];
}
