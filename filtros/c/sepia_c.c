#include <stdio.h>



#include<limits.h>
#define RED 0.5
#define GREEN 0.3
#define BLUE 0.2
#define PIXEL_SIZE 4

unsigned char saturar3( int c);
void sepia(unsigned char *src, unsigned char *dst);

void sepia_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {

    unsigned int srcrow = 0;
    unsigned int dstrow = 0;
    int r = 0;

    while (r < h) {

        for (int c = 0; c < w; c += 1) {
            sepia(&src[srcrow + (c * PIXEL_SIZE)], &dst[dstrow + (c * PIXEL_SIZE)]);
    	}

        srcrow += src_row_size;
        dstrow += dst_row_size;
        r += 1;
    }
}

inline void sepia(unsigned char *src, unsigned char *dst) {
    int tmp = src[0] + src[1] +  src[2];
    dst[0] = saturar3(tmp * BLUE);
    dst[1] = saturar3(tmp * GREEN);
    dst[2] = saturar3(tmp * RED);
}

inline unsigned char saturar3(int c) {
    if (c > UCHAR_MAX) {
        return UCHAR_MAX;
    }
    return c;
}
