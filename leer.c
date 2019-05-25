#include "ficheros.h"
//Le pasaremos por línea de comandos un nº de inodo obtenido con el programa
//anterior (además del nombre del dispositivo). Su funcionamiento tiene que ser
// similar a la función cat de linux, explorando TODO el fichero


int main(int argc, char **argv) {

  int tam_lectura = 1500;
  //errores generales
  if (argc != 3) {
    fprintf(stderr, "Sintaxis: leer <nombre_dispositivo> <ninodo>\n");
    return -1;
  }
  //control de bmount
  if (bmount(argv[1]) == -1) {
    fprintf(stderr, "Error en leer.c --> %d: %s\nFallo en bmount", errno, strerror(errno));
    return -1;
  }
  int ninodo = atoi(argv[2]);
  int offset = 0;
  int B_leidos = 0;
  int valor = 0;
  struct STAT stat;
  unsigned char buffer[tam_lectura];
  memset(buffer, 0, tam_lectura);

  //printf("Leyendo archivo\n");
  while ((valor = mi_read_f(ninodo, buffer, offset, tam_lectura)) > 0) {
    B_leidos += valor;
    write(1, buffer, valor);
    offset += tam_lectura;
    memset(buffer, 0, tam_lectura);
    valor = mi_read_f(ninodo,buffer,offset,tam_lectura);
  }
  fprintf(stderr ,"\nBytes leídos: %d\n", B_leidos);
  mi_stat_f(ninodo, &stat);
  fprintf(stderr ,"Tamaño en bytes lógicos: %d\n", stat.tamEnBytesLog);

  if (bumount() == -1) {
    fprintf(stderr, "Error en leer.c --> %d: %s\n", errno, strerror(errno));
  }

}
