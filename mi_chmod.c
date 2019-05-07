#include "directorios.h"

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        puts("mi_chmod.c --> Uso correcto: ./mi_chmod <disco> <permisos> </ruta>");
        return -1;
    }

    const char *camino = argv[3];
    char permisos = atoi(argv[2]);
    int res;

    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "mi_chmod.c --> Error al montar el dispositivo\n");
        return -1;
    }

    res = mi_chmod(camino, permisos);
    if (res < 0)
    {
        fprintf(stderr, "mi_chmod.c --> Error al cambiar los permisos de %s\n", camino);
        return res;
    }

    if (bumount() == -1)
    {
        fprintf(stderr, "mi_chmod.c --> Error al desmontar el dispositivo\n");
        return -1;
    }

    return res;
}