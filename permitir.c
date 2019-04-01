///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////PERMITIR.C////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "directorios.h"


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
						return ERROR_EXTRAER_CAMINO;
						break;
					case -2:
						printf("ERROR: NO EXISTE ENTRADA CONSULTA\n");
						return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
						break;
					case -3:
						printf("ERROR: NO EXISTE DIRECTORIO INTERMEDIO\n");
						return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
						break;
					case -4:
						printf("ERROR DE PERMISOS:NO PERMISOS DE ESCRITURA\n");
						return ERROR_PERMISO_ESCRITURA;
						break;
					case -5:
						printf("ERROR: LA ENTRADA YA EXISTE\n");
						return ERROR_ENTRADA_YA_EXISTENTE;
						break;
					case -8:
						printf("ERROR DE PERMISOS:NO PERMISOS DE LECTURA\n");
						return ERROR_PERMISO_LECTURA;
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
