#include "listafiltro.h"
#include <stdlib.h>
#include<string.h>

lista_filtro* crear(void) {
    lista_filtro* tmp = malloc(sizeof(lista_filtro));
    tmp->pri = NULL;
    tmp->ult = NULL;
    tmp->actual = NULL;
    return tmp;
}

void agregar(lista_filtro* lista, pfiltro asmf, pfiltro cf, char* nombre){
    nfiltro* fil = malloc(sizeof(nfiltro));
    fil->asmf = asmf;
    fil->cf = cf;
    fil->sig = NULL;
    strncpy(fil->nombre,nombre,100);
    fil->nombre[99] = '\0';
    
    if (lista->actual != NULL) {
        fil->sig = lista->actual->sig;
        lista->actual->sig = fil;
        fil->ant = lista->actual;
        if (fil->sig != NULL) {
            fil->sig->ant = fil;
        } else {
            lista->ult = fil;
        }
    } else {
        lista->pri = fil;
        lista->ult = fil;
    }
    lista->actual = fil;
}

void remover(lista_filtro* lista){
    nfiltro *tmp = lista->actual;
    if (tmp == NULL){
        return;
    }
    if (tmp->ant) {
        lista->actual = tmp->ant;
    } else {
        lista->actual = tmp->sig;
    }
    nfiltro *ant = tmp->ant;
    nfiltro *sig = tmp->sig;
    if (tmp == lista->pri) {
        lista->pri = sig;
    }
    if (tmp == lista->ult) {
        lista->ult = ant;
    }
    if (ant != NULL) {
        ant->sig = sig;
    }
    if (sig != NULL) {
        sig->ant = ant;
    }
    free(tmp);
}

void avanzar(lista_filtro *lista) {
    if (lista->actual && lista->actual->sig) {
        lista->actual = lista->actual->sig;
    }
}

void retroceder(lista_filtro *lista) {
    if (lista->actual && lista->actual->ant) {
        lista->actual = lista->actual->ant;
    }
}

