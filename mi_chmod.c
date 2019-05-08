#include "directorios.h"

// Permite cambiar los permisos de un fichero o de un directorio empleando mi_chmod()
int main(int argc, char const *argv[]) {
    if (argc != 4) {
        puts("mi_chmod.c --> Uso correcto: ./mi_chmod <disco> <permisos> </ruta>");
        return -1;
    }
    const char *camino = argv[3];
    int permisos_check = atoi(argv[2]);
    char permisos = ' ';
    if (permisos_check >= 0 && permisos_check <= 7) {  // Si el valor de permisos està entre 0 y 7, se sigue con la ejecuciòn
        permisos = argv[2][0];
    } else {
        fprintf(stderr, "mi_chmod.c --> '%s' no es considerado un valor de permisos valido. Usar un valor entre 0 y 7.\n",argv[2]);
        return -1;
    }
    int errores;
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "mi_chmod.c --> Error al montar el dispositivo\n");
        return -1;
    }
    errores = mi_chmod(camino, permisos);
    if (errores < 0) {
        fprintf(stderr, "mi_chmod.c --> Error al cambiar los permisos de %s\n", camino);
        return errores;
    }
    if (bumount() == -1) {
        fprintf(stderr, "mi_chmod.c --> Error al desmontar el dispositivo\n");
        return -1;
    }
    return errores;
}
