#include "directorios.h"

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Sintaxis: ./mi_touch <nombre_dispositivo> <permisos> <ruta>\n");
		return -1;
	} else {
		void *nombre = argv[1];
		unsigned char permisos = *argv[2];
		int error = bmount(nombre);
		if (error == -1) {
			fprintf(stderr, "Error en apertura de fichero\n");
			return -1;
		} else {
			int charF = strlen(argv[3])-1;
			if(argv[3][charF]!='/'){
				if((error = mi_creat(argv[3], permisos)) == 0){
					fprintf(stderr, "El fichero %s se ha creado correctamente con permisos %c\n", argv[3], permisos);
				} else {
					return -1;
				}
			} else {
				fprintf(stderr, "Introduce un fichero válido\n");
			}
			error = bumount();
			if (error == -1) {
				fprintf(stderr, "Cierre de fichero incorrecto\n");
				return -1;
			} else {
				return 0;
			}
		}
	}
}
