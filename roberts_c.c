#include "utils.h"

void roberts_c (unsigned char *src, unsigned char *dst, int h, int w, int row_size) {

    for (int y = 0; y < h - 1; y += 1) {
        for (int x = 0; x < w - 1; x += 1) {

            int first_row_start = y * row_size;
            int second_row_start = (y + 1) * row_size;

            int mask_value_y = src[first_row_start + x + 1]
                - src[second_row_start + x];

            int mask_value_x = src[first_row_start + x]
                - src[second_row_start + x + 1];

            unsigned char mask_value = saturar(abs(mask_value_x)
                + abs(mask_value_y));

            dst[first_row_start + x] = mask_value;
        }
    }

}
