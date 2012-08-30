#include <stdio.h>

#define pos(X, Y) (unsigned int) src[src_row_size * (Y) + (X) * PIXEL_SIZE + i]
#define PIXEL_SIZE 4

void blur_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {

    unsigned int dstrow = 0;
    int y = 2;
    dstrow += dst_row_size * 2;

    while (y < h - 2) {

        for (int x = 2; x < w - 2; x++) {
            unsigned char *pdst = &dst[dstrow + (x * PIXEL_SIZE)];
            for (unsigned int i = 0; i < PIXEL_SIZE - 1; i++) {
                pdst[i] = ( pos(x,y-2) + pos(x-1,y-1) + pos(x,y-1) +
                        pos(x+1,y-1) + pos(x-2,y) + pos(x-1,y) +
                        pos(x,y) + pos(x+1,y) + pos(x+2,y) +
                        pos(x-1,y+1) + pos(x,y+1) + pos(x+1,y+1) +
                        pos(x,y+2) ) / 13;
            }
        }

        dstrow += dst_row_size;
        y++;
    }
}



