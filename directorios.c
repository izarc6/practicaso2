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
    //printf("DEBUG - buscar_entrada() | p_inodo es null! Set a 0\n");
    unsigned int placeholder = 0;
    p_inodo = &placeholder;
  }
  if (p_entrada == NULL) {
    //printf("DEBUG - buscar_entrada() | p_entrada es null! Set a 0\n");
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

  //printf("DEBUG - buscar_entrada() | numentrades: %d | nentrada: %d\n",numentrades,nentrada);

  if (numentrades > 0){
    //printf("DEBUG - buscar_entrada() | Permisos del inodo %d: %c\n",*p_inodo_dir, in.permisos);
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
    return -1;
  }
  return 0;
}

// Pone el contenido del directorio especificado en un buffer y devuelve el nùmero de entradas
int mi_dir(const char *camino, char *buffer){
  unsigned int p_inodo_dir = 0;
  struct inodo inodo;
  unsigned int ninodo = 0;
  unsigned int p_inodo, inicial = 0;
  int errores = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &inicial, 0, '4');
  if(errores < 0){
    return -1;
  }
  if (leer_inodo(p_inodo, &inodo) == -1) {
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
  unsigned int numEntradas = inodo.tamEnBytesLog / sizeof(struct entrada),
  tamEntrada = 100;
  char *separador = "\t| ",
  strPermisos[5], strTamEnBytesLog[10], strTime[50], strTipo[2], strNinodo[10],
  strEntrada[tamEntrada * numEntradas];
  struct entrada bufferDir[numEntradas];
  *strEntrada = '\0';
  errores = leer_inodo(p_inodo, &inodoEntrada);
  if (errores < 0) {
    return errores;
  }
  for (int i = 0; i < numEntradas; i++) {
    if (mi_read_f(p_inodo,&bufferDir[i],i*sizeof(struct entrada),sizeof(struct entrada)) < 0) {
      fprintf(stderr, "Error en directorios.c mi_dir()  --> No se ha podido leer la entrada %d\n", bufferDir[i].ninodo);
      return -1;
    }
    if (leer_inodo(bufferDir[i].ninodo,&inodoEntrada) < 0) {
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
  unsigned int p_inodo, inicial;
  //errores = buscar_entrada(camino, &ninodo, &debug1, &debug2, 0, 0);
  errores = buscar_entrada(camino, &ninodo, &p_inodo, &inicial, 0, permisos);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_chmod() --> Ha ocurrido un error por la entrada %s\n", camino);
    return errores;
  }
  errores = mi_chmod_f(p_inodo, permisos);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_chmod() --> No se han podido actualizar los permisos del inodo %d\n", ninodo);
    return errores;
  }
  return 0;
}

// Muestra la informaciòn acerca del inodo de un fichero o un directorio
int mi_stat(const char *camino, struct STAT *p_stat){
  unsigned int ninodo;
  unsigned int p_inodo, inicial;
  int errores = buscar_entrada(camino, &ninodo, &p_inodo, &inicial, 0, '4');
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_stat() --> No se ha encontrado la entrada %s\n", camino);
    return errores;
  }
  errores = mi_stat_f(p_inodo, p_stat);
  if (errores < 0) {
    fprintf(stderr, "Error en directorios.c mi_stat() --> No se ha podido obtener la información del inodo %d\n", ninodo);
    return errores;
  }
  return 0;
}

//////////////////NIVEL 10/////////////////
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
	unsigned int p_inodo_dir, p_inodo, p_entrada = 0;
	if(strcmp (camino, ultimaEntradaLeida.camino) == 0) {
		p_inodo = ultimaEntradaLeida.p_inodo;
	} else {
		if(buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, '4') < 0) return -1;
		strcpy(ultimaEntradaLeida.camino, camino);
		ultimaEntradaLeida.p_inodo = p_inodo;
	}
	return mi_read_f(p_inodo, buf, offset, nbytes);
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
	unsigned int p_inodo_dir, p_inodo, p_entrada;
	p_inodo_dir = 0;
	if(strcmp (camino, ultimaEntradaLeida.camino) == 0) {
		p_inodo = ultimaEntradaLeida.p_inodo;
	} else {
		if(buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, '6') < 0) return -1;
		strcpy(ultimaEntradaLeida.camino, camino);
		ultimaEntradaLeida.p_inodo = p_inodo;
	}
	int bytesLeidos;
	if((bytesLeidos = mi_write_f(p_inodo, buf, offset, nbytes)) < 0){
		return -1;
	}
	return bytesLeidos;
}

//////////////////NIVEL 11/////////////////
int mi_link(const char *camino1, const char *camino2){
	unsigned int p_inodo_dir, p_inodo, p_entrada = 0;
	struct entrada entrada;
	struct inodo inodo;
	if(buscar_entrada(camino1,&p_inodo_dir,&p_inodo,&p_entrada,0,'0') < 0){
		return -1;
	}
	if(leer_inodo(p_inodo, &inodo) == -1) return -1;
	if(inodo.tipo != 'f' && (inodo.permisos & 4) != 4) return -1;
	p_inodo_dir = 0;
	if(buscar_entrada(camino2,&p_inodo_dir,&p_inodo,&p_entrada,1,'6') < 0) return -1;
	if(mi_read_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))==-1) return -1;
	liberar_inodo(entrada.ninodo);
	entrada.ninodo = p_inodo;
	if(mi_write_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))==-1) return -1;
	if(leer_inodo(p_inodo, &inodo) == -1) return -1;
	inodo.nlinks++;
	inodo.ctime = time(NULL);
	escribir_inodo(p_inodo, inodo);
	return 0;
}

int mi_unlink(const char *camino){
	unsigned int p_inodo_dir, p_inodo, p_entrada = 0;
	struct entrada entrada;
	int nentradas;
	struct inodo inodo;
	if(buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,'6') < 0) return -1;
	if(leer_inodo(p_inodo, &inodo) == -1) return -1;
	if(inodo.tipo == 'd' && (inodo.tamEnBytesLog != 0)) return -1;
	if(leer_inodo(p_inodo_dir, &inodo) == -1) return -1;
	nentradas = inodo.tamEnBytesLog/sizeof(struct entrada);
	if(p_entrada != nentradas - 1){
		mi_read_f(p_inodo_dir, &entrada, (nentradas - 1) * sizeof(struct entrada), sizeof(struct entrada));
		mi_write_f(p_inodo_dir, &entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada));
	}
	mi_truncar_f(p_inodo_dir, (nentradas - 1) * sizeof(struct entrada));
	if(leer_inodo(p_inodo, &inodo) == -1) return -1;
	inodo.nlinks--;
	if(inodo.nlinks == 0) {
		liberar_inodo(p_inodo);
	} else{
		inodo.ctime = time(NULL);
		escribir_inodo(p_inodo, inodo);
	}
	return 0;
}
