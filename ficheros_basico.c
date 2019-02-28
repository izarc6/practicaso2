#include "ficheros_basico.h"

//////////////////NIVEL 2//////////////////

struct superbloque SB;

//Calcular tamaño necesario mapa de bits (en bloques)
int tamMB (unsigned int nbloques) {
  int size = (nbloques / 8) / BLOCKSIZE;
  int resta = (nbloques / 8) % BLOCKSIZE;
  if (resta != 0) {
    size++;
  }
  return size;
}

//Calcular tamaño array de inodos
int tamAI (unsigned int ninodos) {
  // int ninodos = nbloques/4; --> El programa mi_mkfs.c le pasará este dato a esta función como parámetro al llamarla
  int size = (ninodos * INODOSIZE) / BLOCKSIZE;
  int resta = (ninodos * INODOSIZE) % BLOCKSIZE;
  if (resta != 0) {
    size++;
  }
  return size;
}

//Inicialización de los datos del superbloque
int initSB (unsigned int nbloques, unsigned int ninodos) {
  SB.posPrimerBloqueMB = posSB + tamSB;
  SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
  SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
  SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
  SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
  SB.posUltimoBloqueDatos = nbloques - 1;
  SB.posInodoRaiz = 0;
  SB.posPrimerInodoLibre = 0;       //tocar nivel3
  SB.cantBloquesLibres = nbloques;  //tocar nivel3
  SB.cantInodosLibres = ninodos;    //tocar nivel3
  for(size_t i = 0; i < INODOSIZE - 2 * sizeof(unsigned char) - 3 * sizeof(time_t) - 18 * sizeof(unsigned int) - 6 * sizeof(unsigned char); i++){
    SB.padding[i] = 0;
  }
  return bwrite(posSB, &SB);
}

//Inicialización del mapa de bits (todos a 0)
int initMB() {
  unsigned char buffer[BLOCKSIZE];
  memset(buffer,'\0',BLOCKSIZE);
  // Leemos superbloque para obtener las posiciones de los datos
  if (bread(0, &SB) == -1) {
      fprintf(stderr, "Error en ficheros_basico.c initMB() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }
  for(size_t i = SB.posPrimerBloqueDatos; i <= SB.posUltimoBloqueDatos; i++) {
    if (bwrite(i, buffer) == -1) {
      fprintf(stderr, "Error en ficheros_basico.c initMB() --> %d: %s\n", errno, strerror(errno));
      return -1;
    }
  }
  // Actualizamos el superbloque
  if (bwrite(0, &SB) < 0) {
      fprintf(stderr, "Error en ficheros_basico.c initMB() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }
  return 0;
}

//Creación de la lista enlazada de inodos
int initAI() {
  struct inodo inodos [BLOCKSIZE/INODOSIZE];
  // Leemos superbloque para obtener las posiciones de los datos
  if (bread(0, &SB) == -1) {
      fprintf(stderr, "Error en ficheros_basico.c initAI() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }
  unsigned int contInodos = SB.posPrimerInodoLibre + 1;
  for (size_t i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++) {
    for (size_t j = 0; j < BLOCKSIZE / INODOSIZE; j++) {
      inodos[j].tipo = 'l';  //libre
      if (contInodos < SB.totInodos) {
        inodos[j].punterosDirectos[0] = contInodos;
        contInodos++;
      } else {
        inodos[j].punterosDirectos[0] = UINT_MAX;
      }
    }
    // Escribimos el bloque de inodos
    if (bwrite(i, inodos) < 0) {
        fprintf(stderr, "Error en ficheros_basico.c initAI() --> %d: %s\n", errno, strerror(errno));
        return -1;
    }
  }
  if (bwrite(0, &SB) == 0) {
      fprintf(stderr, "Error en ficheros_basico.c initAI() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }
  return 0;
}

//////////////////NIVEL 3//////////////////
int escribir_bit(unsigned int nbloque, unsigned int bit) {
  unsigned char mascara = 128; // 10000000
  if (bread(0, &SB) == -1) {
      fprintf(stderr, "Error en ficheros_basico.c escribir_bit() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }

  // Calculamos la posición del byte en el MB, posbyte, y la posición del bit dentro de ese byte, posbit
  unsigned int posbyte = nbloque / 8;
  unsigned int posbit = nbloque % 8;

  // Hemos de determinar luego en qué bloque del MB, nbloqueMB, se halla ese bit para leerlo
  unsigned int nbloqueMB = posbyte/BLOCKSIZE;

  // Y finalmente hemos de obtener en qué posición absoluta del dispositivo virtual
  // se encuentra ese bloque, nbloqueabs, donde leer/escribir el bit
  unsigned int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;

  posbyte = posbyte % BLOCKSIZE;
  unsigned char bufferMB[posbyte];
  memset(bufferMB,'\0',posbyte);
  mascara >>= posbit; // desplazamiento de bits a la derecha
  if (bit == 1) {
    bufferMB[posbyte] |= mascara; // operador OR para bits
  } else {
    bufferMB[posbyte] &= ~mascara; // operadores AND y NOT para bits
  }
  return bwrite(nbloqueabs, bufferMB);
}

unsigned char leer_bit(unsigned int nbloque) {
unsigned char mascara = 128; // 10000000
  if (bread(0, &SB) == -1) {
      fprintf(stderr, "Error en ficheros_basico.c leer_bit() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }

  // Calculamos la posición del byte en el MB, posbyte, y la posición del bit dentro de ese byte, posbit
  unsigned int posbyte = nbloque / 8;
  unsigned int posbit = nbloque % 8;

  // Hemos de determinar luego en qué bloque del MB, nbloqueMB, se halla ese bit para leerlo
  unsigned int nbloqueMB = posbyte/BLOCKSIZE;

  // Y finalmente hemos de obtener en qué posición absoluta del dispositivo virtual
  // se encuentra ese bloque, nbloqueabs, donde leer/escribir el bit
  unsigned int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;

  posbyte = posbyte % BLOCKSIZE;
  unsigned char bufferMB[posbyte];
  memset(bufferMB,'\0',posbyte);
  mascara >>= posbit; // Desplazamiento de bits a la derecha
  mascara &= bufferMB[posbyte]; // AND para bits
  mascara >>= (7-posbit); // Desplazamiento de bits a la derecha, ahora el bit leido està
                          // al final de la mascara

  return mascara;  // Devolvemos el bit leido
}

int reservar_bloque() {
  return 0;
}

int liberar_bloque(unsigned int nbloque) {
  // Primero escribimos un 0 en el bloque pasado como parámetro
  if (escribir_bit(nbloque, 0) == -1) {
      fprintf(stderr, "Error en ficheros_basico.c liberar_bloque() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }
  // Después leemos el Super Bloque
  if (bread(0, &SB) == -1) {
      fprintf(stderr, "Error en ficheros_basico.c liberar_bloque() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }
  // Sumamos una unidad a la cantidad de bloques libres
  SB.cantBloquesLibres++;
  // Guardamos el SB
  if (bwrite(0, &SB) == 0) {
      fprintf(stderr, "Error en ficheros_basico.c liberar_bloque() --> %d: %s\n", errno, strerror(errno));
      return -1;
  }
  // Devolvemos el nº de bloque liberado
  return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo inodo) {
  //Escribe el contenido de una variable del tipo struct inodo en un determinado inodo del array de inodos
  struct superbloque SB = bread(0,&SB);
  if (SB == 0) {
    fprintf(stderr, "Error en ficheros_basico.c escribir_inodo() --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
  unsigned int posInodo = SB.posPrimerBloqueAI + (ninodo * INODOSIZE) / BLOCKSIZE;
  struct inodo inodos[BLOCKSIZE/INODOSIZE];
  bread(posInodo, &inodos); // Leemos el bloque del inodo
  return bwrite(inodos[ninodo %(BLOCKSIZE/INODOSIZE)],&inodo);
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo) {
  struct superbloque SB = bread(0,&SB);
  if (SB == 0) {
    fprintf(stderr, "Error en ficheros_basico.c leer_inodo() --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
  unsigned int posInodo = SB.posPrimerBloqueAI + (ninodo * INODOSIZE) / BLOCKSIZE;
  struct inodo inodos[BLOCKSIZE/INODOSIZE];
  return bread(inodos[ninodo %(BLOCKSIZE/INODOSIZE)],&inodo);
}

int reservar_inodo(unsigned char tipo, unsigned char permisos) {
  //encuentra el primer idondo libre (dato alamcenado en el SB), lo reserva (con
  //la ayuda de la funcuion escribir_inodo(), devuelve su numero y actualiza la
  //lista de inodos libres)
  if (bread(0,&SB) == 0) {
    fprintf(stderr, "Error en ficheros_basico.c reservar_inodo() --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
  // Si no hay inodos libres, error
  if (SB.cantInodosLibres == 0) {
    fprintf(stderr, "Error en ficheros_basico.c reservar_inodo() --> %d: %s\nNingún inodo libre!", errno, strerror(errno));
    return -1;
  }

  int posInodoReservado = SB.posPrimerInodoLibre;

  // TODO: Verificar si esto está bien!!!
  Inodo inodo = leer_inodo(posInodoReservado);
  inodo.tipo = tipo;
  inodo.permisos = permisos;
  inodo.nlinks = 1;
  inodo.tamEnBytesLog = 0;
  inodo.atime = time(NULL);
  inodo.ctime = time(NULL);
  inodo.mtime = time(NULL);
  inodo.numBloquesOcupados = 0;
  memset(inodo.punterosDirectos, 0, sizeof(inodo.punterosDirectos));
  memset(inodo.punterosIndirectos, 0, sizeof(inodo.punterosIndirectos));
  escribir_inodo(posInodoReservado, inodo);

  // Actualización superbloque
  SB.cantInodosLibres--;
  SB.posPrimerInodoLibre++;
  if (bwrite(posSB, &SB) == 0) {
    fprintf(stderr, "Error en ficheros_basico.c reservar_inodo() --> %d: %s\nNo se ha podido actualizar el SB!", errno, strerror(errno));
    return -1;
  }

  return posInodoReservado;
}

//////////////////NIVEL 4//////////////////

int obtener_nrangoBL (struct inodo *inodo, unsigned int nblogico, unsigned int *ptr) {
  if (nblogico < DIRECTOS) {
    *ptr = inodo->punterosDirectos[nblogico];
    return 0;
  } else if (nblogico < INDIRECTOS0) {
    *ptr = inodo->punterosIndirectos[0];
    return 1;
  } else if (nblogico < INDIRECTOS1) {
    *ptr = inodo->punterosIndirectos[1];
    return 2;
  } else if (nblogico < INDIRECTOS2) {
    *ptr = inodo->punterosIndirectos[2];
    return 3;
  } else {
    *ptr = 0;
    fprintf(stderr, "Bloque lógico fuera de rango --> %d: %s\n", errno, strerror(errno));
    return -1;
  }
}

int obtener_indice (unsigned int nblogico, unsigned int nivel_punteros) {
  if (nblogico < DIRECTOS) {
    return nblogico;
  } else if (nblogico < INDIRECTOS0) {
    return nblogico - DIRECTOS;
  } else if (nblogico < INDIRECTOS1) {
    if (nivel_punteros == 2) {
      return (nblogico - INDIRECTOS0) / NPUNTEROS;
    } else if (nivel_punteros == 1) {
      return (nblogico - INDIRECTOS0) % NPUNTEROS;
    }
  } else if (nblogico < INDIRECTOS2) {
    if (nivel_punteros == 3) {
      return (nblogico - INDIRECTOS1)/(NPUNTEROS*NPUNTEROS);
    } else if (nivel_punteros == 2) {
      return ((nblogico - INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))/NPUNTEROS;
    } else if (nivel_punteros == 1) {
      return ((nblogico - INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))%NPUNTEROS;
    }
  }
  fprintf(stderr, "Error en ficheros_basico.c obtener_indice() --> %d: %s\n", errno, strerror(errno));
  return -1;
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar) {
  // TODO
  return 0;
}
