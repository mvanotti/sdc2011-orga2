#include "listafiltro.h"
#include <stdlib.h>
#include<string.h>

typedef void (*pfiltro)(unsigned char *, unsigned char *, int, int, int, int);

typedef struct t_filtro {
    pfiltro fun;
    struct t_filtro* sig;
    struct t_filtro* ant;
    char[256] nombre;
}nfiltro;

typedef struct t_filtro_lista{
    nfiltro* prim;
    nfiltro* ult;
}lista_filtro;

lista_filtro* crear(void){
    lista_filtro* tmp = malloc(sizeof(lista_filtro));
    tmp->prim = NULL;
    tmp->ult = NULL;
    return tmp;
}

void agregar(lista_filtro* lista, pfiltro filtro, char* nombre){
    nfiltro* fil = malloc(sizeof(nfiltro));
    fil->fun = filtro;
    fil->sig = NULL;
    strncpy(fil->nombre,nombre,256);
    fil->ant = NULL;
    
    lista->ult = fil;
    if(lista->prim == NULL){
        lista->prim = fil;
    } else{
        nfiltro* tmp = lista->prim;
        while(tmp->sig != NULL){
            tmp = tmp->sig;
        }
        tmp->sig = fil;
        fil->ant = tmp;
    }
}

void remover(lista_filtro* lista){
    nfiltro *tmp = lista->ult;
    if(tmp == NULL){
        return;
    }
    if(tmp->ant == NULL){
        lista->prim = NULL;
        lista->ult = NULL;
        free(tmp);
        return;
    }
    lista->ult = tmp->ant;
    free(tmp);
    lista->ult->sig = NULL;

}

void borrar(lista_filtro* lista);

