#include <limits.h>

void copiar_bordes (unsigned char *src, unsigned char *dst, int m, int n, int row_size) {        
    for(int j = 0; j<n; j++) {
        // superior
        dst[0*row_size+j] = src[0*row_size+j];
        // inferior
        dst[(m-1)*row_size+j] = src[(m-1)*row_size+j];
    }    
    
    for(int i = 0; i<m; i++) {
        // izquierdo
        dst[i*row_size+0] = src[i*row_size+0];
        // derecho
        dst[i*row_size+(n-1)] = src[i*row_size+(n-1)];
    }
}

unsigned char max(unsigned char a, unsigned char b) {
    return (a > b) ? a : b;
}

int abs(int x) {
    return (x > 0) ? x : -x;
}

unsigned char saturar(unsigned int x) {
    return (x > UCHAR_MAX) ? UCHAR_MAX : x;
}

float abs_double(float x) {
    return (x > 0) ? x : -x;
}

unsigned char saturar_double(float x) {
    return (x > UCHAR_MAX) ? UCHAR_MAX : x;
}
