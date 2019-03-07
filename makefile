CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread

SOURCES=bloques.c mi_mkfs.c ficheros_basico.c leer_sf.c #todos los .c
LIBRARIES=bloques.o ficheros_basico.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h #todos los .h
PROGRAMS=mi_mkfs leer_sf
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

mi_mkfs: mi_mkfs.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

leer_sf: leer_sf.o
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
