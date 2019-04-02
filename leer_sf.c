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
    printf("\nReservado el bloque %d\nCantidad actual de bloques libres: %d\n", bloque_reservado, SB.cantBloquesLibres);
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
    int ninodo = 0;

    if (leer_inodo(ninodo,&inodo) == -1) {
      fprintf(stderr, "Error en leer_sf.c %d: %s\nImposible leer el inodo", errno, strerror(errno));
      return -1;
    }
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("Tipo: %c\n", inodo.tipo);
    int permisos = (int) inodo.permisos;
    printf("Permisos: %d\n", permisos);
    printf("atime: %s\n", atime);
    printf("mtime: %s\n", mtime);
    printf("ctime: %s\n", ctime);
    printf("N. links: %d\n", inodo.nlinks);
    printf("Tamaño en bytes lógicos: %d\n", inodo.tamEnBytesLog);
    printf("N. de bloques ocupados: %d\n", inodo.numBloquesOcupados);

    bread(posSB,&SB);
    printf("\nDEBUG - posPrimerInodoLibre: %d\n",SB.posPrimerInodoLibre);

    printf("\nTest de las funciones de traducciòn de bloques inodos\nReservando... ");
    int inodoReservado = reservar_inodo('f',6);
    printf("Se ha reservado el inodo n.%d\n", inodoReservado);

    bread(posSB,&SB);
    printf("\nDEBUG - posPrimerInodoLibre (después de la reserva): %d\n\n",SB.posPrimerInodoLibre);

    printf("\nTRADUCCION DE LOS BLOQUES LOGICOS 8, 204, 30.004, 400.004 y 16.843.019\n");
    printf("Traducciòn bloque lògico n.8: %d\n",traducir_bloque_inodo(1,8,'0'));
    printf("Traducciòn bloque lògico n.204: %d\n",traducir_bloque_inodo(1,204,'0'));
    printf("Traducciòn bloque lògico n.30.004: %d\n",traducir_bloque_inodo(1,30004,'0'));
    printf("Traducciòn bloque lògico n.400.004: %d\n",traducir_bloque_inodo(1,400004,'0'));
    printf("Traducciòn bloque lògico n.16.843.019: %d\n",traducir_bloque_inodo(1,16843019,'0'));

    // Lectura datos inodo leído
    printf("\nLectura datos del inodo %d\n", inodoReservado);
    ninodo = inodoReservado;

    if (leer_inodo(ninodo,&inodo) == -1) {
      fprintf(stderr, "Error en leer_sf.c %d: %s\nImposible leer el inodo", errno, strerror(errno));
      return -1;
    }
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("Tipo: %c\n", inodo.tipo);
    permisos = (int) inodo.permisos;
    printf("Permisos: %d\n", permisos);
    printf("atime: %s\n", atime);
    printf("mtime: %s\n", mtime);
    printf("ctime: %s\n", ctime);
    printf("N. links: %d\n", inodo.nlinks);
    printf("Tamaño en bytes lógicos: %d\n", inodo.tamEnBytesLog);
    printf("N. de bloques ocupados: %d\n", inodo.numBloquesOcupados);

    printf("\nLIBERAMOS EL INODO RESERVADO EN EL NIVEL ANTERIOR Y TODOS SUS BLOQUES\n");
    printf("Hemos liberado el inodo nº: %d\n",liberar_inodo(inodoReservado));
    bread(posSB,&SB);
    printf("\nAhora el primer inodo libre es el --> posPrimerInodoLibre: %d\n",SB.posPrimerInodoLibre);

    bumount();
    printf("Fichero \"%s\" desmontado con éxito.\n",argv[1]);
    return 0;
}
