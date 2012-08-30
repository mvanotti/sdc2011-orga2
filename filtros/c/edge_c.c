#include <stdio.h>
#include <limits.h>

unsigned char saturar(int);

inline unsigned char saturar(int val) {
    if (val > UCHAR_MAX)
        return UCHAR_MAX;
    if (val < 0)
        return 0;

    return val;
}

void edge_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {
    for (int y = 1; y < h; y += 1) {
        for (int x = 1; x < w; x += 1) {
            int first_row_start = (y - 1) * src_row_size;
            int second_row_start = y * src_row_size;
            int third_row_start = (y + 1) * src_row_size;

            int col1 = src[first_row_start + x - 1] / 2 +
                                src[second_row_start + x - 1] +
                                src[third_row_start + x - 1] / 2;

            int col2 = src[first_row_start + x] +
                                src[second_row_start + x] * ( -6) +
                                src[third_row_start + x];

            int col3 = src[first_row_start + x + 1] / 2 +
                                src[second_row_start + x + 1] +
                                src[third_row_start + x + 1] / 2;

            dst[(dst_row_size * y) + x] = saturar(col1 + col2 + col3);
        }
    }
}


