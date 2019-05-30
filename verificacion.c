#include "verificacion.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("uso: ./verificacion <disco> <directorio_simulacion>\n");
		return -1;
	}
	char *ruta = argv[1];
	bmount(ruta);
	char *camino = argv[2];

	struct STAT stat;
	if(mi_stat(camino, &stat) < 0){
		printf("ERROR: No se ha podido obtener STAT\n");
		return -1;
	}

	if(stat.tamEnBytesLog/sizeof(struct entrada) != NENTRADAS){
		printf("ERROR: No aparecen 100 entradas\n");
		return -1;
	}

	char camino_informe[100];
	char aux[15];
	strcpy(camino_informe,camino);
	sprintf(aux,"informe.txt");
	strcat(camino_informe,aux);

	if(mi_creat(camino_informe, 6) < 0){
		printf("ERROR: EL Informe no se ha podido crear\n");
		return -1;
	}

	unsigned char buffer_ent[NENTRADAS*sizeof(struct entrada)];
	memset(buffer_ent, 0, NENTRADAS*sizeof(struct entrada));
	struct entrada *en;
	en=malloc(sizeof(struct entrada));

	//Leemos las 100 entradas que tiene que tener el fichero
	if(mi_read(camino, &buffer_ent, 0, sizeof(struct entrada)*NENTRADAS) < 0){
		printf("ERROR: Lectura de entradas incorrecta\n");
		return -1;
	}

	char *str;
	unsigned int pid;

	char path_prueba[100];
	unsigned int bytesRe=1;

	char date [80];
	struct tm *ts;

	struct registro buffreg[CANTBUFFER];

	struct registro primera;
	struct registro ultima;
	struct registro mayor;
	struct registro menor;
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
			if((bytesRe=mi_read(path_prueba, &buffreg, j*sizeof(struct registro), sizeof(buffreg))) < 0){
				printf("ERROR: Lectura de entrada incorrecta.-Interno\n");
				return -1;
			}
			for (int n = 0; n < CANTBUFFER; n++)
			{
				if(buffreg[n].pid==pid){
					if (contador==0){
						primera=buffreg[n];
						ultima=buffreg[n];
						mayor=buffreg[n];
						menor=buffreg[n];
					}
					if (buffreg[n].nEscritura<primera.nEscritura){
						primera=buffreg[n];
					}
					if (ultima.nEscritura<buffreg[n].nEscritura){
						ultima=buffreg[n];
					}
					if (buffreg[n].posicion<menor.posicion){
						menor=buffreg[n];
					}
					if (mayor.posicion<buffreg[n].posicion){
						mayor=buffreg[n];
					}
					contador++;
				}
			}
			j+=CANTBUFFER;
		}
		memset(buffer_esc, 0, BLOCKSIZE);

		//NUMERO DE ESCRITURAS:
		sprintf(buffer_esc, "PID: %u\n", pid);
		sprintf(buffer_esc + strlen(buffer_esc), "Numero escrituras: %d\n", contador);

		//PRIMERA ESCRITURA
		sprintf(buffer_esc + strlen(buffer_esc), "Primera escritura\n");
		ts = localtime(&primera.date);
		strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", ts);
		sprintf(buffer_esc + strlen(buffer_esc), "\tFecha: %s\n", date);
		sprintf(buffer_esc + strlen(buffer_esc), "\tnEscritura: %u\n", primera.nEscritura);
		sprintf(buffer_esc + strlen(buffer_esc), "\tPosicion: %u\n", primera.posicion);

		//ULTIMA ESCRITURA
		sprintf(buffer_esc + strlen(buffer_esc), "Última escritura\n");
		ts = localtime(&primera.date);
		strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", ts);
		sprintf(buffer_esc + strlen(buffer_esc), "\tFecha: %s\n", date);
		sprintf(buffer_esc + strlen(buffer_esc), "\tnEscritura: %u\n", ultima.nEscritura);
		sprintf(buffer_esc + strlen(buffer_esc), "\tPosicion: %u\n", ultima.posicion);

		//MAYOR POSICION
		sprintf(buffer_esc + strlen(buffer_esc), "Mayor posición\n");
		ts = localtime(&primera.date);
		strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", ts);
		sprintf(buffer_esc + strlen(buffer_esc), "\tFecha: %s\n", date);
		sprintf(buffer_esc + strlen(buffer_esc), "\tnEscritura: %u\n", mayor.nEscritura);
		sprintf(buffer_esc + strlen(buffer_esc), "\tPosicion: %u\n", mayor.posicion);

		sprintf(buffer_esc + strlen(buffer_esc), "Menor posición\n");
		ts = localtime(&primera.date);
		strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", ts);
		sprintf(buffer_esc + strlen(buffer_esc), "\tFecha: %s\n", date);
		sprintf(buffer_esc + strlen(buffer_esc), "\tnEscritura: %u\n", menor.nEscritura);
		sprintf(buffer_esc + strlen(buffer_esc), "\tPosicion: %u\n", menor.posicion);

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
