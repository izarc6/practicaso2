#include "directorios.h"

// Permite mostrar la informaciòn acerca del inodo de un fichero o de un directorio empleando mi_stat()
int main(int argc, char const *argv[]) {
    if (argc != 3) {
        puts("mi_stat.c --> Uso correcto: ./mi_stat <disco> </ruta>");
        return -1;
    }
    const char *camino = argv[2];
    int errores;
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "mi_stat.c --> Error al montar el dispositivo\n");
        return -1;
    }
    struct STAT stat;
    errores = mi_stat(camino, &stat);
    if (errores < 0) {
        fprintf(stderr, "mi_stat.c --> Error al obtener al información de %s\n", camino);
        return errores;
    }

    printf("Tipo: %c\n",stat.tipo);
    printf("Permisos: %c\n",stat.permisos);
    printf("atime: %s",ctime(&stat.atime));
    printf("mtime: %s",ctime(&stat.mtime));
    printf("ctime: %s",ctime(&stat.ctime));
    printf("nlinks: %u\n",stat.nlinks);
    printf("tamEnBytesLog: %u\n",stat.tamEnBytesLog);
    printf("numBloquesOcupados: %u\n",stat.numBloquesOcupados);

    if (bumount() == -1) {
        fprintf(stderr, "mi_stat.c --> Error al desmontar el dispositivo\n");
        return -1;
    }
    return errores;
}
