CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
LDFLAGS=-pthread

SOURCES=bloques.c ficheros_basico.c leer_sf.c mi_mkfs.c ficheros.c escribir.c leer.c permitir.c truncar.c directorios.c mi_mkdir.c mi_touch.c mi_ls.c mi_chmod.c mi_stat.c mi_cat.c mi_escribir.c mi_link.c mi_rm.c semaforo_mutex_posix.c simulacion.c verificacion.c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o directorios.o semaforo_mutex_posix.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h ficheros.h directorios.h semaforo_mutex_posix.h simulacion.h verificacion.h
PROGRAMS=leer_sf mi_mkfs escribir leer permitir truncar mi_mkdir mi_touch mi_ls mi_chmod mi_stat mi_cat mi_escribir mi_link mi_rm simulacion verificacion

OBJS=$(SOURCES:.c=.o)

all:$(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o:%.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	rm -rf *.o *~ $(PROGRAMS)
	rm -rf disco
	rm -rf ext*
	rm -rf resultados.txt
