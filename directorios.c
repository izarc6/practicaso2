//#include "directorios.h"
#include <string.h> // DEBUG
#include <stdio.h>  // DEBUG
#include <stdlib.h> // DEBUG
#include "directorios.h"

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
  unsigned int p_inodo_dir, p_inodo, p_entrada;
  p_inodo_dir = 0; //Por simplicidad podemos suponer directamente que p_inodo_dir es 0
  int errores = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos);
  if(errores < 0){
    switch(errores){
      case -1:
      fprintf(stderr, "Error en directorios.c mi_creat() --> Tipo no adecuado\n");
      break;
      case -2:
      fprintf(stderr, "Error en directorios.c mi_creat() --> No se puede leer el inodo\n");
      break;
      case -3:
      fprintf(stderr, "Error en directorios.c mi_creat() --> El directorio donde apunta p_inodo_dir no tiene permisos de escritura\n");
      break;
      case -5:
      fprintf(stderr, "Error en directorios.c mi_creat() --> Ya existe\n");
      break;
    }
    return -1;
  }
  return 0;
}

int mi_dir(const char *camino, char *buffer){
    unsigned int p_inodo_dir = 0;
    struct inodo inodo;
    int errores = buscar_entrada(camino, &p_inodo_dir, NULL, NULL, 0, 0);
    if(errores < 0){
      switch(errores){
        case -1:
        fprintf(stderr, "Error en directorios.c mi_dir() --> Tipo no adecuado\n");
        break;
        case -2:
        fprintf(stderr, "Error en directorios.c mi_dir() --> No se puede leer el inodo\n");
        break;
        case -3:
        fprintf(stderr, "Error en directorios.c mi_dir() --> El directorio donde apunta p_inodo_dir no tiene permisos de escritura\n");
        break;
        case -5:
        fprintf(stderr, "Error en directorios.c mi_dir() --> Ya existe\n");
        break;
      }
      return -1;
    }
    if (leer_inodo(ninodo, &inodo) == -1) {
        fprintf(stderr, "Error en directorios.c mi_dir() --> No se ha podido leer el inodo %d\n", ninodo);
        return -1;
    }
    // Comprobamos errores
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, "El fichero '%s' no tiene permisos de lectura.\n", camino);
        return -1;
    }
    if (inodo.tipo != 'd') {
        fprintf(stderr, "%s no es un directorio!\n", camino);
        return -1;
    }
    // Creamos la cadena de salida
    struct inodo inodoEntrada;
    struct tm *tm;
    unsigned int
        numEntradas = inodo.tamEnBytesLog / sizeof(struct entrada),
        tamEntrada = 100;
    char
        *separador = " | ",
        strPermisos[5], strTamEnBytesLog[10], strTime[50], strTipo[2], strNinodo[10],
        strEntrada[tamEntrada * numEntradas];
    struct entrada bufferDir[numEntradas];
    *strEntrada = '\0';
    errores = mi_read_f(ninodo, bufferDir, 0, inodo.tamEnBytesLog);
    if (errores < 0) {
        fprintf(stderr, "Error en directorios.c mi_dir()  --> Error en línea 310\n");
        return errores;
    }
    for (int i = 0; i < numEntradas; ++i) {
        if (leer_inodo(bufferDirectorio[i].ninodo, &inodoEntrada) == -1) {
            fprintf(stderr, "Error en directorios.c mi_dir()  --> No se ha podido leer la entrada %d\n", bufferDirectorio[i].ninodo);
            return -1;
        }
        // toStrings
        *strPermisos = '\0';
        *strNinodo = '\0';
        strTipo[0] = inodoEntrada.tipo;
        strTipo[1] = '\0';
        strcat(strPermisos, strTipo);
        strcat(strPermisos, (inodoEntrada.permisos & 4) ? "r" : "-");
        strcat(strPermisos, (inodoEntrada.permisos & 2) ? "w" : "-");
        strcat(strPermisos, (inodoEntrada.permisos & 1) ? "x" : "-");
        sprintf(strNinodo, "%u", bufferDirectorio[i].ninodo);
        sprintf(strTamEnBytesLog, "%u", inodoEntrada.tamEnBytesLog);
        tm = localtime(&inodoEntrada.mtime);
        sprintf(strTime, "[%d-%02d-%02d %02d:%02d:%02d]", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        strcat(strEntrada, strNinodo);
        strcat(strEntrada, separador);
        strcat(strEntrada, strPermisos);
        strcat(strEntrada, separador);
        strcat(strEntrada, strTamEnBytesLog);
        strcat(strEntrada, separador);
        strcat(strEntrada, strTime);
        strcat(strEntrada, separador);
        strcat(strEntrada, bufferDirectorio[i].nombre);
        strcat(strEntrada, "\n");
    }
    strcpy(buffer, strEntrada);
    return 0;
}

int mi_chmod(const char *camino, unsigned char permisos){
    unsigned int ninodo;
    int errores;
    errores = buscar_entrada(camino, &ninodo, NULL, NULL, 0, 0);
    if (errores < 0) {
        fprintf(stderr, "Error en directorios.c mi_chmod() --> No se ha encontrado la entrada %s\n", camino);
        return errores;
    }
    errores = mi_chmod_f(ninodo, permisos);
    if (errores < 0) {
        fprintf(stderr, "Error en directorios.c mi_chmod() --> No se han podido actualizar los permisos del inodo %d\n", ninodo);
        return errores;
    }
    return 0;
}

int mi_stat(const char *camino, struct STAT *p_stat){
    unsigned int ninodo;
    int errores;
    errores = buscar_entrada(camino, &ninodo, NULL, NULL, 0, 0);
    if (errores < 0) {
        fprintf(stderr, "Error en directorios.c mi_stat() --> No se ha encontrado la entrada %s\n", camino);
        return errores;
    }
    errores = mi_stat_f(ninodo, p_stat);
    if (errores < 0) {
        fprintf(stderr, "Error en directorios.c mi_stat() --> No se ha podido obtener la información del inodo %d\n", ninodo);
        return errores;
    }
}

//////////////////NIVEL 10/////////////////
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){

}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){

}

//////////////////NIVEL 11/////////////////
int mi_link(const char *camino1, const char *camino2) {

}

int mi_unlink(const char *camino) {

}
