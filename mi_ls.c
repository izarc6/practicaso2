#include "directorios.h"

// Lista el contenido de un directorios empleando mi_dir()
int main(int argc, char const *argv[]) {
    if (argc != 3) {
        puts("mi_ls.c --> Uso correcto: ./mi_ls <disco> </ruta_directorio>");
        return -1;
    }
    const char *camino = argv[2];
    char buffer[1000000];
    int errores;
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "mi_ls.c --> Error al montar el dispositivo\n");
        return -1;
    }
    errores = mi_dir(camino, buffer);
    if (errores < 0) {
        fprintf(stderr, "mi_ls.c --> Error al listar el directorio %s\n", camino);
        return errores;
    }
    printf("%s", buffer);
    if (bumount() == -1) {
        fprintf(stderr, "mi_ls.c --> Error al desmontar el dispositivo\n");
        return -1;
    }
    return errores;
}
