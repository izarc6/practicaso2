#include "simulacion.h"

char dirPrueba[69];
char camino[69];
char camino2[200];
static int acabados;

void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
    acabados++;
    //Podemos testear qué procesos van acabando:
    //fprintf(stderr, "Proceso %d: Completadas %d escrituras en %s\n", acabados, NUMESCRITURAS, camino);
    //fflush(stderr);
  }
}

/* Funcion que ejecutra cada proceso y que se encargara de crear un directorio y un fichero
* para cada proceso, guardando en el fichero las correspondientes escrituras (1-50).
*/
void proceso(int pid, char *disco){
  char pidDirectorio[16];
  struct REGISTRO registro;
  if (bmount(disco) == -1) {
    fprintf(stderr, "Error al montar el disco para el proceso con PID %d\n", pid);
    bumount();
    exit(1);
  }
  sprintf(pidDirectorio, "proceso_%d/", pid);
  //fprintf(stderr, "proceso_%d/", pid);
  memset(camino,0,sizeof(camino));
  strcpy(camino, dirPrueba);
  strcat(camino, pidDirectorio);
  //Creamos el directorio del proceso
  if(mi_creat(camino, '7') != 0){
    bumount();
    exit(1);
  }
  memset(camino2,0,sizeof(camino2));
  strcpy(camino2, camino);
  fprintf(stderr, "Proceso %d: Completadas %d escrituras en %sprueba.dat\n", acabados, NUMESCRITURAS, camino);
  fflush(stderr);
  //fprintf(stderr, "%sprueba.dat\n", camino);
  sprintf(camino2, "%sprueba.dat", camino2);

  //fprintf(stderr, "**DEBUG - camino completo: %s**\n",camino2);

  //Creamos pureba.dat
  if(mi_creat(camino2, '6') != 0){
    fprintf(stderr, "simulacion.c - Error en la creaciòn del fichero \"prueba.dat\"\n");
    bumount();
    exit(1);
  }
  //fprintf(stderr, "DEBUG - Creado fichero prueba.dat\n");

  //Escribimos los registros
  srand(time(NULL) + getpid());
  for(int i = 0; i < NUMESCRITURAS; i++) {
    registro.fecha = time(NULL);
    registro.pid = getpid();
    registro.nEscritura = i + 1;
    registro.nRegistro =  rand() % REGMAX;
    if(mi_write(camino2,&registro,registro.nRegistro*sizeof(struct REGISTRO),sizeof(struct REGISTRO)) < 0){
      fprintf(stderr, "Fallo en la escritura en %s\n", camino2);
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
  //Mostramos sintaxis correcta
  if (argc != 2) {
    fprintf(stderr, "Sintaxis correcta: ./simulacion <disco>\n");
    return -1;
  }
  //Montamos el disco
  if (bmount(argv[1]) == -1) return -1;
  fprintf(stderr, "*** Simulación de %d procesos realizando cada uno %d escrituras ***\n", NUMPROCESOS, NUMESCRITURAS);
  //Creamos el directorio
  time_t t = time(NULL);
  ts = localtime(&t);
  memset(fech,0,sizeof(fech));
  strftime(fech, sizeof(fech), "%Y%m%d%H%M%S/", ts);
  memset(dirPrueba,0,sizeof(dirPrueba));
  strcpy(dirPrueba, f);
  strcat(dirPrueba, fech);
  if(mi_creat(dirPrueba, '7') < 0) {
    fprintf(stderr, "El directorio '%s' no se ha podido crear\n", dirPrueba);
    bumount();
    return -1;
  } else {
    fprintf(stderr, "Directorio simulación: %s\n", dirPrueba);
  }
  acabados = 0;
  //Asignamos la función enterrador a la señal de finalización de un hijo
  signal(SIGCHLD, reaper);
  for (int i = 1; i <= NUMPROCESOS; i++) {
    if(fork() == 0) {
      proceso(getpid(), argv[1]);
    }
    //Esperamos 0.2 s
    usleep(200000);
  }
  //Esperamos a que todos los procesos acaben
  while (acabados < NUMPROCESOS) {
    pause();
  }
  fprintf(stderr, "Total de procesos terminados: %d.\n", acabados);
  bumount();
  exit(0);
}
