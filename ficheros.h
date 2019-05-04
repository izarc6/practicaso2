//////////////////Includes//////////////////
#include "ficheros_basico.h"

//////////////////Structs//////////////////
struct STAT {
	unsigned char tipo;
	unsigned char permisos;
	time_t atime;
	time_t mtime;
	time_t ctime;
	unsigned int nlinks;
	unsigned int tamEnBytesLog;
	unsigned int numBloquesOcupados;
	unsigned int punterosDirectos[12];
	unsigned int punterosIndirectos[3];
};

//////////////////Funciones//////////////////
//Nivel 6
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
