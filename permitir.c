///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////PERMITIR.C////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "ficheros.h"


int main(int argc, char **argv) {
	if (argc != 4) {
		printf("Sintaxis: truncar <nombre_dispositivo> <ninodo> <permisos>\n");
		return -1;
	} else {
		void *nombre_fichero = argv[1];
		unsigned char permisos = *argv[2];
		void *ruta = argv[3];
		int bm = bmount(nombre_fichero);
		if (bm == -1) {
			printf("ERROR: APERTURA FICHERO INCORRECTA.\n");
			return -1;
		} else {
			int error = mi_chmod_f(ruta, permisos);
			if (error < 0) {
				switch (error) {
					case -1:
						printf("ERROR: EXTRACCIÓN INCORECTA DEL CAMINO\n");
						return -1;
						break;
					case -2:
						printf("ERROR: NO EXISTE ENTRADA CONSULTA\n");
						return -2;
						break;
					case -3:
						printf("ERROR: NO EXISTE DIRECTORIO INTERMEDIO\n");
						return -3;
						break;
					case -4:
						printf("ERROR DE PERMISOS:NO PERMISOS DE ESCRITURA\n");
						return -4;
						break;
					case -5:
						printf("ERROR: LA ENTRADA YA EXISTE\n");
						return -5;
						break;
					case -8:
						printf("ERROR DE PERMISOS:NO PERMISOS DE LECTURA\n");
						return -8;
						break;
				}
			}
			int bum;
			bum = bumount();
			if (bum == -1) {
				printf("ERROR: CIERRE FICHERO INCORRECTO.\n");
				return -1;
			} else {
				return 0;
			}
		}
	}
}

