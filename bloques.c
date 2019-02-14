#include "bloques.h"
static int descriptor = 0;

int bmount(const char *camino)
{
    int fd = open(camino, O_RDWR | O_CREAT, 666);
    if (fd == -1)
    {
        fprintf(stderr, "ERROR fallo lect . \n");
    }
    descriptor = fd;

    return fd;
}

int bumount()
{ //liberar descriptor de ficheros
    int fc = close(descriptor);
    if (fc == -1)
    {
        //Error
        fprintf(stderr, "ERROR: fallo al cerrar . \n");
        return -1;
    }
    else
    {
        return 0;
    }
}

int bwrite(unsigned int nbloque, const void *buf)
{ //escriure un bloc, el indicat per nbloque, s'sescriu *buf
    //bwrite retorna el nº de bytes que s'ha escrit o -1 si error

    off_t desplazamiento = lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);
    if (desplazamiento == -1)
    {
        //Error
        fprintf(stderr, "ERROR: fallo al buscar bloque . \n");
        return -1;
    }
    else
    {
        int bescritos = write(descriptor, buf, BLOCKSIZE);
        if (bescritos == -1)
        {
            fprintf(stderr, "ERROR: fallo al escribir el bloque . \n");
        }
        return bescritos;
    }
}

int bread(unsigned int nbloque, void *buf)
{

    off_t desplazamiento = lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);
    if (desplazamiento == -1)
    {
        //Error
        fprintf(stderr, "ERROR: fallo al buscar bloque . \n");
        return -1;
    }
    else
    {
        int bleidos = read(descriptor, buf, BLOCKSIZE);
        if (bleidos == -1)
        {
            fprintf(stderr, "ERROR: fallo al leer el bloque . \n");
        }
        return bleidos;
    }
}
