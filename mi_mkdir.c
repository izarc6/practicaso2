#include "directorios.h"

// Crea un fichero o directorio empleando mi_creat()
int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "mi_mkdir.c --> Uso correcto: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return -1;
    }
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "mi_mkdir.c --> Error al montar el dispositivo\n");
        return -1;
    }
    char camino[strlen(argv[3]) + 2];
    strcpy(camino, argv[3]);
    int permisos_check = atoi(argv[2]);
    char permisos = ' ';
    // Comprobaciòn del valor de los permisos
    if ((permisos_check > 0 && permisos_check <= 7) || strcmp(argv[2],"0") == 0) {  // Si el valor de permisos està entre 0 y 7, se sigue con la ejecuciòn
        permisos = argv[2][0];
    } else if (strcmp(argv[2],"0") != 0){
        fprintf(stderr, "mi_mkdir.c --> '%s' no es considerado un valor de permisos valido. Usar un valor entre 0 y 7.\n",argv[2]);
        return -1;
    }
    int errores;
    if (camino[strlen(camino) - 1] != '/') {
        strcat(camino, "/");
    }
    errores = mi_creat(camino, permisos);
    if (errores < 0) {
        fprintf(stderr, "mi_mkdir.c --> No se ha podido crear el directorio %s\n", camino);
        return -1;
    }
    if (bumount() == -1) {
        fprintf(stderr, "mi_mkdir.c --> Error al desmontar el dispositivo\n");
        return -1;
    }
    return errores;
}
