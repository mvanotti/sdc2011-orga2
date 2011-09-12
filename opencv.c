#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "filtros.h"
#include <stdlib.h>
#include <pthread.h>


/* Macro mágico para definir un puntero a una función de filtro */
#define FILTER_FUNCTION(X) void (*(X))(unsigned char *, unsigned char *, int, int, int)

const int max_filter = 4;


/* Son arreglos de punteros a filtros */
FILTER_FUNCTION(filters_asm[]) = {sobel_asm, prewitt_asm, roberts_asm, freichen_asm} ;
FILTER_FUNCTION(filters_c[]) = {sobel_c, prewitt_c, roberts_c, freichen_c} ;

/* filters apunta a filters_asm o a filters_c */
FILTER_FUNCTION(*filters);


void *show_cam(void *dev) ;

char *filter_names[] = {"sobel", "prewitt", "roberts", "freichen"};


void apply_filter(IplImage **src, FILTER_FUNCTION(filter));
IplImage *rgb2gray(IplImage *src);
double get_fps(struct timeval *tv, unsigned int frames);
void write_fps(IplImage *image , char *strfps);


struct cam_t {
	int device;
	CvCapture *capture;
	int width;
	int height;
	char window[2];
};

struct cam_t cams[10];
pthread_t threads[10];


pthread_mutex_t mutex_filters = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_filter_index = PTHREAD_MUTEX_INITIALIZER;
int	filter_index = 0;

int main(void) {

	int k = 0;
/* Cargamos informacion necesaria del archivo de config */
	FILE *config = fopen("config.conf", "r");
	if (config == NULL) {
		fprintf(stderr, "Error al abrir el archivo!\n");
	}


	while (!feof(config) && k < 10) {

		int device = 0;
		int height = 0;
		int width = 0; 

		fscanf(config, "%d %d %d", &device, &width, &height);
		if (feof(config)) break;
		
		cams[k].device = device;
		cams[k].width = width;
		cams[k].height = height;
		cams[k].window[0] = '0' + device;
		cams[k].window[1] = '\0';
		cams[k].capture = NULL;

		k += 1;
	}
	fclose(config);

	filters = filters_asm;
	for (int i = 0; i < k; i++) {
		cams[i].capture = cvCaptureFromCAM(cams[i].device);
		if (cams[i].capture == NULL) {
			fprintf(stderr, "Error al abrir la cámara %d\n", cams[i].device);
			exit(1);
		}

		cvSetCaptureProperty( cams[i].capture, CV_CAP_PROP_FRAME_WIDTH, 
								cams[i].width);
		cvSetCaptureProperty( cams[i].capture, CV_CAP_PROP_FRAME_HEIGHT, 
								cams[i].height);	

		cvNamedWindow(cams[i].window, CV_WINDOW_AUTOSIZE);
		pthread_create(&threads[i], NULL, show_cam, (void *) &cams[i]);

		printf("Se agregó la camara: %d con resolucion %d x %d\n", 
					cams[i].device, cams[i].width, cams[i].height);
	}

	for (int i = 0; i < k; i++) {
		pthread_join( threads[i], NULL); 
		cvReleaseCapture(&(cams[i].capture));
		cvDestroyWindow(cams[i].window);
	}	 

	return 0; 	
		
}


void *show_cam(void *dev) {

	struct cam_t *cam = (struct cam_t *) dev;
	IplImage *frame = NULL;	
	IplImage *buffer = NULL;	

	int	key = -1;
	int continue_while = 1;

	while (continue_while) {
		
		/* Si presionamos alguna tecla se cambia el filtro actual */
		switch (key) {
			case 'q':
				continue_while = 0;
				break;		
			case -1:
				break;
			case ' ':
				pthread_mutex_lock( &mutex_filters );
				if (filters == filters_asm) {
					filters = filters_c;
					printf("Se cambiaron los filtros a la version en C\n");
				} else {
					filters = filters_asm;
					printf("Se cambiaron los filtros a la version en ASM\n");
				}
				pthread_mutex_unlock( &mutex_filters );
			default:
				pthread_mutex_lock( &mutex_filter_index );
					filter_index = (filter_index + 1) % max_filter;
					printf("Filtro actual: %s\n", filter_names[filter_index]);
				pthread_mutex_unlock( &mutex_filter_index );

				break;
		}

		frame = cvQueryFrame(cam->capture);		

		if (frame == NULL) {
			break;
		}


		/* Convertimos la imagen a blanco y negro usando nuestros filtros */
		buffer = rgb2gray(frame);

		/* Aplicamos el filtro correspondiente */
		apply_filter(&buffer, filters[filter_index]);

		cvShowImage(cam->window, buffer);
		cvReleaseImage(&buffer);
		key = cvWaitKey(1);
	}

	return NULL;
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


