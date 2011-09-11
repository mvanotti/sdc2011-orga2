#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "filtros.h"


/* Macro mágico para definir un puntero a una función de filtro */
#define FILTER_FUNCTION(X) void (*(X))(unsigned char *, unsigned char *, int, int, int)

const int max_filter = 4;


/* Son arreglos de punteros a filtros */
FILTER_FUNCTION(filters_asm[]) = {sobel_asm, prewitt_asm, roberts_asm, freichen_asm} ;
FILTER_FUNCTION(filters_c[]) = {sobel_c, prewitt_c, roberts_c, freichen_c} ;

/* filters apunta a filters_asm o a filters_c */
FILTER_FUNCTION(*filters);

char *filter_names[] = {"sobel", "prewitt", "roberts", "freichen"};


void apply_filter(IplImage **src, FILTER_FUNCTION(filter));
IplImage *rgb2gray(IplImage *src);
double get_fps(struct timeval *tv, unsigned int frames);
void write_fps(IplImage *image , char *strfps);

int main(void) {

	int filter_index = 0;
	filters = filters_asm;
	int continue_while = 1;

	CvCapture *capture = NULL;
	IplImage *frame = NULL;	
	IplImage *buffer = NULL;
	int key;


	char fpsstr[100];
	0[fpsstr] = 0;
	int counter = 0; 


	struct timeval tv;
	gettimeofday(&tv, (void *) NULL);


/* Cargamos informacion necesaria del archivo de config */
	FILE *config = fopen("config.conf", "r");
	if (config == NULL) {
		fprintf(stderr, "Error al abrir el archivo!\n");
	}

	int device = 0;
	int height = 0;
	int width = 0; 

	fscanf(config, "%d %d %d", &device, &width, &height);

	if (feof(config)) fprintf(stderr, "Lectura inválida\n");

	fclose(config);
	capture = cvCaptureFromCAM(device);
	
	if (capture == NULL) {
		fprintf(stderr, "No se pudo abrir la camara %d\n", device);
		exit(1);
	}

	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width);
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height);
	cvNamedWindow("filtros", CV_WINDOW_AUTOSIZE);
	printf("Se agregó la camara: %d con resolucion %d x %d\n", device, width, height);



	frame = cvQueryFrame(capture);	
	cvMoveWindow("filtros", 0, 0);
	
	key = -1;
	while (continue_while) {
		
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

		frame = cvQueryFrame(capture);		

		if (frame == NULL) {
			break;
		}


		/* Convertimos la imagen a blanco y negro usando nuestros filtros */
		buffer = rgb2gray(frame);

		/* Aplicamos el filtro correspondiente */
		apply_filter(&buffer, filters[filter_index]);

		/* Una vez aplicado el filtro, agregamos los fps a la imagen */
		write_fps(buffer, fpsstr);

		cvShowImage("filtros", buffer);
		cvReleaseImage(&buffer);

		key = cvWaitKey(1);

		/* Calculamos los fps */
		counter++;
		if (counter % 10 == 0) {
			sprintf(fpsstr, "fps: %.2f", get_fps(&tv, 10));
		}
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("filtros");
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


