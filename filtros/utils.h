#ifndef __UTILS__H__
#define __UTILS__H__

#include <highgui.h>

typedef struct bgra_img_t {
	unsigned char 	* imageData;
	IplImage * original;
	int		height;
	int		width;
	int		widthStep;
	int		nChannels;
} bgra_img;
	
typedef struct distancias_t {
	unsigned long pixeles;
	unsigned long sumdif;
	unsigned long maxdif;
	unsigned long difpixeles;
	double radio;
	double porcentaje;
} distancias;

distancias* bindiff (const char *arch1, const char *arch2);

bgra_img * crearDesde(IplImage *);
void guardar(bgra_img*);
void liberar(bgra_img**);


void freedist(distancias *);

#endif /* !__UTILS__H__ */
