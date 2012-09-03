#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "filtros/filtros.h"
#include "listafiltro.h"

#define ASM 1
#define C 0

int actual = ASM;

const int max_filter = 1;

lista_filtro *lista = NULL;

void clean_screen();
char fpsstr[100];

void (*filters_asm[])(unsigned char *, unsigned char *, int, int, int, int) =
	 {rotar_asm} ;


void (*filters_c[])(unsigned char *, unsigned char *, int, int, int, int) =
	 {rotar_c} ;

void (**filters)(unsigned char *, unsigned char *, int, int, int, int) ;

char *filter_names[] = {"rotar"};

void print_everything();

int main(void) {
	nfiltro *it = NULL;

	lista = crear();

	CvCapture *capture = NULL;
	IplImage *frame = NULL;	
	IplImage *buffer = NULL;
	IplImage *buffer2 = NULL;
	IplImage *bufferTMP = NULL;
	int key;

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

	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 1366);
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 720);

	cvNamedWindow("filters", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	frame = cvQueryFrame(capture);	
	cvMoveWindow("original", frame->width + 10, 0);
	cvMoveWindow("filters", 0, 0);

    buffer = cvCreateImage( cvSize(frame->width, frame->height), 
        IPL_DEPTH_8U,
        4);
    

    buffer2 = cvCreateImage( cvSize(frame->width, frame->height), 
        IPL_DEPTH_8U,
        4);

	
	key = -1;
	while (key != 'z') {
		frame = cvQueryFrame(capture);		

		if (frame == NULL) {
			break;
		}
		/* Si presionamos alguna tecla se cambia el filtro actual */
		/* Comandos: 
			s - cambia implementación
			q - agrega filtro rotar
			w - agrega filtro scale2x
			e - agrega filtro monocromatizar
			r - agrega filtro sepia
			t - agrega filtro smalltiles
			y - agrega filtro blur
			d - borra el filtro actual
			f - pasa al siguiente filtro
			g - pasa al filtro anterior
		*/
		if (key != -1) 	{
			switch ((char)key) {

				case 's':
					if ( actual == ASM) {
						actual = C;
					} else {
						actual = ASM;
					}
					break;
				case 'q':
					agregar(lista, rotar_asm, rotar_c, "rotar");
					print_everything();
					break;
				case 'w':
					agregar(lista, scale2x_asm, scale2x_c, "scale2x");
					print_everything();
					break;
				case 'e':
					agregar(lista, monocromatizar_asm, monocromatizar_c, "monocromatizar");
					print_everything();
					break;
				case 'r':
					agregar(lista, sepia_asm, sepia_c, "sepia");
					print_everything();
					break;
				case 't':
					agregar(lista, smalltiles_asm, smalltiles_c, "smalltiles");
					print_everything();
					break;
				case 'y':
					agregar(lista, blur_asm, blur_c, "blur");
					print_everything();
					break;
				case 'd':
					remover(lista);
					print_everything();
					break;
				case 'g':
					avanzar(lista);
					print_everything();
					break;
				case 'f':
					retroceder(lista);
					print_everything();
					break;
				default:
					break;
			}
		}

		//clean_screen();
		//Calculamos los fps 
		counter++;
		if (counter % 10 == 0) {
			gettimeofday(&tv2, (void *) NULL);

			fps = (10.0 * 1000000.0) / 
					(difftime(tv2.tv_sec, tv.tv_sec) * 1000000 + 
					 (tv2.tv_usec - tv.tv_usec));

			gettimeofday(&tv, (void *) NULL);
			sprintf(fpsstr, "fps: %.2f  ", fps);
			print_everything();
		}

		/* aplicamos los filtros */
        cvCvtColor( frame, buffer, CV_RGB2RGBA);
		for (it = lista->pri; it != NULL; it = it->sig) {
            
			if (actual == ASM) {
				it->asmf((unsigned char *) buffer->imageData, 
								(unsigned char *)  buffer2->imageData, 
								buffer->height, buffer->width, 
								buffer->widthStep, buffer2->widthStep);
			} else {
				it->cf((unsigned char *) buffer->imageData, 
								(unsigned char *)  buffer2->imageData, 
								buffer->height, buffer->width, 
								buffer->widthStep, buffer2->widthStep);
			}

            bufferTMP = buffer;
            buffer = buffer2;
            buffer2 = bufferTMP;
		}
		fflush(stdout);
				

		/* Una vez aplicado el filtro, agregamos los fps a la imagen */
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA);
		//cvPutText(buffer2, fpsstr , cvPoint(10, 30), 
	//		&font, cvScalar(255, 255, 255, 0));

		cvShowImage("original", frame);
		cvShowImage("filters", buffer);


	    key = cvWaitKey(1);
		if (key == -1) {
		}
	}

	cvReleaseImage(&buffer);
	cvReleaseImage(&buffer2);
	cvReleaseCapture(&capture);
	cvDestroyWindow("original");
	cvDestroyWindow("filters");
	return 0; 	
		
}

void print_everything() {
	nfiltro *it = NULL;
	clean_screen();
	printf(actual ? "ASM " : "C ");
	printf(fpsstr);
	printf("\n");
	for (it = lista->pri; it != NULL; it = it->sig) {
		if (it == lista->actual) {
			printf("*");
		}
		printf(it->nombre);
		printf("  ");
	}
	fflush(stdout);
}
void clean_screen() {
	printf("\033[2J\033[1;1H");
}
