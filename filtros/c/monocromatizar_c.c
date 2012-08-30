#include<limits.h>
#define ALFA 0.07
#define BETA 0.71
#define GAMA 0.21
#define PIXEL_SIZE 4

unsigned char saturar2(double c);

void monocromatizar_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {

    unsigned int srcrow = 0;
    unsigned int dstrow = 0;
    int r = 0;

    while (r < h) {

        for (int c = 0; c < w; c += 1) {
            monocromatizar(&src[srcrow + (c * PIXEL_SIZE)], &dst[dstrow + (c * PIXEL_SIZE)]);
    	}

        srcrow += src_row_size;
        dstrow += dst_row_size;
        r += 1;
    }
}

inline void monocromatizar(unsigned char *src, unsigned char *dst) {
    unsigned char mono = ALFA * src[2] + BETA * src[1] + GAMA * src[0];
    dst[0] = mono;
    dst[1] = mono;
    dst[2] = mono;
}

inline unsigned char saturar2(double c) {
    if (c > UCHAR_MAX) {
        return UCHAR_MAX;
    }
    if (c < 0) {
        return 0;
    }
    return c;
}
