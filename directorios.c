//#include "directorios.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);


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

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir,
  unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {

}
