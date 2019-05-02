//#include "directorios.h"
#include <string.h> // DEBUG
#include <stdio.h>  // DEBUG
#include <stdlib.h> // DEBUG

// DEBUG
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);

// SOLO PARA PRUEBAS
int main(int argc, char **argv) {
  extraer_camino("/uno/dos/tres/cuatro",NULL,NULL,NULL);
}

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
  char cam[40];
  char **resultado = malloc(30*sizeof(cam));
  strcpy(cam,camino);
  char *test = strtok(cam,"/");
  while (test != NULL) {
    printf("%s\n",test);
    *resultado = test;
    resultado++;
    test = strtok(NULL,"/");
  }
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
