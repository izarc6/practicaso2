//Validación de sintaxis
//montar dispositivo
//llamada a mi_chmod_f() con los argumentos recibidos, convertidos a entero
//desmontar dispositivo

#include "ficheros.h"


int main(int argc, char **argv) {
	if (argc != 4) {
		printf("Sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
		return -1;
	}

	  unsigned int ninodo = atoi(argv[2]);
		unsigned char permisos = argv[3];

		if (bmount(argv[1]) == -1) {
      fprintf(stderr, "Error en permitir.c --> %d: %s\nFallo en bmount", errno, strerror(errno));
      return -1;
		}
			mi_chmod_f(ninodo, permisos);

		   bumount();

	return 0;
}
