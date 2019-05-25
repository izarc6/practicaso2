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
    fprintf(stderr, "Error en truncar.c --> %d: %s\nFallo en bmount\n", errno, strerror(errno));
    return -1;
  }
  mi_truncar_f(ninodo, nbytes);

  struct inodo inodo;
  leer_inodo(ninodo, &inodo);

  struct tm *ts;
  char atime[80];
  char mtime[80];
  char ctime[80];
  ts = localtime(&inodo.atime);
  strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
  ts = localtime(&inodo.mtime);
  strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
  ts = localtime(&inodo.ctime);
  strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

  printf("\nDATOS INODO: %d\n", ninodo);
  printf("Tipo: %c\n", inodo.tipo);
  int permisos = (int) inodo.permisos;
  printf("Permisos: %d\n", permisos);
  printf("atime: %s\n", atime);
  printf("mtime: %s\n", mtime);
  printf("ctime: %s\n", ctime);
  printf("N. links: %d\n", inodo.nlinks);
  printf("Tamaño en bytes lógicos: %d\n", inodo.tamEnBytesLog);
  printf("N. de bloques ocupados: %d\n\n", inodo.numBloquesOcupados);

  if(bumount() == -1){
    fprintf(stderr, "Error en truncar.c --> %d: %s\nError al cerrar fichero\n", errno, strerror(errno));
    return -1;
  }
  return 0;
}
