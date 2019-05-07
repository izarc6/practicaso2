CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread

SOURCES=bloques.c mi_mkfs.c mi_mkdir.c mi_chmod.c mi_ls.c mi_stat.c ficheros_basico.c leer_sf.c ficheros.c escribir.c leer.c permitir.c truncar.c directorios.c #todos los .c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o directorios.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h ficheros.h directorios.h #todos los .h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar mi_mkdir mi_chmod mi_ls mi_stat
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

mi_mkfs: mi_mkfs.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

leer_sf: leer_sf.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

escribir: escribir.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

leer: leer.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

permitir: permitir.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

truncar: truncar.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

mi_chmod: mi_chmod.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

mi_ls: mi_ls.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

mi_stat: mi_stat.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

mi_mkdir: mi_mkdir.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
