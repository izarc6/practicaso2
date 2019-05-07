#include "directorios.h"

int c_tipo;
char *c_inicial, *c_final;

// Dado un camino que empieza con '/', separa lo que està contenido entre los primeros dos '/' en inicial y lo demàs en final
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
  int tipoInt;
  unsigned int lengthC = strlen(camino + 1), lengthF;
  if (camino[0] != '/' || camino == NULL) {
    fprintf(stderr, "Error en directorios.c extraer_camino() --> Debe empezar con el símbolo /");
    return -1;
  }
  if (lengthC < 1 || lengthC > 60) {
    fprintf(stderr, "Error en directorios.c extraer_camino() --> Path inválido\n");
    return -1;
  }
  final = strchr(camino + 1, '/');
  if (final == NULL) {
    tipoInt = 0; //Es Fichero
    final = "/";
    lengthF = 0;
  }
  else {
    tipoInt = 1; //Es Directorio
    lengthF = strlen(final);
  }

  // WORKAROUND - Aquì me daba segfault porque inicial es NULL, portanto creo otro char* de la longitud que
  // necesitamos y asigno inicial a ese char*, asì puede seguir la ejecuciòn del programa
  char inicial_debug[lengthC-lengthF+1];
  inicial = inicial_debug;
  
  strncpy(inicial, camino + 1, lengthC - lengthF);
  inicial[lengthC - lengthF] = '\0';
  if (inicial == NULL) {
    fprintf(stderr, "Error en directorios.c extraer_camino() --> Inicial\n");
    return -1;
  }

  printf("DEBUG - Inicial: %s | Final: %s | Tipo: %d\n", inicial, final, tipoInt);

  return tipoInt; //Devuelve 0 si es Fichero o 1 si es Directorio
}

// Busca la entrada indicada entre las entradas del inodo correspondiente a su directorio padre
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
  struct inodo inodo_dir;
  if (strcmp(camino_parcial,"/")==0) {
    *p_inodo = 0;  //la raiz siempre estara asociada al inodo 0
    *p_entrada = 0;
    fprintf(stderr, "buscar_entrada() | ERROR: Entrada es directorio raiz --> %d %s\n",errno, strerror(errno));
    return 0;
  }
  c_tipo = extraer_camino(camino_parcial, c_inicial, c_final, NULL);
  if (c_tipo == -1) {
    fprintf(stderr, "buscar_entrada() | ERROR_EXTRAER_CAMINO --> %d %s\n",errno, strerror(errno));
    return -1;
  }
  if (leer_inodo(*p_inodo_dir, &inodo_dir)==-1) {
    fprintf(stderr, "buscar_entrada() | ERROR_PERMISO_LECTURA --> %d %s\n",errno, strerror(errno));
    return -1;
  }
  if ((inodo_dir.permisos & 4) != 4) {
    fprintf(stderr, "buscar_entrada() | ERROR_PERMISO_LECTURA (permisos = 4) --> %d %s\n",errno, strerror(errno));
    return -1;
  }
  struct entrada entradas[max_entradas];
  unsigned int numentradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
  unsigned int nentrada = 0, offset = 0, buffSize = max_entradas * sizeof(struct entrada);

  if (numentradas > 0) {
    int res = mi_read_f(*p_inodo_dir, entradas, offset, buffSize);
    while ((res > 0) && (nentrada < numentradas) && !strcmp(c_inicial, entradas[nentrada].nombre)) {
      nentrada++;
      offset += buffSize;
      res = mi_read_f(*p_inodo_dir, entradas, offset, buffSize);
    }
  }
  if (nentrada == numentradas) {
    switch (reservar) {
      case 0:
      fprintf(stderr, "buscar_entrada (directorios.c) --> No existe la entrada a consultar\n");
      return -1;
      break;

      default:
      if (inodo_dir.tipo == 'f') {
        fprintf(stderr, "buscar_entrada (directorios.c) --> No es posible crear la entrada en un fichero\n");
        return -1;
      }
      else if ((inodo_dir.permisos & 2) != 2) {
        fprintf(stderr, "buscar_entrada (directorios.c) --> El inodo %d no tiene permisos de escritura\n", *p_inodo_dir);
        return -1;
      }
      else {
        strcpy(entradas[nentrada].nombre, c_inicial);  // AQUI HAY SEGFAULT!!!
        if (c_tipo == 'd') {
          if (strcmp(c_final, "/")) {
            entradas[nentrada].ninodo = reservar_inodo('d', 6);
          }
          else {
            fprintf(stderr, "buscar_entrada (directorios.c) --> No existe directorio intermedio\n");
            return -1;
          }
        }
        else {
          entradas[nentrada].ninodo = reservar_inodo('f', 6);
        }
        if (escribir_inodo(entradas[nentrada].ninodo, inodo_dir) == -1) {
          if (entradas[nentrada].ninodo != -1) {
            liberar_inodo(entradas[nentrada].ninodo);
          }
          return EXIT_FAILURE;
        }
      }
      break;
    }
  }

  if (strcmp(c_final, "/")) {
    if ((nentrada < numentradas) && (reservar == 1)) {
      fprintf(stderr, "buscar_entrada (directorios.c) --> La entrada ya existe\n");
      return -1;
    }

    *p_inodo = entradas[nentrada].ninodo;
    *p_entrada = entradas[nentrada].ninodo;
    return EXIT_SUCCESS;
  } else {
    *p_inodo_dir = entradas[nentrada].ninodo;
    return buscar_entrada(c_final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
  }
}

//////////////////NIVEL 9//////////////////

// Crea un fichero/directorio y su entrada de directorio
int mi_creat(const char *camino, unsigned char permisos){
  unsigned int p_inodo_dir, p_inodo, p_entrada;
  p_inodo_dir = 0; //Por simplicidad podemos suponer directamente que p_inodo_dir es 0

  int errores = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos);
  if(errores < 0){
    switch(errores){
      case -1:
      fprintf(stderr, "Error en directorios.c mi_creat() --> Tipo no adecuado\n");
      break;
      case -2:
      fprintf(stderr, "Error en directorios.c mi_creat() --> No se puede leer el inodo\n");
      break;
      case -3:
      fprintf(stderr, "Error en directorios.c mi_creat() --> El directorio donde apunta p_inodo_dir no tiene permisos de escritura\n");
      break;
      case -5:
      fprintf(stderr, "Error en directorios.c mi_creat() --> Ya existe\n");
      break;
    }
    return -1;
  }
  return 0;
}

// Pone el contenido del directorio especificado en un buffer y devuelve el nùmero de entradas
int mi_dir(const char *camino, char *buffer){
  unsigned int p_inodo_dir = 0;
  struct inodo inodo;
  unsigned int ninodo = 0;
  int errores = buscar_entrada(camino, &p_inodo_dir, NULL, NULL, 0, 0);
  if(errores < 0){
    switch(errores){
      case -1:
      fprintf(stderr, "Error en directorios.c mi_dir() --> Tipo no adecuado\n");
      break;
      case -2:
      fprintf(stderr, "Error en directorios.c mi_dir() --> No se puede leer el inodo\n");
      break;
      case -3:
      fprintf(stderr, "Error en directorios.c mi_dir() --> El directorio donde apunta p_inodo_dir no tiene permisos de escritura\n");
      break;
      case -5:
      fprintf(stderr, "Error en directorios.c mi_dir() --> Ya existe\n");
      break;
    }
    return -1;
  }
  if (leer_inodo(ninodo, &inodo) == -1) {
    fprintf(stderr, "Error en directorios.c mi_dir() --> No se ha podido leer el inodo %d\n", ninodo);
    return -1;
  }
  // Comprobamos errores
  if ((inodo.permisos & 2) != 2) {
    fprintf(stderr, "El fichero '%s' no tiene permisos de lectura.\n", camino);
    return -1;
  }
  if (inodo.tipo != 'd') {
    fprintf(stderr, "%s no es un directorio!\n", camino);
    return -1;
  }
  // Creamos la cadena de salida
  struct inodo inodoEntrada;
  struct tm *tm;
  unsigned int
  numEntradas = inodo.tamEnBytesLog / sizeof(struct entrada),
  tamEntrada = 100;
  char *separador = " | ",
  strPermisos[5], strTamEnBytesLog[10], strTime[50], strTipo[2], strNinodo[10],
  strEntrada[tamEntrada * numEntradas];
  struct entrada bufferDir[numEntradas];
  *strEntrada = '\0';
  errores = mi_read_f(ninodo, bufferDir, 0, inodo.tamEnBytesLog);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_dir()  --> Error en línea 310\n");
    return errores;
  }
  for (int i = 0; i < numEntradas; ++i) {
    if (leer_inodo(bufferDir[i].ninodo, &inodoEntrada) == -1) {
      fprintf(stderr, "Error en directorios.c mi_dir()  --> No se ha podido leer la entrada %d\n", bufferDir[i].ninodo);
      return -1;
    }
    *strPermisos = '\0';
    *strNinodo = '\0';
    strTipo[0] = inodoEntrada.tipo;
    strTipo[1] = '\0';
    strcat(strPermisos, strTipo);
    strcat(strPermisos, (inodoEntrada.permisos & 4) ? "r" : "-");
    strcat(strPermisos, (inodoEntrada.permisos & 2) ? "w" : "-");
    strcat(strPermisos, (inodoEntrada.permisos & 1) ? "x" : "-");
    sprintf(strNinodo, "%u", bufferDir[i].ninodo);
    sprintf(strTamEnBytesLog, "%u", inodoEntrada.tamEnBytesLog);
    tm = localtime(&inodoEntrada.mtime);
    sprintf(strTime, "[%d-%02d-%02d %02d:%02d:%02d]", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    strcat(strEntrada, strNinodo);
    strcat(strEntrada, separador);
    strcat(strEntrada, strPermisos);
    strcat(strEntrada, separador);
    strcat(strEntrada, strTamEnBytesLog);
    strcat(strEntrada, separador);
    strcat(strEntrada, strTime);
    strcat(strEntrada, separador);
    strcat(strEntrada, bufferDir[i].nombre);
    strcat(strEntrada, "\n");
  }
  strcpy(buffer, strEntrada);
  return 0;
}

// Funciòn que permite cambiar los permisos de un fichero o de un directorio
int mi_chmod(const char *camino, unsigned char permisos){
  unsigned int ninodo;
  int errores;
  errores = buscar_entrada(camino, &ninodo, NULL, NULL, 0, 0);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_chmod() --> No se ha encontrado la entrada %s\n", camino);
    return errores;
  }
  errores = mi_chmod_f(ninodo, permisos);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_chmod() --> No se han podido actualizar los permisos del inodo %d\n", ninodo);
    return errores;
  }
  return 0;
}

// Muestra la informaciòn acerca del inodo de un fichero o un directorio
int mi_stat(const char *camino, struct STAT *p_stat){
  unsigned int ninodo;
  int errores;
  errores = buscar_entrada(camino, &ninodo, NULL, NULL, 0, 0);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_stat() --> No se ha encontrado la entrada %s\n", camino);
    return errores;
  }
  errores = mi_stat_f(ninodo, p_stat);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_stat() --> No se ha podido obtener la información del inodo %d\n", ninodo);
    return errores;
  }
  return 0;
}

//////////////////NIVEL 10/////////////////
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
  return 0;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
  return 0;
}

//////////////////NIVEL 11/////////////////
int mi_link(const char *camino1, const char *camino2) {
  return 0;
}

int mi_unlink(const char *camino) {
  return 0;
}
