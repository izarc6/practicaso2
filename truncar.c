<<<<<<< HEAD
#include <stdio.h>
#include <string.h>
#include "ficheros.h"



int main(int argc, char **argv) {
	void *nombre_fichero = argv[1];
	int ninodo = atoi(argv[2]);
	int nbytes = atoi(argv[3]);
	//Validacion de sintaxis
	if(argc==4){
	//montar dispositivo
		if(bmount(nombre_fichero)==-1){
			printf("Error de fichero");
		}else{
			//llamada a mi_truncar_f(ninodo, nbytes)
			mi_truncar_f(ninodo,nbytes);
		}
	//desmontar dispositivo
		int bum;
		bum=bumount();
		if(bum==-1){
			printf("Error en el close del fitxer\n");
			return -1;
		} else {
			return 0;
		}
	}else{
			printf("Sintaxis: ./truncar <nombre_dispositivo> <ninodo> <nbytes>\n");

	}





}
=======
#include "ficheros.h"

int main (int argc, char **argv) {
    //TODO
    return 0;
}
>>>>>>> 5da354dddca143b802e1b35f092dc8afc94b15d6
