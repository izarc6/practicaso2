#include <signal.h>
#include <sys/wait.h>
#include "simulacion.h"

#define POSMAX 500000
#define PROCESOS 100 //Número de procesos
#define NUMESCRITURAS 50 //Número de escrituras por procreso

char dirPrueba[20];
int acabados;

//El enterrador trata los procesos zombies e incrementa la variable de acabados
void reaper() {
  pid_t ended;
  signal(SIGCHLD, reaper);
  while((ended=waitpid(-1, NULL, WNOHANG)) > 0){
    acabados++;
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
  struct registro registro;

  if (bmount(disco) == -1)
  {
    fprintf(stderr, "Error al montar el disco para el proceso con PID %d\n", pid);
    bumount();
    exit(1);
  }

  sprintf(pidDirectorio,"proceso_%d/",pid);

  strcpy(camino,dirPrueba);
  strcat(camino, pidDirectorio);

//Creamos el directorio del proceso
  if(mi_creat(camino, 7) != 0){
    bumount();
    exit(1);
  }

  strcpy(camino2, camino);
  sprintf(camino, "%sprueba.dat", camino);


//Creamos pureba.dat
  if(mi_creat(camino, 6) != 0){
    bumount();
    exit(1);
  }

  memset(buffer,0,BLOCKSIZE);


  //Escribimos los registros
  srand(time(NULL) + getpid());
  for(int i = 0; i <= NUMESCRITURAS; i++)
  {
    registro.pid = getpid();
    registro.nEscritura = i;
    registro.posicion = rand()%POSMAX;
    registro.date = time(NULL);
    if(mi_write(camino,&registro,registro.posicion*sizeof(struct registro),sizeof(struct registro)) < 0){
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

  if (argc != 2) {
    fprintf(stderr, "Número de parametros incorrectos\n");
    return -1;
  }

  if (bmount(argv[1]) == -1) return -1;

  time_t t = time(NULL);
  ts = localtime(&t);
  strftime(fech, sizeof(fech), "%Y%m%d%H%M%S/", ts);
  strcpy(dirPrueba, f);
  strcat(dirPrueba, fech);
  //Creamos el directorio de la simulación
  if(mi_creat(dirPrueba, 7) < 0) {
    fprintf(stderr, "ERROR: El directorio '%s' no se ha podido crear\n", dirPrueba);
    bumount();
    return -1;
  }
  acabados = 0;
  signal(SIGCHLD, reaper);

  for (int i = 0; i < PROCESOS; i++){
    if(fork() == 0) proceso(getpid(), argv[1]);
    //Provocamos la espera de creacion de procesos (0.2 s)
    usleep(200000);
  }

  //Esperamos a que todos los procesos acaben
  while (acabados < PROCESOS) pause();

  memset(buffer,0,BLOCKSIZE);
  printf("Directorio raiz: %s\n", dirPrueba);

  fprintf(stderr, "Numero de entradas dir total: %d\n", mi_dir(dirPrueba,(char *)buffer));
  //Desmontamos el sistema de ficheros
  bumount();
  exit(0);
}
