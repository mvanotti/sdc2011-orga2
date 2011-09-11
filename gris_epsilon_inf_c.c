#include "utils.h"
#include "color.h"

void gris_epsilon_inf_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w ; x++) {
        
            int src_x = x * RGB_SIZE;
            int src_y = y * src_row_size;
            
            unsigned char r = src[src_y + (src_x + RGB_RED_OFFSET)];
            unsigned char g = src[src_y + (src_x + RGB_GREEN_OFFSET)];
            unsigned char b = src[src_y + (src_x + RGB_BLUE_OFFSET)];

            dst[y * dst_row_size + x] = max(max(r, g), b);
        }
    }
}

