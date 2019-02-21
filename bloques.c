#include "bloques.h"

static int descriptor = 0;  // Descriptor del fichero del file system


// bmount: Abre el file system para la lectura/escritura
int bmount(const char *camino) {
    int fd = open(camino, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
            fprintf(stderr, "ERROR fallo en abertura. \n");
        }
    descriptor = fd;
    return fd;
}

// bumount: "Desmonta" el file system que hemos abierto para la lectura/escritura
int bumount() {
    int fc = close(descriptor);
    if (fc == -1) {
        //Error
        fprintf(stderr, "ERROR: fallo al cerrar. \n");
        return -1;
    }
    else {
        return 0;
    }
}

// escriure un bloc, el indicat per nbloque, s'sescriu *buf
// bwrite retorna el nº de bytes que s'ha escrit o -1 si error
int bwrite(unsigned int nbloque, const void *buf) {

    // Con 'lseek' nos desplazamos en el gestor de ficheros para coger
    // el bloque que nos interesa
    off_t desplazamiento = lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);

    if (desplazamiento == -1) {
        //Error
        fprintf(stderr, "ERROR: fallo al buscar bloque. \n");
        return -1;
    }
    else {
        int bescritos = write(descriptor, buf, BLOCKSIZE);  // N. de bytes escritos
        if (bescritos == -1) {
            fprintf(stderr, "ERROR: fallo al escribir el bloque. \n");
        }
        return bescritos;
    }
}

// bread: lee un bloque indicado por nbloque, devuelve el
//        numero de bytes leidos
int bread(unsigned int nbloque, void *buf) {

    off_t desplazamiento = lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);
    if (desplazamiento == -1) {
        //Error
        fprintf(stderr, "ERROR: fallo al buscar bloque. \n");
        return -1;
    }
    else {
        int bleidos = read(descriptor, buf, BLOCKSIZE); // N. de bytes leidos
        if (bleidos == -1) {
            fprintf(stderr, "ERROR: fallo al leer el bloque. \n");
        }
        return bleidos;
    }
}
