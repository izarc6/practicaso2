#include "ficheros_basico.h"
#include "bloques.h"

// Uso de leer_sf: ./leer_sf <nombre_dispositivo>
// argc=2            ^ argv[0]     ^ argv[1]

int main(int argc, char **argv) {

    // Comprobaciòn sintaxis correcta
    if (strcmp(argv[1],"") == 0) {
        fprintf(stderr,"Paràmetros no especificados. Uso: leer_sf <nombre_dispositivo>\n");
        exit(1);
    }

    bmount(argv[1]);    // Abrimos el fichero de disco, si no existe se crea

    struct superbloque SB;

    if (bread(0,&SB) == -1) {
        fprintf(stderr, "Error en leer_sf.c --> %d: %s\nImposible leer el superbloque!", errno, strerror(errno));
        return -1;
    }

    printf("Datos del Superbloque:\n");
    printf("\tposPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    printf("\tposUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);
    printf("\tposPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
    printf("\tposUltimoBloqueAI = %d\n", SB.posPrimerBloqueAI);
    printf("\tposPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf("\tposUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf("\tposInodoRaiz = %d\n", SB.posInodoRaiz);
    printf("\tposPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf("\tcantBloquesLibres = %d\n", SB.cantBloquesLibres);
    printf("\tcantInodosLibres = %d\n", SB.cantInodosLibres);
    printf("\ttotBloques = %d\n", SB.totBloques);
    printf("\ttotInodos = %d\n", SB.totInodos);

    printf("sizeof struct superbloque: %lu\n", sizeof(struct superbloque));
    printf("sizeof struct inodo: %lu\n", sizeof(struct inodo));

    printf("\nRecorrido lista enlazada de inodos libres:\n");

    bumount();
    return 0;
}
