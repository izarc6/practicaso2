//////////////////Includes//////////////////
#include "ficheros.h"
#include "ficheros_basico.h"


//////////////////Defines//////////////////
struct entrada {
char nombre[60];  //En el SF ext2 la longitud del nombre es 255
unsigned int ninodo;
};

//////////////////Funciones//////////////////
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);
