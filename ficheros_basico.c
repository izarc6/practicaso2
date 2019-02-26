#include "ficheros_basico.h"

//////////////////NIVEL 2//////////////////
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
  struct superbloque SB;
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
  struct superbloque SB; // ERROR -- No se puede leer con argumento (...,SB) da fallo // ERROR -- No se puede leer con argumento (...,SB) da fallo
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
  unsigned char buffer[BLOCKSIZE];
  memset(buffer,'\0',BLOCKSIZE);
  struct inodo inodos [BLOCKSIZE/INODOSIZE];
  struct superbloque SB;
  int contInodos = SB.posPrimerInodoLibre + 1;
  for (size_t i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++) {
    for (size_t j = 0; j < BLOCKSIZE / INODOSIZE; j++) {
      buffer[inodos[j].tipo] = 'l';  //libre
      if (contInodos < SB.totInodos) {
        buffer[inodos[j].punterosDirectos[0]] = contInodos;
        contInodos++;
      } else {
        buffer[inodos[j].punterosDirectos[0]] = UINT_MAX;
      }
    }
    bwrite(i, buffer);
  }
  return 0;
}

//////////////////NIVEL 3//////////////////
int escribir_bit(unsigned int nbloque, unsigned int bit) {
  unsigned char mascara = 128; // 10000000
  struct superbloque SB;

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
  return NULL;
}

int reservar_bloque() {
  return 0;
}

int liberar_bloque(unsigned int nbloque) {
  return 0;
}

int escribir_inodo(unsigned int ninodo, struct inodo inodo) {
  return 0;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo) {
  return 0;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos) {
  return 0;
}
