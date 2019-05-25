//////////////////Includes//////////////////
#include <stdio.h>      // printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h>      // O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h>   // S_IRUSR, S_IWUSR
#include <stdlib.h>     // exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h>     // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>      // errno
#include <string.h>     // strerror()
#include <time.h>       // time_t
#include "semaforo_mutex_posix.h"

//////////////////Defines//////////////////
#define BLOCKSIZE 1024  // bytes de tamaño de bloque logico

//////////////////Funciones//////////////////
void mi_waitSem();
void mi_signalSem();
int bmount(const char *camino);
int bumount();
int bwrite(unsigned int nbloque, const void *buf);
int bread(unsigned int nbloque, void *buf);
