
typedef void (*pfiltro)(unsigned char *, unsigned char *, int, int, int, int);

typedef struct t_filtro {
    pfiltro fun;
    struct t_filtro* sig;
    struct t_filtro* ant;
    pfiltro asmf;
    pfiltro cf;
    char nombre[100];
} nfiltro;

typedef struct t_filtro_lista {
    nfiltro *prim;
    nfiltro *ult;
    nfiltro *actual;
}lista_filtro;

lista_filtro* crear(void);

void agregar(lista_filtro* lista, pfiltro asmf, pfiltro cf, char* nombre);

void remover(lista_filtro* lista);

void borrar(lista_filtro* lista);
