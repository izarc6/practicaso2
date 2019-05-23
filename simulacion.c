#include "simulacion.h"

char dirPrueba[20];
int acabados;

void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
    acabados++;
    //Podemos testear qué procesos van acabando:
    fprintf(stderr, "acabado: %d total acabados: %d\n", ended, acabados);
  }
}

/* Funcion que ejecutra cada proceso y que se encargara de crear un directorio y un fichero
* para cada proceso, guardando en el fichero las correspondientes escrituras (1-50).
*/
void proceso(int pid, char *disco){
  char pidDirectorio[8];
  char camino[50];
  char camino2[200];
  char buffer[60*20];
  struct REGISTRO registro;
  if (bmount(disco) == -1) {
    fprintf(stderr, "Error al montar el disco para el proceso con PID %d\n", pid);
    bumount();
    exit(1);
  }
  sprintf(pidDirectorio,"proceso_%d/",pid);
  strcpy(camino,dirPrueba);
  strcat(camino, pidDirectorio);
  //Creamos el directorio del proceso
  if(mi_creat(camino, '7') != 0){
    bumount();
    exit(1);
  }
  strcpy(camino2, camino);
  sprintf(camino, "%sprueba.dat", camino);
  //Creamos pureba.dat
  if(mi_creat(camino, '6') != 0){
    bumount();
    exit(1);
  }
  memset(buffer,0,BLOCKSIZE);
  //Escribimos los registros
  srand(time(NULL) + getpid());
  for(int i = 0; i <= NUMESCRITURAS; i++)
  {
    registro.fecha = time(NULL);
    registro.pid = getpid();
    registro.nEscritura = i + 1;
    registro.nRegistro =  rand() % REGMAX;
    if(mi_write(camino,&registro,registro.nRegistro*sizeof(struct REGISTRO),sizeof(struct REGISTRO)) < 0){
      fprintf(stderr, "Fallo en la escritura en %s\n", camino);
      exit(1);
    }
    usleep(50000);
  }
  bumount();
  exit(0);
}

int main (int argc, char **argv) {
  char fech[16];
  char f[8] = "/simul_";
  struct tm *ts;
  char buffer[64*200];

  //Pasamos sintaxis correcta
  if (argc != 2) {
    fprintf(stderr, "Sintaxis correcta: ./simulacion <disco>\n");
    return -1;
  }

  //Montamos el disco
  if (bmount(argv[1]) == -1) return -1;

  //Creamos el directorio
  time_t t = time(NULL);
  ts = localtime(&t);
  strftime(fech, sizeof(fech), "%Y%m%d%H%M%S/", ts);
  strcpy(dirPrueba, f);
  strcat(dirPrueba, fech);
  if(mi_creat(dirPrueba, '7') < 0) {
    fprintf(stderr, "El directorio '%s' no se ha podido crear\n", dirPrueba);
    bumount();
    return -1;
  } else {
    printf("Directorio simulación: %s\n", dirPrueba);
  }
  acabados = 0;
  //Asignamos la función enterrador a la señal de finalización de un hijo
  signal(SIGCHLD, reaper);

  for (int i = 0; i < PROCESOS; i++){
    if(fork() == 0) proceso(getpid(), argv[1]);
    //Esperamos 0.2 s
    usleep(200000);
  }

  //Esperamos a que todos los procesos acaben
  while (acabados < PROCESOS) pause();
  memset(buffer,0,BLOCKSIZE);
  fprintf(stderr, "Numero de entradas dir total: %d\n", mi_dir(dirPrueba,(char *)buffer));
  bumount();
  exit(0);
}
