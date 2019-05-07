#include "directorios.h"

void printStat(struct STAT stat){
    printf("Tipo: %c\n",stat.tipo);
    printf("Permisos: %u\n",stat.permisos);
    printf("atime: %s\n",ctime(&stat.atime));
    printf("mtime: %s\n",ctime(&stat.mtime));
    printf("ctime: %s\n",ctime(&stat.ctime));
    printf("nlinks: %u\n",stat.nlinks);
    printf("tamEnBytesLog: %u\n",stat.tamEnBytesLog);
    printf("numBloquesOcupados: %u\n",stat.numBloquesOcupados);
    printf("\n");
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        puts("mi_stat.c --> Uso correcto: ./mi_stat <disco> </ruta>");
        return -1;
    }

    const char *camino = argv[2];
    int res;

    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "mi_stat.c --> Error al montar el dispositivo\n");
        return -1;
    }

    struct STAT stat;
    res = mi_stat(camino, &stat);
    if (res < 0)
    {
        fprintf(stderr, "mi_stat.c --> Error al obtener al información de %s\n", camino);
        return res;
    }
    printStat(stat);

    if (bumount() == -1)
    {
        fprintf(stderr, "mi_stat.c --> Error al desmontar el dispositivo\n");
        return -1;
    }

    return res;
}