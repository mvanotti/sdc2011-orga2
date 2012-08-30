#include <getopt.h>
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "filtros.h"
#include "tiempo.h"
#include "utils.h"


#ifdef DEBUG
#define DLOG(...) printf("DEBUG:: "); printf(__VA_ARGS__);
#else
#define DLOG(...)
#endif


const char* nombre_programa;

void imprimir_ayuda ( );
void imprimir_tiempos_ejecucion(unsigned long long int start, unsigned long long int end, int cant_iteraciones);

int main( int argc, char** argv ) {
	int siguiente_opcion;

	// Opciones
	const char* const op_cortas = "hi:vt:";

	const struct option op_largas[] = {
		{ "help", 0, NULL, 'h' },
		{ "implementacion", 1, NULL, 'i' },
		{ "verbose", 0, NULL, 'v' },
		{ "tiempo", 1, NULL, 't' },
		{ NULL, 0, NULL, 0 }
	};

	// Parametros
	const char* nombre_implementacion = NULL;
	int cant_iteraciones = 0;

	// Flags de opciones
	int verbose = 0;
	int tiempo = 0;

	// Guardar nombre del programa para usarlo en la ayuda
	nombre_programa = argv[0];

	// Si se ejecuta sin parametros ni opciones
	if (argc == 1) {
		imprimir_ayuda ( );

		exit ( EXIT_SUCCESS );
	}

	// Procesar opciones
	while (1) {
		siguiente_opcion = getopt_long ( argc, argv, op_cortas, op_largas, NULL);

		// No hay mas opciones
		if ( siguiente_opcion == -1 )
			break;

		// Procesar opcion
		switch ( siguiente_opcion ) {
			case 'h' : /* -h o --help */
				imprimir_ayuda ( );
				exit ( EXIT_SUCCESS );
				break;
			case 'i' : /* -i o --implementacion */
				nombre_implementacion = optarg;
				break;
			case 't' : /* -t o --tiempo */
				tiempo = 1;
				cant_iteraciones = atoi ( optarg );
				break;
			case 'v' : /* -v o --verbose */
				verbose = 1;
				break;
			case '?' : /* opcion no valida */
				imprimir_ayuda ( );
				exit ( EXIT_SUCCESS );
			default : /* opcion no valida */
				abort ( );
		}
	}

	// Verifico nombre del proceso
	char *nomb_proceso = argv[optind++];
	

	if (nomb_proceso == NULL) {
		imprimir_ayuda ( );
		exit ( EXIT_SUCCESS );
	}
	
	int okproceso = 0;
	for (int i = 0; i < NFILTROS; i++) {
		if (strcmp(nomb_proceso, filtros[i])) {
			okproceso = 1;
		}
	}
	
	if (!okproceso) {
		imprimir_ayuda ( );
		exit ( EXIT_SUCCESS );
	}
	
	

	// Verifico nombre de la implementacion
	if (nombre_implementacion == NULL ||
		(strcmp(nombre_implementacion, "c") != 0 &&
		strcmp(nombre_implementacion, "asm") != 0)) {
		imprimir_ayuda ( );

		exit ( EXIT_SUCCESS );
	}

	// Verifico nombre de archivo
	const char *nomb_arch_entrada = argv[optind++];

	if (nomb_arch_entrada == NULL) {
		imprimir_ayuda ( );

		exit ( EXIT_SUCCESS );
	}

	if (access( nomb_arch_entrada, F_OK ) == -1) {
		printf("Error al intentar abrir el archivo: %s.\n", nomb_arch_entrada);
		
		exit ( EXIT_SUCCESS );
	}


	// Imprimo info
	if ( verbose ) {
		printf ( "Procesando imagen...\n");
		printf ( "  Filtro             : %s\n", nomb_proceso);
		printf ( "  Implementación     : %s\n", nombre_implementacion);
		printf ( "  Archivo de entrada : %s\n", nomb_arch_entrada);
	}

	// Procesar imagen
	aplicar(nomb_proceso, tiempo, cant_iteraciones, nombre_implementacion, nomb_arch_entrada, NULL);


	return 0;
}

void imprimir_ayuda ( ) {
	printf ( "Uso: %s opciones filtro nombre_archivo_entrada parametros_filtro                                            \n", nombre_programa );
	printf ( "    Los filtros que se pueden aplicar son:                                                                  \n" );
	for (int i = 0; i < NFILTROS; i++ ){
		printf ( "       * %s                                                                                           \n" , filtros[i]);
	}
	printf ( "                                                                                                            \n" );
	printf ( "    -h, --help                                 Imprime esta ayuda                                           \n" );
	printf ( "                                                                                                            \n" );
	printf ( "    -i, --implementacion NOMBRE_MODO           Implementación sobre la que se ejecutará el filtro           \n" );
	printf ( "                                               seleccionado. Los implementaciones disponibles               \n" );
	printf ( "                                               son: c, asm                                                  \n" );
	printf ( "                                                                                                            \n" );
	printf ( "    -t, --tiempo CANT_ITERACIONES              Mide el tiempo que tarda en ejecutar el filtro sobre la      \n" );
	printf ( "                                               imagen de entrada una cantidad de veces igual a              \n" );
	printf ( "                                               CANT_ITERACIONES                                             \n" );
	printf ( "                                                                                                            \n" );
	printf ( "    -v, --verbose                              Imprime información adicional                                \n" );
	printf ( "                                                                                                            \n" );
}

void imprimir_tiempos_ejecucion(unsigned long long int start, unsigned long long int end, int cant_iteraciones) {
	unsigned long long int cant_ciclos = end-start;
	
	printf("Tiempo de ejecución:\n");
	printf("  Comienzo                          : %llu\n", start);
	printf("  Fin                               : %llu\n", end);
	printf("  # iteraciones                     : %d\n", cant_iteraciones);
	printf("  # de ciclos insumidos totales     : %llu\n", cant_ciclos);
	printf("  # de ciclos insumidos por llamada : %.3f\n", (float)cant_ciclos/(float)cant_iteraciones);	
}

unsigned long long int aplicar(const char *filtro, int tiempo, int cant_iteraciones, const char *nomb_impl, const char *nomb_arch_entrada, const char *dir_salida) {
	printf("DEBUG::aplicar -> filtro %s para %s en %s\n", filtro, nomb_arch_entrada, nomb_impl);

	IplImage *srccv = 0;
	IplImage *dstcv = 0;
	IplImage *blitcv = 0;
	
	bgra_img *blit = 0;
	bgra_img *src = 0;
	bgra_img *dst = 0;


	typedef void (fn_t) (unsigned char*, unsigned char*, int, int, int, int);
	typedef void (blit_fn_t) (unsigned char*, unsigned char*, int, int, int, int, unsigned char*, int, int, int);

	fn_t *proceso;
	blit_fn_t *proceso_blit;
	int caso = 0;
	int origen = CV_LOAD_IMAGE_COLOR;
	int destino = 3;
	
	if (strcmp(filtro, "monocromatizar") == 0) {
		if (strcmp(nomb_impl, "c") == 0) {
			proceso = monocromatizar_c;
		} else {
			proceso = monocromatizar_asm;
		}
	} else if (strcmp(filtro, "blur") == 0) {
		if (strcmp(nomb_impl, "c") == 0) {
			proceso = blur_c;
		} else {
			proceso = blur_asm;
		}
	} else if (strcmp(filtro, "scale2x") == 0) {
		if (strcmp(nomb_impl, "c") == 0) {
			proceso = scale2x_c;
		} else {
			proceso = scale2x_asm;
		}
	} else if (strcmp(filtro, "edge") == 0) {
		origen = CV_LOAD_IMAGE_GRAYSCALE;
		destino = 1;
		if (strcmp(nomb_impl, "c") == 0) {
			proceso = edge_c;
		} else {
			proceso = edge_asm;
		}
	} else if (strcmp(filtro, "sepia") == 0) {
		if (strcmp(nomb_impl, "c") == 0) {
			proceso = sepia_c;
		} else {
			proceso = sepia_asm;
		}
	} else if (strcmp(filtro, "smalltiles") == 0) {
		if (strcmp(nomb_impl, "c") == 0) {
			proceso = smalltiles_c;
		} else {
			proceso = smalltiles_asm;
		}
	} else if (strcmp(filtro, "rotar") == 0) {
		if (strcmp(nomb_impl, "c") == 0) {
			proceso = rotar_c;
		} else {
			proceso = rotar_asm;
		}
	} else if (strcmp(filtro, "blit") == 0) {
		if (strcmp(nomb_impl, "c") == 0) {
			proceso_blit = blit_c;
		} else {
			proceso_blit = blit_asm;
		}
		caso = 1;
		
		// Cargo la imagen
		if( (blitcv = cvLoadImage ("data/blit.bmp", CV_LOAD_IMAGE_COLOR)) == 0 ) {
			printf("No se puede abrir la imagen data/blit.bmp\n");
			exit(EXIT_FAILURE);
		}
		blit = crearDesde(blitcv);
		DLOG("BLITCV at %p data=%p h=%d w=%d ws=%d\n", (void*)blitcv, blitcv->imageData, blitcv->height, blitcv->width, blitcv->widthStep);
		DLOG("BLIT at %p data=%p h=%d w=%d ws=%d\n", (void*)blit, blit->imageData, blit->height, blit->width, blit->widthStep);

	}

	
	// Cargo la imagen
	if( (srccv = cvLoadImage (nomb_arch_entrada, origen)) == 0 ) {
		printf("No se puede abrir la imagen fuente: %s\n", nomb_arch_entrada);
		exit(EXIT_FAILURE);
	}
	src = crearDesde(srccv);
	DLOG("SRCCV at %p data=%p h=%d w=%d ws=%d\n", (void*)srccv, srccv->imageData, srccv->height, srccv->width, srccv->widthStep);
	DLOG("SRC at %p data=%p h=%d w=%d ws=%d\n", (void*)src, src->imageData, src->height, src->width, src->widthStep);
	if (caso == 1) {
		if (srccv->height < blitcv->height || 
			srccv->width < blitcv->width) {
			cvReleaseImage(&blitcv);
			liberar(&blit);
			if( (blitcv = cvLoadImage ("data/blitsmall.bmp", CV_LOAD_IMAGE_COLOR)) == 0 ) {
				printf("No se puede abrir la imagen data/blitsmall.bmp\n");
				exit(EXIT_FAILURE);
			}
			if (srccv->height < blitcv->height || 
				srccv->width < blitcv->width) {
					printf("La imagen original es muy chica para blitear: alto %d - ancho %d (requiere %d x %d)\n", srccv->height, srccv->width, blitcv->height, blitcv->width);
					exit(EXIT_FAILURE);
			}
			
			blit = crearDesde(blitcv);
			DLOG("BLITCV at %p data=%p h=%d w=%d ws=%d\n", (void*)blitcv, blitcv->imageData, blitcv->height, blitcv->width, blitcv->widthStep);
			DLOG("BLIT at %p data=%p h=%d w=%d ws=%d\n", (void*)blit, blit->imageData, blit->height, blit->width, blit->widthStep);
		
		}
	}

	// Creo una IplImage para cada salida esperada
	if( (dstcv = cvCreateImage (cvGetSize (srccv), IPL_DEPTH_8U, destino) ) == 0 ) {
		printf("No se puede crear la imagen de destino\n");
		exit(EXIT_FAILURE);
	}
	
	for (int f = 0; f < dstcv->height - 1; f++) {
		memset((unsigned char*)dstcv->imageData + f * dstcv->widthStep, 0, dstcv->widthStep);
	}
	memset((unsigned char*)dstcv->imageData + (dstcv->height-1) * dstcv->widthStep, 0, dstcv->width * destino);
	
	
	dst = crearDesde(dstcv);
	DLOG("DSTCV (&%p) at %p data=%p h=%d w=%d ws=%d\n", (void*)&dstcv, (void*)dstcv, dstcv->imageData, dstcv->height, dstcv->width, dstcv->widthStep);
	DLOG("DST at %p data=%p h=%d w=%d ws=%d\n", (void*)dst, dst->imageData, dst->height, dst->width, dst->widthStep);
	
	unsigned long long int start, end;
	if (caso == 0) {

		if (tiempo) {
			

			MEDIR_TIEMPO_START(start);

			for(int i=0; i<cant_iteraciones; i++) {
				proceso((unsigned char*)src->imageData, (unsigned char*)dst->imageData, src->height, src->width, src->widthStep, dst->widthStep);
			}

			MEDIR_TIEMPO_STOP(end);

			imprimir_tiempos_ejecucion(start, end, cant_iteraciones);
		} else {
			MEDIR_TIEMPO_START(start);
			proceso((unsigned char*)src->imageData, (unsigned char*)dst->imageData, src->height, src->width, src->widthStep, dst->widthStep);
			MEDIR_TIEMPO_STOP(end);
		}
	} else {
		
		if (tiempo) {
			unsigned long long int start, end;

			MEDIR_TIEMPO_START(start);

			for(int i=0; i<cant_iteraciones; i++) {
				proceso_blit((unsigned char*)src->imageData, (unsigned char*)dst->imageData, src->height, src->width, src->widthStep, dst->widthStep, (unsigned char*)blit->imageData, blit->height, blit->width, blit->widthStep);
			}

			MEDIR_TIEMPO_STOP(end);

			imprimir_tiempos_ejecucion(start, end, cant_iteraciones);
		} else {
			MEDIR_TIEMPO_START(start);
			proceso_blit((unsigned char*)src->imageData, (unsigned char*)dst->imageData, src->height, src->width, src->widthStep, dst->widthStep, (unsigned char*)blit->imageData, blit->height, blit->width, blit->widthStep);
			MEDIR_TIEMPO_STOP(end);
		}
		
	}
	// Guardo imagen y libero las imagenes
	char nomb_arch_salida[256];

	memset(nomb_arch_salida, 0, 256);

	if (dir_salida != NULL) {
		sprintf(nomb_arch_salida, "%s/%s.%s.%s.bmp", dir_salida, strrchr(nomb_arch_entrada,'/'), filtro, nomb_impl);
	} else {
		sprintf(nomb_arch_salida, "%s.%s.%s.bmp", nomb_arch_entrada, filtro, nomb_impl);
	}

	guardar(dst);

	cvSaveImage(nomb_arch_salida, dstcv, NULL);
	
	DLOG ("Archivo guardado en %s\n", nomb_arch_salida);

	cvReleaseImage(&srccv);
	cvReleaseImage(&dstcv);
	if (caso == 1) {
		cvReleaseImage(&blitcv);
		liberar(&blit);
	}
	
	liberar(&src);
	liberar(&dst);
	
	
	return end-start;
}
