
typedef void (*pfiltro)(unsigned char *, unsigned char *, int, int, int, int);

typedef struct t_filtro {
    pfiltro fun;
    struct t_filtro* sig;
    struct t_filtro* ant;
    char* nombre;
}filtro;

typedef struct t_filtro_lista{
    filtro* prim;
    filtro* ult;
}lista_filtro;

lista_filtro* crear(void);

void agregar(lista_filtro* lista, pfiltro filtro, char* nombre);

void remover(lista_filtro* lista);

void borrar(lista_filtro* lista);
