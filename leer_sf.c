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

    // Reservamos y liberamos un bloque
    int bloque_reservado = reservar_bloque();
    bread(posSB,&SB);
    printf("Reservado el bloque %d\nCantidad actual de bloques libres: %d\n", bloque_reservado, SB.cantBloquesLibres);
    liberar_bloque(bloque_reservado);
    bread(posSB,&SB);
    printf("Liberado el bloque %d\nCantidad actual de bloques libres: %d\n\n", bloque_reservado, SB.cantBloquesLibres);

    // Lectura de bits individuales de las zonas del FS
    printf("Lectura de bits de las zonas del FS\n");
    printf("Valor del bit leido en posSB (bloque n.%d): %d\n", posSB, leer_bit(posSB));
    printf("Valor del bit leido en posPrimerBloqueMB (bloque n.%d): %d\n", SB.posPrimerBloqueMB, leer_bit(SB.posPrimerBloqueMB));
    printf("Valor del bit leido en posUltimoBloqueMB (bloque n.%d): %d\n", SB.posUltimoBloqueMB, leer_bit(SB.posUltimoBloqueMB));
    printf("Valor del bit leido en posPrimerBloqueAI (bloque n.%d): %d\n", SB.posPrimerBloqueAI, leer_bit(SB.posPrimerBloqueAI));
    printf("Valor del bit leido en posUltimoBloqueAI (bloque n.%d): %d\n", SB.posUltimoBloqueAI, leer_bit(SB.posUltimoBloqueAI));
    printf("Valor del bit leido en posPrimerBloqueDatos (bloque n.%d): %d\n", SB.posPrimerBloqueDatos, leer_bit(SB.posPrimerBloqueDatos));
    printf("Valor del bit leido en posUltimoBloqueDatos (bloque n.%d): %d\n\n", SB.posUltimoBloqueDatos, leer_bit(SB.posUltimoBloqueDatos));

    // Lectura datos inodo del directorio raìz
    printf("Lectura datos del inodo del directorio raíz\n");
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    struct inodo inodo;
    int ninodo = 0; // DEBUG: o 0 o 1

    leer_inodo(ninodo,&inodo);
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("Tipo: %c\n", inodo.tipo);
    printf("Permisos: %c\n", inodo.permisos);
    printf("atime: %s\n", atime);
    printf("mtime: %s\n", mtime);
    printf("ctime: %s\n", ctime);
    printf("N. links: %d\n", inodo.nlinks);
    printf("Tamaño en bytes lógicos: %d\n", inodo.tamEnBytesLog);
    printf("N. de bloques ocupados: %d\n", inodo.numBloquesOcupados);

    bumount();
    printf("Fichero \"%s\" desmontado con éxito.\n",argv[1]);
    return 0;
}
