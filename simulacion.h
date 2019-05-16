#include <signal.h>
#include <sys/wait.h>
#include "directorios.h"

struct registro {
	time_t date;
	unsigned int pid;
	unsigned int nEscritura;
	unsigned int posicion;
};
