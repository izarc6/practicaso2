#include "verificacion.h"

int main (int argc, char **argv) {
	// Comprobaciòn de sintaxis
	if (argc != 3) {
		fprintf(stderr, "uso: ./verificacion <disco> <directorio_simulacion>\n");
		return -1;
	}
	char *ruta = argv[1];
	bmount(ruta);	// Montamos el disco
	char *camino = argv[2];

	fprintf(stderr, "dir_sim: %s\n", camino);

	// Obtenimos las estadisticas de los ficheros del directorio
	struct STAT stat;
	if(mi_stat(camino, &stat) < 0){
		fprintf(stderr, "verificacion.c --> No se ha podido obtener STAT\n");
		return -1;
	}

	// Calculamos el n. de entradas
	if(stat.tamEnBytesLog/sizeof(struct entrada) != NENTRADAS){
		fprintf(stderr, "verificacion.c --> No aparecen 100 entradas\n");
		return -1;
	}

	fprintf(stderr, "numentradas: %d NUMPROCESOS: %d\n", NENTRADAS, NUMPROCESOS);

	// Creaciòn del fichero de informe
	char camino_informe[100];
	char aux[15];
	strcpy(camino_informe,camino);
	sprintf(aux,"informe.txt");
	strcat(camino_informe,aux);

	if(mi_creat(camino_informe, 6) < 0){
		fprintf(stderr, "verificacion.c --> EL Informe no se ha podido crear\n");
		return -1;
	}

	// Creaciòn buffers de lectura
	unsigned char buffer_ent[NENTRADAS*sizeof(struct entrada)];
	memset(buffer_ent, 0, NENTRADAS*sizeof(struct entrada));
	struct entrada *en;
	en=malloc(sizeof(struct entrada));

	//Leemos las 100 entradas que tiene que tener el fichero
	if(mi_read(camino, &buffer_ent, 0, sizeof(struct entrada)*NENTRADAS) < 0){
		fprintf(stderr, "verificacion.c --> Lectura de entradas incorrecta\n");
		return -1;
	}

	char *str;
	unsigned int pid;

	char path_prueba[100];
	unsigned int bytesRe=1;

	char date [80];
	struct tm *ts;

	struct INFORMACION info;
	struct REGISTRO buffreg[INDEX];
	char buffer_esc[BLOCKSIZE];
	int off_info=0;

	for (int i = 0; i < NENTRADAS; i++) {
		memcpy(en,i*sizeof(struct entrada)+buffer_ent,sizeof(struct entrada));
		str=strchr(en->nombre,'_')+1;
		pid=atoi(str);
		strcpy(path_prueba,camino);
		strcat(path_prueba,en->nombre);
		sprintf(aux,"/prueba.dat");
		strcat(path_prueba,aux);

		int j=0; int contador=0;

		while(bytesRe>0 && contador<50){
			memset(&buffreg, 0, sizeof(buffreg));
			//Leemos las 100 entradas que tiene que tener el fichero
			if((bytesRe=mi_read(path_prueba, &buffreg, j*sizeof(struct REGISTRO), sizeof(buffreg))) < 0){
				fprintf(stderr, "verificacion.c --> Lectura de entrada incorrecta.\n");
				return -1;
			}
			for (int n = 0; n < INDEX; n++) {
				if(buffreg[n].pid==pid){
					if (contador==0){
						info.PrimeraEscritura=buffreg[n];
						info.UltimaEscritura=buffreg[n];
						info.MayorPosicion=buffreg[n];
						info.MenorPosicion=buffreg[n];
					}
					if (buffreg[n].nEscritura<info.PrimeraEscritura.nEscritura){
						info.PrimeraEscritura=buffreg[n];
					}
					if (info.UltimaEscritura.nEscritura<buffreg[n].nEscritura){
						info.UltimaEscritura=buffreg[n];
					}
					if (buffreg[n].nRegistro<info.MenorPosicion.nRegistro){
						info.MenorPosicion=buffreg[n];
					}
					if (info.MayorPosicion.nRegistro<buffreg[n].nRegistro){
						info.MayorPosicion=buffreg[n];
					}
					contador++;
				}
			}
			j+=INDEX;
		}
		memset(buffer_esc, 0, BLOCKSIZE);

		//NUMERO DE ESCRITURAS:
		sprintf(buffer_esc, "\nPID: %u\n", pid);
		sprintf(buffer_esc + strlen(buffer_esc), "Numero escrituras: %d\n", contador);

		//PRIMERA ESCRITURA
		ts = localtime(&info.PrimeraEscritura.fecha);
		strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", ts);
		sprintf(buffer_esc + strlen(buffer_esc), "Primera Escritura\t%u\t%u\t%s\n", info.PrimeraEscritura.nEscritura, info.PrimeraEscritura.nRegistro, date);

		//ULTIMA ESCRITURA
		ts = localtime(&info.UltimaEscritura.fecha);
		strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", ts);
		sprintf(buffer_esc + strlen(buffer_esc), "Última Escritura\t%u\t%u\t%s\n", info.UltimaEscritura.nEscritura, info.UltimaEscritura.nRegistro, date);

		//MENOR POSICION
		ts = localtime(&info.MenorPosicion.fecha);
		strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", ts);
		sprintf(buffer_esc + strlen(buffer_esc), "Menor Posición\t\t%u\t%u\t%s\n", info.MenorPosicion.nEscritura, info.MenorPosicion.nRegistro, date);

		//MAYOR POSICION
		ts = localtime(&info.MayorPosicion.fecha);
		strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", ts);
		sprintf(buffer_esc + strlen(buffer_esc), "Mayor Posición\t\t%u\t%u\t%s\n", info.MayorPosicion.nEscritura, info.MayorPosicion.nRegistro, date);

		//Escritura
		if(mi_write(camino_informe, buffer_esc, off_info, strlen(buffer_esc))< 0){
			fprintf(stderr, "Error escribiendo informe.txt");
			return 0;
		}
		write(1, buffer_esc, strlen(buffer_esc));
		off_info+=strlen(buffer_esc);
	}
	bumount();
}
