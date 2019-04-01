#include "ficheros.h"

//Validación de sintaxis
//montar dispositivo
//llamada a mi_truncar_f() con los argumentos recibidos, convertidos a entero
//desmontar dispositivo




int main(int argc, char **argv) {


	//Validacion de sintaxis
  	if (argc != 4) {
    	printf("Sintaxis: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
    	return -1;
  	}
  	unsigned int ninodo = atoi(argv[2]);
	unsigned int nbytes = atoi(argv[3]);

  	if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error en truncar.c --> %d: %s\nFallo en bmount", errno, strerror(errno));
        return -1;
  	}
  	mi_truncar_f(ninodo, nbytes);

  	bumount();

  	return 0;
}





