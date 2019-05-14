#include "directorios.h"

struct UltimaEntrada ultimaEntradaLeida;

// Dado un camino que empieza con '/', separa lo que està contenido entre los primeros dos '/' en inicial y lo demàs en final
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
  int i = 0;
  if (camino[0] != '/') {
    fprintf(stderr, "Error en directorios.c extraer_camino() --> El camino debe empezar con '/'\n");
    return -1;
  }
  const char *sig_camino = strchr(camino + 1,'/');
  memset(inicial,0,strlen(inicial));
  if(sig_camino==NULL){
    strcpy(inicial, camino+1);
    *final = '\0';
    *tipo = 'f'; //Es un file
  } else {
    i = sig_camino - camino - 1;
    strncpy(inicial, camino + 1,i);
    strcpy(final, sig_camino);
    *tipo = 'd'; //Es un directorio
  }
  return 0;
}

// Busca la entrada indicada entre las entradas del inodo correspondiente a su directorio padre
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
  if (strcmp(camino_parcial, "/") == 0) {
    *p_inodo = 0;   // Inodo raìz
    *p_entrada =0;
    return 0;
  }

  if (p_inodo == NULL) {
    printf("DEBUG - buscar_entrada() | p_inodo es null! Set a 0\n");
    unsigned int placeholder = 0;
    p_inodo = &placeholder;
  }
  if (p_entrada == NULL) {
    printf("DEBUG - buscar_entrada() | p_entrada es null! Set a 0\n");
    unsigned int placeholder = 0;
    p_entrada = &placeholder;
  }
  

  char inicial[MAX_CHAR];
  char final[strlen(camino_parcial)];
  memset(inicial,0,MAX_CHAR);
  memset(final,0,strlen(camino_parcial));
  char tipo;
  if (extraer_camino(camino_parcial, inicial, final,&tipo) == -1) {
    fprintf(stderr, "Error en directorios.c buscar_entrada() --> Error en extraer camino\n");
    return -1;
  }
  struct inodo in;

  if (leer_inodo(*p_inodo_dir,&in) == -1) {
      fprintf(stderr, "Error en directorios.c buscar_entrada() --> Ha ocurrido un error leyendo el inodo\n");
      return -1;
  }
  //printf("DEBUG - buscar_entrada() | Desp. de lectura inodo | *p_inodo: %d | *p_entrada: %d\n",*p_inodo,*p_entrada);

  char buffer[in.tamEnBytesLog];
  struct entrada *entrada;
  entrada = malloc(sizeof(struct entrada));
  entrada->nombre[0] = '\0';
  int numentrades = in.tamEnBytesLog / sizeof(struct entrada);
  int nentrada = 0;

  printf("DEBUG - buscar_entrada() | numentrades: %d | nentrada: %d\n",numentrades,nentrada);

  if (numentrades > 0){
    printf("DEBUG - buscar_entrada() | Permisos del inodo %d: %c\n",*p_inodo_dir, in.permisos);
    if ((in.permisos & 4) != 4) {
      fprintf(stderr, "Error en directorios.c buscar_entrada() --> No tiene permisos de lectura\n");
      return -1;
    }
    int offset = 0; int encontrado = 1;
    while(nentrada < numentrades && encontrado != 0) {
      mi_read_f(*p_inodo_dir, buffer, nentrada * sizeof(struct entrada), sizeof(buffer)); //leer siguiente entrada
      memcpy(entrada, buffer, sizeof(struct entrada));

      //printf("DEBUG - buscar_entrada() | inicial: %s | entrada->nombre: %s\n",inicial,entrada->nombre);

      encontrado = strcmp(inicial, entrada->nombre);
      //printf("DEBUG - buscar_entrada() | strcmp en while 1 OK\n");
      while(offset < numentrades && nentrada < numentrades && encontrado != 0) {
        //printf("DEBUG - buscar_entrada() | EN WHILE 2\n");
        nentrada++;
        offset++;
        memcpy(entrada, offset * sizeof(struct entrada) + buffer, sizeof(struct entrada));
        //printf("DEBUG - buscar_entrada() | EN WHILE 2 | inicial: %s | entrada->nombre: %s\n",inicial,entrada->nombre);
        encontrado = strcmp(inicial, entrada->nombre);
        //printf("DEBUG - buscar_entrada() | strcmp en while 2 OK\n");
      }
      offset = 0;
    }
  }
  if (nentrada == numentrades){
    //printf("DEBUG - buscar_entrada() | En if 2\n");
    switch(reservar){
      case 0:
      fprintf(stderr, "Error en directorios.c buscar_entrada() --> No existe entrada consulta\n");
      return -1;
      case 1:
      if (in.tipo=='f') {
        return -1;
      }
      strcpy(entrada->nombre, inicial);
      if (tipo == 'd'){
        if (strcmp(final,"/")==0) {
          entrada->ninodo = reservar_inodo('d', permisos);
        } else {
          fprintf(stderr, "Error en directorios.c buscar_entrada() --> No existe directorio intermedio\n");
          return -1;
        }
      } else {
        entrada->ninodo = reservar_inodo('f', permisos);
      }
      if (mi_write_f(*p_inodo_dir, entrada, nentrada * sizeof(struct entrada), sizeof(struct entrada)) < 0) {
        if (entrada->ninodo != -1) {
          liberar_inodo(entrada->ninodo);
        }
        printf("4\n");
        fprintf(stderr, "Error en directorios.c buscar_entrada() --> No tiene permisos de escritura\n");
        return -1;
      }
    }
  }
  //printf("DEBUG - buscar_entrada() | Fuera de if 2\n");
  if ((strcmp(final,"/")==0) || strcmp(final, "\0") == 0){
    //printf("DEBUG - buscar_entrada() | strcmp final OK | final: %s\n",final);
    if ((nentrada < numentrades) && (reservar == 1)) {
      fprintf(stderr, "Error en directorios.c buscar_entrada() --> Entrada ya existente\n");
      return -1;
    }
    //printf("DEBUG - buscar_entrada() | *p_inodo: %d | *p_entrada: %d\n",*p_inodo,*p_entrada);
    *p_inodo = entrada->ninodo;
    *p_entrada = nentrada;
    //printf("DEBUG - buscar_entrada() | Antes de return 0\n");
    return 0;
  } else {
    *p_inodo_dir = entrada->ninodo;
    return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
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
  unsigned int debug1, debug2 = 0;
  int errores = buscar_entrada(camino, &p_inodo_dir, &debug1, &debug2, 0, 0);
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
  unsigned int ninodo = 0;
  int errores;
  //errores = buscar_entrada(camino, &ninodo, &debug1, &debug2, 0, 0);
  errores = buscar_entrada(camino, &ninodo, NULL, NULL, 0, 0);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_chmod() --> Ha ocurrido un error por la entrada %s\n", camino);
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
  int errores = buscar_entrada(camino, &ninodo, NULL, NULL, 0, 0);
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
  int errores;
  unsigned int p_inodo, bytesLeidos = 0;
  if (strcmp(camino, ultimaEntradaLeida.camino) == 0) {
    p_inodo = ultimaEntradaLeida.p_inodo;
  } else {
    errores = buscar_entrada(camino, &p_inodo, NULL, NULL, 0, 0);
    if (errores < 0) {
      fprintf(stderr, "Error en directorios.c mi_read() --> No se ha encontrado la entrada %s\n", camino);
      return -1;
    }
  }
  bytesLeidos = mi_read_f(p_inodo, buf, offset, nbytes);
  if (bytesLeidos <= 0) {
    fprintf(stderr, "Error en directorios.c mi_read() --> No se ha podido leer el inodo %d\n", p_inodo);
    return -1;
  }
  return bytesLeidos;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
  int errores;
  unsigned int p_inodo, bytesEscritos = 0;
  if (strcmp(camino, ultimaEntradaLeida.camino) == 0) {
    p_inodo = ultimaEntradaLeida.p_inodo;
  } else {
    errores = buscar_entrada(camino, &p_inodo, NULL, NULL, 0, 0);
    if (errores < 0) {
      fprintf(stderr, "Error en directorios.c mi_write() --> No se ha encontrado la entrada %s\n", camino);
      return -1;
    }
  }
  bytesEscritos = mi_write_f(p_inodo, buf, offset, nbytes);
  if (bytesEscritos <= 0) {
    fprintf(stderr, "Error en directorios.c mi_write() --> No se ha podido leer el inodo %d\n", p_inodo);
    return -1;
  }
  return bytesEscritos;
}

//////////////////NIVEL 11/////////////////
int mi_link(const char *camino1, const char *camino2) {
  struct inodo inodo_f, inodo_d;
  struct entrada nuevaEntrada;
  int errores;
  unsigned int ninodo_f, ninodo_link, ninodo_d, nentrada, offsetNuevaEntrada;
  errores = buscar_entrada(camino1, &ninodo_f, NULL, NULL, 0, 0);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_link() --> No existe la entrada %s\n", camino1);
    return errores;
  }
  if (leer_inodo(ninodo_f, &inodo_f) != 0) {
    fprintf(stderr, "Error en directorios.c mi_link() --> Error al leer el inodo de fichero %d\n", ninodo_f);
    return -1;
  }
  if ((inodo_f.permisos & 4) != 4) {
    fprintf(stderr, "Error en directorios.c mi_link() --> El inodo %d no tiene permisos de lectura\n", ninodo_f);
    return -1;
  }
  if (inodo_f.tipo != 'f') {
    fprintf(stderr, "Error en directorios.c mi_link() --> %s no es un fichero\n", camino1);
    return -1;
  }
  errores = buscar_entrada(camino2, &ninodo_link, &ninodo_d, &nentrada, 1, 7);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_link() --> No existe la entrada %s\n", camino2);
    return errores;
  }
  liberar_inodo(ninodo_link);
  if (leer_inodo(ninodo_d, &inodo_d) != 0) {
    fprintf(stderr, "Error en directorios.c mi_link() --> Error al leer el inodo de directorio %d\n", ninodo_d);
    return -1;
  }
  offsetNuevaEntrada = nentrada * sizeof(struct entrada);
  errores = mi_read_f(ninodo_d, &nuevaEntrada, offsetNuevaEntrada, sizeof(struct entrada));
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_link() --> No se ha podido leer el inodo %d\n", ninodo_d);
    return errores;
  }
  nuevaEntrada.ninodo = ninodo_f;
  if (leer_inodo(ninodo_f, &inodo_f) != 0) {
    fprintf(stderr, "Error en directorios.c mi_link() --> Error al leer el inodo de fichero %d\n", ninodo_f);
    return -1;
  }
  inodo_f.nlinks++;
  inodo_f.ctime = time(NULL);
  if (escribir_inodo(ninodo_f, inodo_f) != 0) {
    fprintf(stderr, "Error en directorios.c mi_link() --> Error al escribir en el inodo de fichero %d\n", ninodo_f);
    return -1;
  }
  return 0;
}

int mi_unlink(const char *camino) {
  struct inodo inodo_f, inodo_d;
  unsigned int ninodo_f, ninodo_d, nentrada, nentradas_d;
  int errores = buscar_entrada(camino, &ninodo_d, &ninodo_f, &nentrada, 0, 0);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_unlink() --> No se ha podido encontrar la entrada %s\n", camino);
    return errores;
  }
  if (leer_inodo(ninodo_f, &inodo_f) != 0) {
    fprintf(stderr, "Error en directorios.c mi_unlink() --> No se ha podido leer el inodo de fichero %d\n", ninodo_f);
    return -1;
  }
  if (leer_inodo(ninodo_d, &inodo_d) != 0) {
    fprintf(stderr, "Error en directorios.c mi_unlink() --> No se ha podido leer el inodo de directorio %d\n", ninodo_d);
    return -1;
  }
  if (inodo_f.tipo == 'd' && inodo_f.tamEnBytesLog > 0) {
    fprintf(stderr, "Error en directorios.c mi_unlink() --> No es posible borrar la entrada %s ya que es un directorio no vacío\n", camino);
    return -1;
  }
  nentradas_d = inodo_d.tamEnBytesLog / sizeof(struct entrada);
  if (nentrada == nentradas_d - 1) {
    struct entrada ultEntrada;
    unsigned int entradaBorrar = nentrada * sizeof(struct entrada), ultimaEntrada = inodo_d.tamEnBytesLog - sizeof(struct entrada);
    errores = mi_read_f(ninodo_d, &ultEntrada, ultimaEntrada, sizeof(struct entrada));
    if (errores < 0) {
      fprintf(stderr, "Error en directorios.c mi_unlink() --> No se ha podido leer el inodo %d\n", ninodo_d);
      return errores;
    }
    errores = mi_write_f(ninodo_d, &ultEntrada, entradaBorrar, sizeof(struct entrada));
    if (errores < 0) {
      fprintf(stderr, "Error en directorios.c mi_unlink() --> No se ha podido escribir en el inodo %d\n", ninodo_d);
      return errores;
    }
    errores = mi_truncar_f(ninodo_d, (nentradas_d - 1) * sizeof(struct entrada));
    if (errores < 0) {
      fprintf(stderr, "Error en directorios.c mi_unlink() --> No se ha podido truncar el inodo %d\n", ninodo_d);
      return errores;
    }
    inodo_f.nlinks--;
    if (inodo_f.nlinks == 0) {
      liberar_inodo(ninodo_f);
    } else {
      inodo_f.ctime = time(NULL);
      if (escribir_inodo(ninodo_f, inodo_f) != 0) {
        fprintf(stderr, "Error en directorios.c mi_unlink() --> No se ha podido escribir en el inodo %d\n", ninodo_f);
        return -1;
      }
    }
  }
  return 0;
}
