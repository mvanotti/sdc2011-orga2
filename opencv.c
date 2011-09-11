#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "filtros.h"


const int max_filter = 4;
const int max_camaras = 10; 

/* filters es un arreglo de punteros a los filtros */


void (*filters_asm[])(unsigned char *, unsigned char *, int, int, int) =
	 {sobel_asm, prewitt_asm, roberts_asm, freichen_asm} ;

void (*filters_c[])(unsigned char *, unsigned char *, int, int, int) =
	 {sobel_c, prewitt_c, roberts_c, freichen_c} ;

void (**filters)(unsigned char *, unsigned char *, int, int, int) ;

char *filter_names[] = {"sobel", "prewitt", "roberts", "freichen"};




void apply_filter(IplImage **src, void (*filter)(unsigned char *, unsigned char *, int, int, int));
IplImage *rgb2gray(IplImage *src);
double get_fps(struct timeval *tv, unsigned int frames);
void write_fps(IplImage *image , char *strfps);

int main(void) {

	int filter_index = 0;
	filters = filters_asm;
	int continue_while = 1;

	CvCapture *camaras[max_camaras];


	CvCapture *capture = NULL;
	IplImage *frame = NULL;	
	IplImage *buffer = NULL;
	int key;


	char fpsstr[100];
	0[fpsstr] = 0;
	int counter = 0; 


	struct timeval tv;
	gettimeofday(&tv, (void *) NULL);

/*	int i, k = 0; */

	capture = cvCaptureFromCAM(0);
	camaras[0] = capture;
	capture = camaras[0];
/*
	for (i = 0; i < max_camaras; i++) {
		camaras[k] = cvCaptureFromCAM(i);
		if (camaras[k] != NULL) {
			cvSetCaptureProperty( camaras[k], CV_CAP_PROP_FRAME_WIDTH, 640);
			cvSetCaptureProperty( camaras[k], CV_CAP_PROP_FRAME_HEIGHT, 480);
			k++;
		}
	}

	if (k == 0) {
		fprintf(stderr, "No se pudo abrir ninguna camara!\n");
		exit(1); 
	}
*/

	cvNamedWindow("filters", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	frame = cvQueryFrame(capture);	
	cvMoveWindow("original", frame->width + 10, 0);
	cvMoveWindow("filters", 0, 0);
	
	key = -1;
	while (continue_while) {
		frame = cvQueryFrame(capture);		

		if (frame == NULL) {
			break;
		}


		/* Si presionamos alguna tecla se cambia el filtro actual */
		switch (key) {
			case ' ':
				if ( filters == filters_c ) {
					filters = filters_asm;
				} else {
					filters = filters_c;
				}
				break;
			case 'q':
				continue_while = 0;
				break;			
			case -1:
				break;
			default: 
				filter_index += 1;
				filter_index %= max_filter;
				printf("Filtro actual:\t%s\n", filter_names[filter_index]);
		}


		/* Convertimos la imagen a blanco y negro usando nuestros filtros */
		buffer = rgb2gray(frame);

		/* Aplicamos el filtro correspondiente */
		apply_filter(&buffer, filters[filter_index]);


		/* Una vez aplicado el filtro, agregamos los fps a la imagen */
		write_fps(buffer, fpsstr);

		cvShowImage("original", frame);
		cvShowImage("filters", buffer);



		/* Calculamos los fps */
		counter++;
		if (counter % 10 == 0) {
			sprintf(fpsstr, "fps: %.2f", get_fps(&tv, 10));
		}

		key = cvWaitKey(1);
		cvReleaseImage(&buffer);
	//	cvReleaseImage(&buffer2);
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("original");
	cvDestroyWindow("filters");
	return 0; 	
		
}

void write_fps(IplImage *image , char *fpsstr) {
		CvFont font;
    	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA);
    	cvPutText(image, fpsstr , cvPoint(10, 30), 
			&font, cvScalar(255, 255, 255, 0));
}

double get_fps(struct timeval *tv, unsigned int frames) {
	struct timeval tv2;	

	gettimeofday(&tv2, (void *) NULL);

	double fps = (frames * 1000000.0) / 
					(difftime(tv2.tv_sec, tv->tv_sec) * 1000000 + 
					(tv2.tv_usec - tv->tv_usec));

	gettimeofday(tv, (void *) NULL);
	return fps;
}

IplImage *rgb2gray(IplImage *src) {
		IplImage *buffer = cvCreateImage( cvSize(src->width, src->height), 
							IPL_DEPTH_8U,
							1);

		if (buffer == NULL) {
			fprintf(stderr, "Error al crear la imagen en rgb2grayscale\n");
			exit(1);
		}

		gris_epsilon_uno_asm((unsigned char *) src->imageData,
							(unsigned char *) buffer->imageData,
							 src->height, src->width, src->widthStep, 
							buffer->widthStep);

		return buffer;
}

void apply_filter(IplImage **src, void (*filter)(unsigned char *, unsigned char *, int, int, int)) {

		IplImage *buffer = *src;
		IplImage *buffer2 = NULL;

		buffer2 = cvCreateImage( cvSize( buffer->width, buffer->height), 
				IPL_DEPTH_8U,
				1);

		if (buffer2 == NULL) {
			fprintf(stderr, "Error al aplicar el filtro");
			exit(1);
		}

		filter ((unsigned char *) buffer->imageData, 
				(unsigned char *) buffer2->imageData, 
				buffer->height, buffer->width, 
				buffer->widthStep);

		cvReleaseImage(src);
		*src = buffer2;		
}


