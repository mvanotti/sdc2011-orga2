#include <stdbool.h>
#define PIXEL_SIZE 4

void copy_pixel(unsigned char *, unsigned char *);
bool equal_pixel(unsigned char *, unsigned char *);

void scale2x_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {

    int row = 1;
    int col = 1;

    while (row < h / 2 + 1) {
        src += src_row_size;
        for (col = 1; col < w / 2 + 1; col++) {
            unsigned char *current_pixel = src + col * PIXEL_SIZE;

            unsigned char *pixel_above = current_pixel - src_row_size;
            unsigned char *pixel_below = current_pixel + src_row_size;
            unsigned char *pixel_left = current_pixel - PIXEL_SIZE;
            unsigned char *pixel_right = current_pixel + PIXEL_SIZE;

            unsigned char *e[4];
            unsigned char *dst_e[4];

            for (unsigned int i = 0; i < 4; i++) {
                e[i] = current_pixel;
            }

            dst_e[0] = dst + (col - 1) * 2 * PIXEL_SIZE;
            dst_e[1] = dst + ((col - 1) * 2 + 1) * PIXEL_SIZE;
            dst_e[2] = dst_e[0] + dst_row_size;
            dst_e[3] = dst_e[1] + dst_row_size;

            if (!equal_pixel(pixel_above, pixel_below) && 
                    !equal_pixel(pixel_left, pixel_right)) {
                if (equal_pixel(pixel_left, pixel_above)) {
                    e[0] = pixel_left;
                }

                if (equal_pixel(pixel_above, pixel_right)) {
                    e[1] = pixel_right;
                }
                
                if (equal_pixel(pixel_left, pixel_below)) {
                    e[2] = pixel_left;
                }

                if (equal_pixel(pixel_below, pixel_right)) {
                    e[3] = pixel_right;
                }
            }

            for (unsigned int i = 0; i < 4; i++) {
                copy_pixel(e[i], dst_e[i]);
            }
        }

        dst += dst_row_size * 2;
        row += 1;
    }
}

inline void copy_pixel(unsigned char *src, unsigned char *dst) {
    for (unsigned int i = 0; i < PIXEL_SIZE; i++) {
        dst[i] = src[i];
    }
}

inline bool equal_pixel(unsigned char *a, unsigned char *b) {
    for (unsigned int i = 0; i < PIXEL_SIZE; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}
