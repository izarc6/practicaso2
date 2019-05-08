#include "directorios.h"

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "mi_escribir.c --> Uso correcto: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return -1;
    }
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "mi_escribir.c --> Error al montar el disco\n");
        return -1;
    }
    char *camino = argv[2];
    char *texto = argv[3];
    unsigned int offset = atoi(argv[4]), length = strlen(texto);
    if (camino[strlen(camino - 1)] == '/') {
        fprintf(stderr, "mi_escribir.c --> La entrada %s no es un fichero\n", camino);
        return -1;
    }
    //Los permisos de escritura se comprueban en mi_write_f, llamado desde mi_write
    int errores = mi_write(camino, texto, offset, length);
    if (errores < 0) {
        fprintf(stderr, "mi_escribir.c --> No se ha podido escribir en la entrada %s\n", camino);
        return -1;
    }
    fprintf(stderr, "\nNº de bytes escritos: %d\n", errores);
    if (bumount() == -1) {
        fprintf(stderr, "mi_escribir.c --> Error al desmontar disco\n");
        return -1;
    }
    return 0;
}
