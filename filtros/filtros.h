#ifndef __FILTROS__H__
#define __FILTROS__H__

void rotar_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);
void rotar_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);

void monocromatizar_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);
void monocromatizar_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);

void scale2x_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size); 
void scale2x_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);

void sepia_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size); 
void sepia_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);

void smalltiles_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size); 
void smalltiles_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);

void blur_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size); 
void blur_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);
#endif /* !__FILTROS__H__ */
