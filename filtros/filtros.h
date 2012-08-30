#ifndef __FILTROS__H__
#define __FILTROS__H__


#define FILTRO(X) void X##_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size); void X##_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size); 
#define FILTRO_VAR(X, ...) void X##_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size, __VA_ARGS__); void X##_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size, __VA_ARGS__); 

#define NFILTROS 8
static char* filtros[NFILTROS] = {"rotar", "smalltiles", "blit", "monocromatizar", "sepia", "edge", "scale2x", "blur"};

FILTRO(monocromatizar)
FILTRO(blur)
FILTRO(scale2x)
FILTRO(edge)
FILTRO(sepia)
FILTRO(smalltiles)
FILTRO(rotar)
FILTRO_VAR(blit, unsigned char *blit, int bh, int bw, int b_row_size)


unsigned long long int aplicar(const char *filtro, int tiempo, int cant_iteraciones, const char *nomb_impl, const char* nomb_arch_entrada, const char* dir_salida);

#endif /* !__FILTROS__H__ */
