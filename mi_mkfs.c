#include "bloques.h"

// Uso de mi_mkfs: ./mi_mkfs <nombre_dispositivo> <nbloques>
// argc=3            ^ argv[0]     ^ argv[1]         ^ argv[2]

int main(int argc, char **argv) {

    // Comprobaciòn sintaxis correcta
    if (strcmp(argv[1],"") == 0 || strcmp(argv[2],"") == 0) {
        fprintf(stderr,"Paràmetros no especificados. Uso: mi_mkfs <nombre_disp> <nbloques>\n");
        exit(1);
    }

    bmount(argv[1]);    // Abrimos el fichero de disco, si no existe se crea
    unsigned char buffer[BLOCKSIZE];
    memset(buffer,'\0',BLOCKSIZE);  // Bloque vacìo

    // Escribimos nbloques vacìos para inicializar el FS
    for(int i = 0; i < atoi(argv[2]); i++)
    {
        bwrite(i,buffer);
    }

    bumount();  // Cerramos el fichero

}