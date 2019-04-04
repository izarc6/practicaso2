#include "ficheros.h"
//Le pasaremos por línea de comandos un nº de inodo obtenido con el programa
//anterior (además del nombre del dispositivo). Su funcionamiento tiene que ser
// similar a la función cat de linux, explorando TODO el fichero


int main(int argc, char **argv) {
  //errores generales
  if (argc != 3) {
		printf("Sintaxis: leer <nombre_dispositivo> <ninodo>\n");
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
    struct STAT stat;
    unsigned char buffer[BLOCKSIZE];
    memset(buffer, 0, BLOCKSIZE);

    printf("Leyendo archivo\n");
    int valor = mi_read_f(ninodo, buffer, offset, BLOCKSIZE);
    while (valor > 0) {
      for (size_t i = 0; i < valor; i++) {
        printf("%c", buffer[i]);
      }
      B_leidos = B_leidos + valor;
      offset = offset + BLOCKSIZE;
      memset(buffer,0,BLOCKSIZE);
      valor = mi_read_f(ninodo, buffer, offset, BLOCKSIZE);
    }
    printf("\n");
    printf("Bytes leídos: %d\n", B_leidos);
    mi_stat_f(ninodo, &stat);
    printf("Tamaño en bytes lógicos: %d\n", stat.tamEnBytesLog);

    if (bumount() == -1) {
        fprintf(stderr, "Error en leer.c --> %d: %s\n", errno, strerror(errno));
    }

}
