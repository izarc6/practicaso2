//////////////////Includes//////////////////
#include "ficheros.h"

//////////////////Defines//////////////////
#define MAX_CHAR 256
#define max_entradas (BLOCKSIZE / sizeof(struct entrada))
#define ERROR_EXTRAER_CAMINO -1
#define ERROR_NO_EXISTE_ENTRADA_CONSULTA -2
#define ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO -3
#define ERROR_PERMISO_ESCRITURA -4
#define ERROR_ENTRADA_YA_EXISTENTE -5
#define ERROR_NO_DIRECTORIO -6
#define ERROR_NO_FICHERO -7
#define ERROR_PERMISO_LECTURA -8

struct entrada {
  char nombre[60];  //En el SF ext2 la longitud del nombre es 255
  unsigned int ninodo;
};

struct UltimaEntrada {
  char camino [512];
  int p_inodo;
};

//////////////////Funciones//////////////////
// Nivel 8
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);
// Nivel 9
int mi_creat(const char *camino, unsigned char permisos);
int mi_dir (const char *camino, char *buffer);
int mi_chmod(const char *camino, unsigned char permisos);
int mi_stat (const char *camino, struct STAT *p_stat);
// Nivel 10
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes);
int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes);
// Nivel 11
int mi_link(const char *camino1, const char *camino2);
int mi_unlink(const char *camino);
