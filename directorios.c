#include "directorios.h"

//////////////////NIVEL 8//////////////////
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {

}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){

}

//////////////////NIVEL 9//////////////////
int mi_creat(const char *camino, unsigned char permisos){

}

int mi_dir (const char *camino, char *buffer){

}

int mi_chmod(const char *camino, unsigned char permisos){

}

int mi_stat (const char *camino, struct STAT *p_stat){

}

//////////////////NIVEL 10/////////////////
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){

}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){

}
