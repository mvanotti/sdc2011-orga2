#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "filtros/filtros.h"


const int max_filter = 4;

/* filters es un arreglo de punteros a los filtros */


void (*filters_asm[])(unsigned char *, unsigned char *, int, int, int, int) =
	 {} ;


void (*filters_c[])(unsigned char *, unsigned char *, int, int, int, int) =
	 {} ;

void (**filters)(unsigned char *, unsigned char *, int, int, int) ;

char *filter_names[] = {"sobel", "prewitt", "roberts", "freichen"};


int main(void) {

	int filter_index = 0;
	filters = filters_asm;


	CvCapture *capture = NULL;
	IplImage *frame = NULL;	
	IplImage *buffer = NULL;
	IplImage *buffer2 = NULL;
	int key;


	char fpsstr[100];
	0[fpsstr] = 0;
	double fps; 
	int counter = 0; 


	struct timeval tv;
	struct timeval tv2;
	gettimeofday(&tv, (void *) NULL);


	capture = cvCaptureFromCAM(0);
	if (capture == NULL) {
		fprintf(stderr, "No se ha podido abrir la cam\n");
		exit(1);
	}

	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 640);
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 480);




	cvNamedWindow("filters", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	frame = cvQueryFrame(capture);	
	cvMoveWindow("original", frame->width + 10, 0);
	cvMoveWindow("filters", 0, 0);
	
	key = -1;
	while (key != 'q') {
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
			case -1:
				break;
			default: 
				filter_index += 1;
				filter_index %= max_filter;
				printf("Filtro actual:\t%s\n", filter_names[filter_index]);
		}

		/* Necesitamos pasar el frame capturado a blanco y negro
		   el problema es que la profundidad va a cambiar, entonces
			nos creamos una imagen con esa profundidad y canales y usamos
			cvCvtColor.
		 */
		buffer = cvCreateImage( cvSize(frame->width, frame->height), 
				IPL_DEPTH_8U,
				1);

		buffer2 = cvCreateImage( cvSize(frame->width, frame->height), 
				IPL_DEPTH_8U,
				1);

		filters[filter_index] ((unsigned char *) buffer->imageData, 
								(unsigned char *)  buffer2->imageData, 
								buffer->height, buffer->width, 
								buffer->widthStep);

		/* Una vez aplicado el filtro, agregamos los fps a la imagen */
		CvFont font;
    	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA);
    	cvPutText(buffer2, fpsstr , cvPoint(10, 30), 
			&font, cvScalar(255, 255, 255, 0));

		cvShowImage("original", frame);
		cvShowImage("filters", buffer2);


		/* Calculamos los fps */
		counter++;
		if (counter % 10 == 0) {
			gettimeofday(&tv2, (void *) NULL);

			fps = (10.0 * 1000000.0) / 
					(difftime(tv2.tv_sec, tv.tv_sec) * 1000000 + 
					 (tv2.tv_usec - tv.tv_usec));

			gettimeofday(&tv, (void *) NULL);
			sprintf(fpsstr, "fps: %.2f", fps);

		}

		key = cvWaitKey(1);
		cvReleaseImage(&buffer);
		cvReleaseImage(&buffer2);
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("original");
	cvDestroyWindow("filters");
	return 0; 	
		
}
