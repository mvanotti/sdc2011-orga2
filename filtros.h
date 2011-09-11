#ifndef __FILTROS__H__
#define __FILTROS__H__

void roberts_asm (unsigned char *src, unsigned char *dst, int m, int n, int row_size);
void prewitt_asm (unsigned char *src, unsigned char *dst, int m, int n, int row_size);
void sobel_asm (unsigned char *src, unsigned char *dst, int m, int n, int row_size);
void freichen_asm (unsigned char *src, unsigned char *dst, int m, int n, int row_size);

void gris_epsilon_uno_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);

void roberts_c (unsigned char *src, unsigned char *dst, int m, int n, int row_size);
void prewitt_c (unsigned char *src, unsigned char *dst, int m, int n, int row_size);
void sobel_c (unsigned char *src, unsigned char *dst, int m, int n, int row_size);
void freichen_c (unsigned char *src, unsigned char *dst, int m, int n, int row_size);

#endif /* !__FILTROS__H__ */
