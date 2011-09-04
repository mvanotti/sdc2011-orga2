#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "filtros.h"


int main(void) {
	CvCapture *capture = NULL;
	IplImage *frame = NULL;	
	IplImage *buffer = NULL;
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

	cvNamedWindow("webcam", CV_WINDOW_AUTOSIZE);
	
	while (key != 'q') {
		frame = cvQueryFrame(capture);		

		if (frame == NULL) {
			break;
		}

		/* Necesitamos pasar el frame capturado a blanco y negro
		   el problema es que la profundidad va a cambiar, entonces
			nos creamos una imagen con esa profundidad y canales y usamos
			cvCvtColor.
		 */
		buffer = cvCreateImage( cvSize(frame->width, frame->height), 
				IPL_DEPTH_8U,
				1);

		cvCvtColor( frame, buffer, CV_RGB2GRAY);

		frame = cvCreateImage( cvSize(frame->width, frame->height), 
				IPL_DEPTH_8U,
				1);

		sobel_asm (buffer->imageData, frame->imageData, buffer->height, 
					buffer->width, buffer->widthStep);

		/* Una vez aplicado el filtro, agregamos los fps a la imagen */
		CvFont font;
    	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA);
    	cvPutText(frame, fpsstr , cvPoint(10, 30), 
			&font, cvScalar(255, 255, 255, 0));

		cvShowImage("webcam", frame);


		/* Calculamos los fps */
		counter++;
		if (counter % 10 == 0) {
			gettimeofday(&tv2, (void *) NULL);

			fps       =  (10.0 * 1000000.0) / (difftime(tv2.tv_sec, tv.tv_sec) * 1000000 +  (tv2.tv_usec - tv.tv_usec));
			gettimeofday(&tv, (void *) NULL);
			printf("\b\b\b\b");
			printf("fps: %.2f", fps);
			sprintf(fpsstr, "fps: %.2f", fps);

		}

		key = cvWaitKey(1);
		cvReleaseImage(&buffer);
		cvReleaseImage(&frame);
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("webcam");
	return 0; 	
		
}
