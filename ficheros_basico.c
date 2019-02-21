#include "ficheros_basico.h"

int tamMB (unsigned int nbloques) {
  //calcular tamaño necesario mapa de bits (en bloques)
  int bloques = (nbloques / 8) / BLOCKSIZE;
  int resta = (nbloques % 8) % BLOCKSIZE;
  if (resta != 0) {
    bloques++;
  }
  return bloques;
}

int tamAI (unsigned int inodos) {
  //tamaño array de inodos
  int ninodos = (nbloques * INODOSIZE) / BLOCKSIZE;
  int resta = (nbloques * INODOSIZE) % BLOCKSIZE;
  if (resta != 0) {
    ninodos++;
  }
  return ninodos;
}

int initSB (unsigned int bloques, unsigned int inodos) {
  struct superbloque SB;
  SB.posPrimerBloqueMB =posSB + tamSB;
  SB.posUltimoBloqueMB= SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
  SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
  SB.posUltimoBloqueAI = SB.posPrimerBloqueAI+tamAI(ninodos) - 1;
  SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
  SB.posUltimoBloqueDatos = nbloques - 1;
  SB.posInodoRaiz = 0;
  SB.posPrimerInodoLibre = 0;       //tocar nivel3
  SB.cantBloquesLibres = nbloques;  //tocar nivel3
  SB.cantInodosLibres = ninodos;    //tocar nivel3

  bwrite(posSB, SB);

}

int initMB() {
  //poner a 0 todos los bits del mapa de bits
  unsigned char buffer[BLOCKSIZE];
  memset(buffer,'\0',BLOCKSIZE);

  // Leemos superbloque para obtener las posiciones de los datos
  struct superbloque SB = bread(posSB, SB);

  for(int i = SB.posPrimerBloqueDatos; i < tamMB(nbloques); i++) {
      bwrite(i,buffer);
  }

  return 0;

}

int initAI() {
  struct inodo inodos[BLOCKSIZE];
  struct superbloque SB = (struct superbloque) bread(posSB, SB);
  int contInodos = SB.posPrimerInodoLibre + 1;
  for (size_t i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++) {
    for (size_t j = 0; j < BLOCKSIZE / INODOSIZE; j++) {
      inodos[j].tipo = 'l';
      if (contInodos < SB.totInodos) {
        inodos[j].punterosDirectos[0] = contInodos;
        contInodos++;
      } else {
        inodos[j].punterosDirectos[0] = UINT_MAX;
      }
    }
    bwrite(i,buffer);
  }
  return 0;
}
