#include "ficheros_basico.h"

// Uso de mi_mkfs: ./mi_mkfs <nombre_dispositivo> <nbloques>
// argc=3            ^ argv[0]     ^ argv[1]         ^ argv[2]

int main(int argc, char **argv) {

    // Comprobaciòn sintaxis correcta
    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr,"Paràmetros no especificados. Uso: mi_mkfs <nombre_disp> <nbloques>\n");
        exit(1);
    }

    bmount(argv[1]);    // Abrimos el fichero de disco, si no existe se crea
    unsigned char buffer[BLOCKSIZE];
    memset(buffer,'\0',BLOCKSIZE);  // Bloque vacìo

    unsigned int nbloques = atoi(argv[2]);
    unsigned int ninodos = nbloques / 4;

    printf("Creaciòn bloques vacios, ahora escribo\n");

    // Escribimos nbloques vacíos para inicializar el FS
    for(int i = 0; i < nbloques; i++) {
        bwrite(i,buffer);
    }

    printf("Escritura bloques vacìos completada.\n");

    //Inicialización de los datos del superbloque
    initSB(nbloques, ninodos);
    printf("InitSB completado.\n");
    //Inicialización del mapa de bits (todos a 0)
    initMB();
    printf("InitMB completado.\n");
    //Creación de la lista enlazada de inodos
    initAI();
    printf("InitAI completado.\n");
    //Creaciòn del directorio raiz
    reservar_inodo('d',7);
    printf("Creaciòn directorio raìz completada.\n");

    bumount();  // Cerramos el fichero
    printf("FS desmontado.\n");

}
