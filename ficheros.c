#include "ficheros_basico.h"

//////////////////NIVEL 6//////////////////
// Escribe el contenido de un buffer de memoria en un fichero/directorio
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
  unsigned int primerBLogico, ultimoBLogico, desp1, desp2, bfisico;
  unsigned char buf_bloque[BLOCKSIZE];
  struct inodo inodo;
  if (leer_inodo(ninodo, &inodo) < 0) {
    fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
  if ((inodo.permisos & 2) == 2) {
    // Primer bloque lógico donde hay que escribir
    primerBLogico = offset/BLOCKSIZE;
    // Último bloque lógico donde hay que escribir
    ultimoBLogico = (offset+nbytes-1)/BLOCKSIZE;
    // Primeramente trataremos el caso en que el primer y último bloque coincidan
    if (primerBLogico == ultimoBLogico) {
      desp1 = offset % BLOCKSIZE;
      bfisico = traducir_bloque_inodo(ninodo, primerBLogico, 1);
      if (bfisico == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      if (bread(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      memcpy(buf_bloque+desp1, buf_original, nbytes);
      if (bwrite(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      // Trataremos el caso en que la operación afecte a más de un bloque
    } else {
      // Distingamos tres fases:
      // Primer bloque
      bfisico = traducir_bloque_inodo(ninodo, primerBLogico, 1);
      if (bfisico == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      if (bread(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      desp1 = offset % BLOCKSIZE;
      memcpy(buf_bloque+desp1, buf_original, BLOCKSIZE - desp1);
      if (bwrite(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      // Bloques intermedios
      for (int i = primerBLogico + 1; i < ultimoBLogico; i++) {
        bfisico = traducir_bloque_inodo(ninodo, i, 1);
        if(bwrite(bfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE) == -1) {
          fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
          return -1;
        }
      }
      // Último bloque
      bfisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
      if (bfisico == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      if (bread(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      desp2 = (offset + nbytes - 1) % BLOCKSIZE;
      memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
      if (bwrite(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
    }
    if ((offset + nbytes) > inodo.tamEnBytesLog) {
      inodo.tamEnBytesLog = (offset + nbytes);
      inodo.ctime = time(NULL);
    }
    inodo.mtime = time(NULL);
    if (escribir_inodo(inodo, ninodo) == -1) {
      fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
      return -1;
    }
  } else {
    fprintf(stderr, "Error en ficheros.c mi_write_f() --> No tienes permisos para escribir --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
  return nbytes;
}

// Lee información de un fichero/directorio y la almacena en un buffer de memoria
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
  unsigned int primerBLogico, ultimoBLogico, desp1, desp2, bfisico;
  unsigned char buf_bloque[BLOCKSIZE];
  struct inodo inodo;
  int leidos = 0;
  if (leer_inodo(ninodo, &inodo) < 0) {
    fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
  if ((inodo.permisos & 4) == 4) {
    if (offset >= inodo.tamEnBytesLog) {
      // No podemos leer nada
      return leidos;
    }
    if (offset + nbytes >= inodo.tamEnBytesLog) {
      // leemos sólo los bytes desde el offset hasta EOF
      nbytes = inodo.tamEnBytesLog - offset;
    }
    // Primer bloque lógico donde hay que escribir
    primerBLogico = offset/BLOCKSIZE;
    // Último bloque lógico donde hay que escribir
    ultimoBLogico = (offset+nbytes-1)/BLOCKSIZE;
    // Primeramente trataremos el caso en que el primer y último bloque coincidan
    if (primerBLogico == ultimoBLogico) {
      desp1 = offset % BLOCKSIZE;
      bfisico = traducir_bloque_inodo(ninodo, primerBLogico, 0);
      if (bfisico == -1) {
        fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      if (bread(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      memcpy(buf_original, buf_bloque + desp1, nbytes);
      leidos = leidos + nbytes;
    } else {
      // Distingamos tres fases:
      // Primer bloque
      bfisico = traducir_bloque_inodo(ninodo, primerBLogico, 0);
      if (bfisico == -1) {
        fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      if (bread(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      desp1 = offset % BLOCKSIZE;
      memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
      leidos = leidos + BLOCKSIZE - desp1;
      // Bloques intermedios
      for (int i = primerBLogico + 1; i < ultimoBLogico; i++){
        bfisico = traducir_bloque_inodo(ninodo, i, 0);
        if (bfisico == -1) {
          fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
          return -1;
        }
        if (bread(bfisico, buf_bloque) == -1) {
          fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
          return -1;
        }
        memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE, buf_bloque , BLOCKSIZE);
        leidos = leidos + BLOCKSIZE;
      }
      // Último bloque
      bfisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 0);
      if (bfisico == -1) {
        fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      if (bread(bfisico, buf_bloque) == -1) {
        fprintf(stderr, "Error en ficheros.c mi_read_f() --> %d: %s\n", errno, strerror(errno));
        return -1;
      }
      desp2 = (offset + nbytes - 1) % BLOCKSIZE;
      memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
      leidos = leidos + desp2 + 1;
    }
    inodo.atime = time(NULL);
    if (escribir_inodo(inodo, ninodo) == -1) {
      fprintf(stderr, "Error en ficheros.c mi_write_f() --> %d: %s\n", errno, strerror(errno));
      return -1;
    }
  } else {
    fprintf(stderr, "Error en ficheros.c mi_read_f() --> No tienes permisos para leer --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
  return leidos;
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