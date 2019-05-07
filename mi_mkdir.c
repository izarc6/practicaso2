#include "directorios.h"

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "mi_mkdir.c --> Uso correcto: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return -1;
    }

    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "mi_mkdir.c --> Error al montar el dispositivo\n");
        return -1;
    }

    char camino[strlen(argv[3]) + 2];
    strcpy(camino, argv[3]);
    char permisos = atoi(argv[2]);
    int res;

    if (camino[strlen(camino) - 1] != '/')
    {
        strcat(camino, "/");
    }
    res = mi_creat(camino, permisos);
    if (res < 0)
    {
        fprintf(stderr, "mi_mkdir.c --> No se ha podido crear el directorio %s\n", camino);
        return -1;
    }

    if (bumount() == -1)
    {
        fprintf(stderr, "mi_mkdir.c --> Error al desmontar el dispositivo\n");
        return -1;
    }

    return res;
}