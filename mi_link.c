#include "directorios.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "mi_link.c --> Uso correcto: ./mi_link <disco> </ruta_fichero_original> </ruta_enlace>\n");
        return -1;
    }
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "mi_link.c --> Error al montar el disco\n");
        return -1;
    }
    const char *camino1 = argv[2], *camino2 = argv[3];
    int errores = mi_link(camino1, camino2);
    if (errores < 0) {
        fprintf(stderr, "mi_link.c --> No se han podido enlazar las rutas %s y %s\n", camino1, camino2);
        return -1;
    }
    if (bumount() == -1) {
        fprintf(stderr, "mi_link.c --> Error al desmontar el disco\n");
        return -1;
    }
    return 0;
}
