#include <stdio.h>
#include <string.h>
#include "ficheros.h"



int main(int argc, char **argv) {

	int ninodo = atoi(argv[2]);
	int nbytes = atoi(argv[3]);
	//Validacion de sintaxis
  if (argc != 4) {
    printf("Sintaxis: truncar <nombre_dispositivo> <ninodo> <permisos>\n");
    return -1;
  }
  



}
