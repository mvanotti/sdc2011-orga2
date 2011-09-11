#include "color.h"
#include "utils.h"

void gris_epsilon_uno_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {

    for (int y = 0; y < h; y += 1) {
        for (int x = 0; x < w; x += 1) {

            int row_start = y * src_row_size;
            unsigned char b = src[row_start + x * RGB_SIZE + RGB_BLUE_OFFSET];
            unsigned char g = src[row_start + x * RGB_SIZE + RGB_GREEN_OFFSET];
            unsigned char r = src[row_start + x * RGB_SIZE + RGB_RED_OFFSET];
            
           dst[y * dst_row_size + x] = saturar((r + 2 * g + b) / 4);
        }
    }
}
