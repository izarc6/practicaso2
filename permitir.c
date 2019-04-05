//Validación de sintaxis
//montar dispositivo
//llamada a mi_chmod_f() con los argumentos recibidos, convertidos a entero
//desmontar dispositivo

#include "ficheros.h"

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Sintaxis correcta: ./permitir <nombre_dispositivo> <ninodo> <permisos>\n");
		return -1;
	} else {
		//Montamos dispositivo
		if (bmount(argv[1]) == -1) {
			fprintf(stderr, "Error en permitir.c --> bmount()\n");
			return -1;
		}
		//Extraemos de los argumentos el nº de inodo y los permisos a colocar
		unsigned int ninodo = atoi(argv[2]), permisos = atoi(argv[3]);
		if (mi_chmod_f(ninodo, permisos) == -1) {
			fprintf(stderr, "Error en permitir.c --> mi_chmod_f()\n");
			return -1;
		}
		//Desmontamos dispositivo
		if (bumount() == -1) {
			fprintf(stderr, "Error en permitir.c --> bumount()\n");
			return -1;
		} else {
			return 0;
		}
	}
}
