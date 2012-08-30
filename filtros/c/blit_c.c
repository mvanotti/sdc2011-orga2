#include<string.h>

#define PIXEL_SIZE 4

static int esmagenta (unsigned char *);
void blit_c (unsigned char *src, unsigned char *dst, int h, int w, 
int src_row_size, int dst_row_size, unsigned char *blit, int bh, 
int bw, int b_row_size) {
	
	// Copiamos toda la imagen, vamos linea por linea porque no
	// podemos asegurar nada sobre el padding al final de cada fila
	for (unsigned int i = 0; i < h; i++) {
		unsigned int dstrow = i * dst_row_size;
		unsigned int srcrow = i * src_row_size;
		memcpy(dst + dstrow, src + srcrow, w * PIXEL_SIZE);
	}
	
	// Fila y columna inicial del blit sobre nuestro destino
	unsigned int blith = h - bh;
	unsigned int blitw = w - bw;
	
	for (unsigned int i = 0; i < bh; i++) {
		unsigned int dstrow = (blith + i) * dst_row_size;
		unsigned int srcrow = (i * b_row_size);
		
		for (unsigned int j = 0; j < bw; j += 1){
			unsigned int dstpos = dstrow + (blitw + j) * PIXEL_SIZE;
			unsigned int srcpos = srcrow + j * PIXEL_SIZE;
			if (esmagenta(blit + srcpos) == 0 ) {
				memcpy(dst + dstpos, blit + srcpos, PIXEL_SIZE);
			}
		} 
	}
}

inline static int esmagenta(unsigned char *pix){
	if(pix[0] == 255 && pix[1] == 0 && pix[2] == 255){
		return 1;
	} else {
		return 0;
	}	
}
