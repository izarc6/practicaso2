#include "ficheros.h"


/*
Escribirá texto en uno o varios inodos haciendo uso de reservar_inodo ('f', 6)
para obtener un nº de inodo,que mostraremos por pantalla y además utilizaremos
como parámetro para mi_write_f().
*/
//pasaremos el argumento por parametro, y calculamos su longitud con streln
int main(int argc, char **argv) {

	if(argc!=4){
    printf("Sintaxis: escribir <nombre_dispositivo> <$(cat fichero)> <diferentes_inodos>\nOffsets: 0, 5120, 256000, 30720000, 71680000");
    return -1;
  }
		void *nombre_fichero=argv[1]; //Nombre fichero
		char *string=argv[2];         //Argumento
		int reserva_inodo=atoi(argv[3]); //Offset inodo.
		int longitud = strlen(string);//Ya que lo hacemos por parámetro
		char buffer [longitud];
		int OFFSETS[5]={0,5210,256000,30720000,71680000};
		strcpy(buffer,string);
		char strings[128]; //cambiar por fprintf
		struct STAT *state;//cambiado a puntero

		if(bmount(nombre_fichero)==-1){
			fprintf(stderr, "Error en escribir.c, nombre_fichero --> %d: %s\n", errno, strerror(errno));
      return -1;
		}

			int resNinodo = reservar_inodo('f',6);

			int j =mi_write_f(resNinodo,string,OFFSETS[0],longitud);
			memset(buffer,0,longitud);
			mi_stat_f(resNinodo,&state);
			sprintf(strings,"\nNº inodo reservado=%d\noffset:%d\nBytes escritos: %d\nstat.tamEnBytesLog=%d\nstat.numBloquesOcupados=%d\n",resNinodo,OFFSETS[0],j, state.tamEnBytesLog,state.numBloquesOcupados);
			write(2,strings,strlen(strings));

			for (int i = 1; i < 5; ++i){
			if (reserva_inodo!=0){
				resNinodo = reservar_inodo('f',6);
			}
			int j=mi_write_f(resNinodo,string,OFFSETS[i],longitud);
			memset(buffer,0,longitud);

			mi_stat_f(resNinodo,&state);
			sprintf(strings,"\nNº inodo reservado=%d\noffset:%d\nBytes escritos: %d\nstat.tamEnBytesLog=%d\nstat.numBloquesOcupados=%d\n",resNinodo,OFFSETS[i],j, state.tamEnBytesLog,state.numBloquesOcupados);
			write(2,strings,strlen(strings));


			}

		if(bumount()==-1){
			fprintf(stderr, "Error en escribir.c --> %d: %s\n", errno, strerror(errno));
			return -1;
		}
			return 0;
		}


}
