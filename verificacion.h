#include "simulacion.h"

#define NENTRADAS 100
#define NUMREGISTROS 200

struct INFORMACION {
    int pid;
    unsigned int nEscrituras; // validadas
    struct REGISTRO PrimeraEscritura;
    struct REGISTRO UltimaEscritura;
    struct REGISTRO MenorPosicion;
    struct REGISTRO MayorPosicion;
};
