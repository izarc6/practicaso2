#include "ficheros.h"


/*
Escribirá texto en uno o varios inodos haciendo uso de reservar_inodo ('f', 6)
para obtener un nº de inodo,que mostraremos por pantalla y además utilizaremos
como parámetro para mi_write_f().
*/
//pasaremos el argumento por parametro, y calculamos su longitud con streln
int main(int argc, char **argv) {

	if(argc != 4){
		printf("Sintaxis: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\nOffsets: 0, 5120, 256000, 30720000, 71680000\nSi diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n");
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
	struct STAT stat;//cambiado a puntero

	if(bmount(nombre_fichero)==-1){
		fprintf(stderr, "Error en escribir.c, nombre_fichero --> %d: %s\n", errno, strerror(errno));
		return -1;
	}
	int nInodo = reservar_inodo('f',6);
	printf("longitud texto: %d\n", longitud);
	int bEscritos = mi_write_f(nInodo,string,OFFSETS[0],longitud);
	memset(buffer,0,longitud);
	mi_stat_f(nInodo, &stat);

	struct tm *ts;
	char atime[80];
	char mtime[80];
	char ctime[80];
	ts = localtime(&stat.atime);
	strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&stat.mtime);
	strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&stat.ctime);
	strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

	write(2, strings, strlen(strings));
	for (int i = 1; i < 5; ++i){
		if (reserva_inodo!=0){
			nInodo = reservar_inodo('f',6);
		}
		int bEscritos = mi_write_f(nInodo,string,OFFSETS[i],longitud);
		memset(buffer,0,longitud);
		mi_stat_f(nInodo,&stat);
		printf("\nNº inodo reservado=%d\noffset:%d\nBytes escritos: %d\n\n",nInodo,OFFSETS[i-1],bEscritos);
		printf("DATOS INODO: %d\n", nInodo);
		printf("Tipo: %c\n", stat.tipo);
		int permisos = (int) stat.permisos;
		printf("Permisos: %d\n", permisos);
		printf("atime: %s\n", atime);
		printf("mtime: %s\n", mtime);
		printf("ctime: %s\n", ctime);
		printf("N. links: %d\n", stat.nlinks);
		printf("Tamaño en bytes lógicos: %d\n", stat.tamEnBytesLog);
		printf("N. de bloques ocupados: %d\n", stat.numBloquesOcupados);
		write(2, strings, strlen(strings));
	}

	if(bumount()==-1){
		fprintf(stderr, "Error en escribir.c --> %d: %s\n", errno, strerror(errno));
		return -1;
	}
	return 0;
}
