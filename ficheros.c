#include "ficheros_basico.h"

//////////////////NIVEL 6//////////////////
// Escribe el contenido de un buffer de memoria en un fichero/directorio
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
  return 0;
}

// Lee información de un fichero/directorio y la almacena en un buffer de memoria
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
  return 0;
}

// Devuelve la metainformación de un fichero/directorio
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat) {
  struct inodo inodo;
  if (leer_inodo(ninodo, &inodo) < 0) {
    fprintf(stderr, "Error en ficheros.c mi_stat_f() --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
  p_stat->tipo = inodo.tipo;
  p_stat->permisos = inodo.permisos;
  p_stat->nlinks = inodo.nlinks;
  p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
  p_stat->atime = inodo.atime;
  p_stat->mtime = inodo.mtime;
  p_stat->ctime = inodo.ctime;
  p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
  return 0;
}

// Cambia los permisos de los inodos
int mi_chmod_f(unsigned int ninodo, unsigned char permisos) {
  struct inodo inodo;
  leer_inodo(ninodo, &inodo);
  // Cambiamos permisos
  inodo.permisos = permisos;
  // Actualizamos ctime
  inodo.ctime  = time(NULL);
  escribir_inodo(inodo, ninodo);
  return 0;
}

// Trunca un inodo en nbytes indicados como parámetro
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes) {
  struct inodo inodo;
  unsigned int nblogico;
  leer_inodo(ninodo, &inodo);
  // Miramos si tenemos permisos
  if ((inodo.permisos & 2) != 2) {
    fprintf(stderr, "Error en ficheros.c mi_truncar_f() --> %d: %s\n", errno, strerror(errno));
    return -1;
  } else {
    if (inodo.tamEnBytesLog <= nbytes) {
      fprintf(stderr, "Error en ficheros.c mi_truncar_f() --> %d: %s\n", errno, strerror(errno));
      return -1;
    }
    // Calculamos nblogico
    if (nbytes % BLOCKSIZE == 0) {
      nblogico = nbytes / BLOCKSIZE;
    } else {
      nblogico = nbytes / BLOCKSIZE + 1;
    }
    int bLiberados = liberar_bloques_inodo(ninodo, nblogico);
    inodo.numBloquesOcupados = inodo.numBloquesOcupados - bLiberados;
    // Actualizamos mtime, ctime y tamEnBytesLog
    inodo.mtime = time(NULL);
    inodo.ctime  = time(NULL);
    inodo.tamEnBytesLog = nbytes;
    escribir_inodo(inodo, ninodo);
    return bLiberados;
  }
  fprintf(stderr, "Error en ficheros.c mi_truncar_f() --> %d: %s\n", errno, strerror(errno));
  return -1;
}
