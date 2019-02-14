#include <stdio.h>

#define BLOCKSIZE = 1024    // Tamaño de bloque lògico

int bmount(const char *camino); // Mount volume
int bumount();                  // Unmount volume
int bwrite(unsigned int nbloque, const void *buf);  // Write block
int bread(unsigned int nbloque, void *buf)  // Read block

// NOTA: Cada llamada a sistema tiene que tener gestion/control de errores a seguir

