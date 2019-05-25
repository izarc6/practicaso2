#include "directorios.h"

int main(int argc, char const **argv) {
    if (argc != 3) {
        puts("mi_rm.c --> Uso correcto: ./mi_rm <disco> </ruta>");
        return -1;
    }
    const char *camino = argv[2];
    int errores;
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "mi_rm.c --> Error el montar el disco\n");
        return -1;
    }
    errores = mi_unlink(camino);
    if (errores < 0) {
        fprintf(stderr, "mi_rm.c --> No se ha podido borrar la entrada %s\n", camino);
        return errores;
    }
    if (bumount() == -1) {
        fprintf(stderr, "mi_rm.c --> Error al desmontar el disco\n");
        return -1;
    }
    return errores;
}
