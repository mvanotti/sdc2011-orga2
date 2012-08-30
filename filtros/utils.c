#include <utils.h>
#include <stdlib.h>


distancias* bindiff (const char *arch1, const char *arch2) {
	distancias * resultado = (distancias*) malloc(sizeof(distancias));
	
	IplImage *a = 0;
	IplImage *b = 0;
	
	resultado->pixeles = 0;
	resultado->sumdif = 0;
	resultado->maxdif = 0;
	resultado->radio = 0.0;
	resultado->difpixeles = 0;
	
	a = cvLoadImage (arch1, CV_LOAD_IMAGE_UNCHANGED);
	if (a == 0) {
		resultado->pixeles = -1;
		return resultado;
	}
	b = cvLoadImage (arch2, CV_LOAD_IMAGE_UNCHANGED);
	if (b == 0) {
		resultado->pixeles = -1;
		return resultado;
	}
	
	if (a->width != b->width ||
		a->height != b->height ||
		a->depth != b->depth ||
		a->nChannels != b->nChannels) {
		resultado->pixeles = -1;
		return resultado;
	}
	
	unsigned char * da = (unsigned char*)a->imageData;
	unsigned char * db = (unsigned char*)b->imageData;
	
	
	unsigned long p1;
	unsigned long p2;
	
	unsigned int ars = a->widthStep;
	unsigned int brs = b->widthStep;
	
	long dist;
	
	for (unsigned int f = 0; f < a->height; f++) {
		for (unsigned int c = 0; c < a->width*a->nChannels; c+=a->nChannels) {
			p1 = 0;
			p2 = 0;
			for (unsigned int ca = 0; ca < a->nChannels; ca++) {
				p1 += da[f * ars + c + ca] << ca*8;
				p2 += db[f * brs + c + ca] << ca*8;
			}
			resultado->pixeles++;
			if (p1 != p2) {
				resultado->difpixeles++;
				dist = 0;
				for (unsigned int ca = 0; ca < a->nChannels; ca++) {
					dist += abs(da[f * ars + c + ca] - db[f * brs + c + ca]);
				}
				if (resultado->maxdif < dist) {
					resultado->maxdif = dist;
				}
				resultado->sumdif += dist;
			}
		}
	}
	
	resultado->radio = (double)resultado->sumdif/(double)(resultado->pixeles*a->nChannels);
	resultado->porcentaje = (double)resultado->difpixeles/(double)(resultado->pixeles);
	
	cvReleaseImage(&a);
	cvReleaseImage(&b);
	
	return resultado;
	
}

bgra_img * crearDesde(IplImage * original) {
	int w = original->width;
	int	channels = original->nChannels == 3 ? 4 : 1;
	int	ochannels = original->nChannels;
	int sw = original->width * channels;
	int h = original->height;
	
	
	while (sw % 16 != 0) {
		sw++;
	}
	
	unsigned char * data = (unsigned char *)malloc(h * sw);
	unsigned char * odata = (unsigned char *)original->imageData;
	int osw = original->widthStep;
	
	for (unsigned int f = 0; f < h; f++) {
		for (unsigned int co = 0, c=0; co < w*ochannels; co+=original->nChannels, c+=channels) {
			for (int ca = 0; ca < ochannels; ca++){
				data[f * sw + c + ca] = odata[f * osw + co + ca];
			}
			for (int ca = ochannels; ca < channels; ca++){
				data[f * sw + c + ca] = 0;
			}
		}
	}
	
	bgra_img * resultado = (bgra_img*) malloc(sizeof(bgra_img));
	
	resultado->width = w;
	resultado->height = h;
	resultado->widthStep = sw;
	resultado->imageData = data;
	resultado->original = original;
	resultado->nChannels = channels;
	
	return resultado;
	
}
void guardar(bgra_img* imagen) {
	int w = imagen->width;
	int	channels = imagen->nChannels;
	int	ochannels = imagen->original->nChannels;
	int sw = imagen->widthStep;
	int h = imagen->height;
	unsigned char * data = imagen->imageData;
	unsigned char * odata = (unsigned char *)imagen->original->imageData;
	int osw = imagen->original->widthStep;
	
	for (unsigned int f = 0; f < h; f++) {
		for (unsigned int co = 0, c=0; co < w*ochannels; co+=ochannels, c+=channels) {
			for (int ca = 0; ca < ochannels; ca++){
				odata[f * osw + co + ca] = data[f * sw + c + ca];
			}
		}
	}
	
}

void liberar(bgra_img** imagen) {
	free((*imagen)->imageData);
	free(*imagen);
	*imagen = NULL;
}


void freedist(distancias * dist) {
	free(dist);
}
