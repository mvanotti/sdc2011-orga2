#ifndef __UTILS__H__
#define __UTILS__H__

void copiar_bordes (unsigned char *src, unsigned char *dst, int m, int n, int row_size);
unsigned char max(unsigned char a, unsigned char b);

int abs(int x);
unsigned char saturar(unsigned int x);

float abs_double(float x);
unsigned char saturar_double(float x);

#define SQRT_2 1.4142135623730950488016887

#endif /* !__UTILS__H__ */
