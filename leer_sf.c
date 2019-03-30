#include "ficheros_basico.h"
#include "bloques.h"

// Uso de leer_sf: ./leer_sf <nombre_dispositivo>
// argc=2            ^ argv[0]     ^ argv[1]

int main(int argc, char **argv) {

    // Comprobaciòn sintaxis correcta
    if (argv[1] == NULL) {
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
    printf("\tposUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
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
    struct inodo in[BLOCKSIZE/INODOSIZE];
    for (size_t i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI; i++) {
        for (size_t j = 0; j < BLOCKSIZE / INODOSIZE; j++) {
            if (bread(i, in) == -1) {
                fprintf(stderr, "Error en leer_sf.c --> %d: %s\nImposible leer el bloque inodo!", errno, strerror(errno));
                return -1;
            }
            // No imprimimos toda la lista para que no pete el terminal
            if (i >= SB.posPrimerBloqueAI && i < SB.posPrimerBloqueAI+3) {
                printf("%d ",in[j].punterosDirectos[0]);
            }
            if (i >= SB.posPrimerBloqueAI && i < SB.posUltimoBloqueAI-3) {
                if (i % 500 == 0) {
                    printf(". ");
                }
            }
            if (i >= SB.posUltimoBloqueAI-3 && i <= SB.posUltimoBloqueAI) {
                printf("%d ",in[j].punterosDirectos[0]);
            }
            
        }
    }
    printf("\n");
    bumount();
    return 0;
}
