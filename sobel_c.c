#include "utils.h"

void sobel_c (unsigned char *src, unsigned char *dst, int m, int n, int row_size) {
    /* Mask to apply: (horizontal, vertical)
    
       -1 0 1
       -2 0 2
       -1 0 1
       
       -1 -2 -1
       0  0  0
       1  2  1
    */
    for (int y = 1; y < m - 1; y += 1) {
        for (int x = 1; x < n - 1; x += 1) {
            int first_row_start = (y - 1) * row_size;
            int second_row_start = y * row_size;
            int third_row_start = (y + 1) * row_size;

            int mask_value_x = - src[first_row_start + x - 1]
                + src[first_row_start + x + 1]
                - 2 * src[second_row_start + x - 1]
                + 2 * src[second_row_start + x + 1]
                - src[third_row_start + x - 1]
                + src[third_row_start + x + 1];
            
            int mask_value_y = - src[first_row_start + x - 1]
                - 2 * src[first_row_start + x]
                - src[first_row_start + x + 1]
                + src[third_row_start + x - 1]
                + 2 * src[third_row_start + x]
                + src[third_row_start + x + 1];

            dst[second_row_start + x] = saturar(abs(mask_value_x)
                + abs(mask_value_y));
        }
    }
}
