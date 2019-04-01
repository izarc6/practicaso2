//Le pasaremos por línea de comandos un nº de inodo obtenido con el programa
//anterior (además del nombre del dispositivo). Su funcionamiento tiene que ser
// similar a la función cat de linux, explorando TODO el fichero


int main(int argc, char **argv) {
    bmount(argv[1]);
    int ninodo = atoi(argv[2]);
    unsigned char buffer[BLOCKSIZE];
    memset(buffer, 0, BLOCKSIZE);

    int i = 0;
    int lectura = 0;
    printf("Leyendo archivo\n");

    while ((lectura = mi_read_f(ninodo, buffer, i * BLOCKSIZE, BLOCKSIZE)) > 0) {
        printf("%s", buffer);
        memset(buffer,0,BLOCKSIZE);
        i++;
    }
    printf("\n");

    if (lectura == -1) {
        fprintf(stderr, "Error en leer.c nombre fichero --> %d: %s\n", errno, strerror(errno));
    }

    bumount();
}
