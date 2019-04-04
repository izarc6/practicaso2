///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////FICHEROS_BASICO.C/////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "ficheros_basico.h"

struct superbloque sprblc;
int tamMB(unsigned int nbloques){
/*
Hay que programar esta función para calcular el tamaño, en bloques, necesario para el mapa de bits.
*/
	int MBsize;
	//Utilizaremos el módulo para saber si necesitamos esa cantidad o un bloque adicional

	if(((nbloques/8)%BLOCKSIZE)==0){
		MBsize=(nbloques/8)/BLOCKSIZE;
	}else {
		MBsize=((nbloques/8)/BLOCKSIZE)+1;
	}
	return MBsize;
}

int tamAI(unsigned int ninodos){
	int AIsize;
	//Utilizaremos el módulo para saber si necesitamos esa cantidad o un bloque adicional
	if((ninodos%(BLOCKSIZE/INODOSIZE))==0){
		AIsize=ninodos/(BLOCKSIZE/INODOSIZE);
	}else {
		AIsize=(ninodos/(BLOCKSIZE/INODOSIZE))+1;
	}
	return AIsize;
}


int initSB(unsigned int nbloques, unsigned int ninodos){
/*vamos a definir una función que permita rellenar los datos básicos del superbloque.
*/
	struct superbloque SB;
/*Se trata de definir una zona de memoria (una variable de tipo SUPERBLOQUE) que se vaya rellenando con la información pertinente.
*/
	SB.posPrimerBloqueMB=posSB+tamSB;
	SB.posUltimoBloqueMB=SB.posPrimerBloqueMB+tamMB(nbloques)-1;
	SB.posPrimerBloqueAI=SB.posUltimoBloqueMB+1;
	SB.posUltimoBloqueAI=SB.posPrimerBloqueAI+tamAI(ninodos)-1;
	SB.posPrimerBloqueDatos=SB.posUltimoBloqueAI+1;
	SB.posUltimoBloqueDatos=nbloques-1;
	SB.posInodoRaiz=0;
	SB.posPrimerInodoLibre=0;
	SB.cantBloquesLibres=nbloques;
	SB.cantInodosLibres=ninodos;
	SB.totBloques=nbloques;
	SB.totInodos=ninodos;

/*Al finalizar, su contenido se escribe en el bloque correspondiente mediante la función bwrite().
*/
	if(bwrite(posSB,&SB)==-1) return -1;
	else return 1;
}

int initMB(unsigned int nbloques){
	struct superbloque SB;
	if (bread(posSB,&SB)==-1) return -1;
	//En esta etapa simplemente pondremos a 0 todos los bits del mapa de bits
	unsigned char T[BLOCKSIZE];//Del tamaño de un bloque.
	memset(T,0,BLOCKSIZE);//inicializaremos a 0.
/*El contenido del buffer se escribe en los bloques correspondientes mediante sucesivas llamadas a la
función bwrite (habrá que leer primeramente el superbloque para obtener la localización del mapa de bits).*/
	int i;
	for (i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB;i++){
		bwrite(i,T);//Des de la posicion del primer MB hasta la última.
	}
	for ( i = posSB; i <= SB.posUltimoBloqueAI; i++){
		escribir_bit(i,1);
		SB.cantBloquesLibres--;
	}
	bwrite(posSB, &SB);
	return 1;
}

int initAI(unsigned int ninodos){
	struct superbloque SB;
	if (bread(posSB,&SB)==-1) {printf("fallo\n"); return -1;}
	//definir una zona de memoria:
	struct inodo inodos[BLOCKSIZE/INODOSIZE];
	unsigned int contInodos = SB.posPrimerInodoLibre+1;
	int i;
	for(i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI;i++){
		int j;
		for(j=0; j<BLOCKSIZE/INODOSIZE; j++){//Hasta que j sea mayor que la cantidad de inodos en el bloque.
			inodos[j].tipo = 'l';  //libre
			//printf("eslibre\n");
			if(contInodos < SB.totInodos) {
				inodos[j].punterosDirectos[0]= contInodos;
				contInodos++;
			}else{
				inodos[j].punterosDirectos[0]= UINT_MAX;
				j = BLOCKSIZE/INODOSIZE;
			}
		}
		bwrite(i,&inodos);

	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////FASE 3////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int escribir_bit(unsigned int nbloque, unsigned int bit)
{
	unsigned char bufferMB [BLOCKSIZE];
	unsigned char mascara = 128; 		// 10000000
	struct superbloque sb;
	int posbyte, posbit, posbloqueMB;
	posbyte = nbloque/8;
	posbit = nbloque%8;
	if(bread(posSB, &sb) == -1)return -1;
	posbloqueMB = (posbyte/BLOCKSIZE) + sb.posPrimerBloqueMB;
	posbyte = posbyte%BLOCKSIZE;
	if(bread(posbloqueMB, bufferMB) == -1)return -1;
	if(bit==1){
										//Para poner un bit a 1:
		mascara >>= posbit;				// desplazamiento de bits a la derecha
		bufferMB[posbyte] |= mascara;	// operador OR para bits
	}else{								//bit==0
 		mascara >>= posbit; 			// desplazamiento de bits a la derecha
     	bufferMB[posbyte] &= ~mascara; 	// operadores AND y NOT para bits
     }
     if(bwrite(posbloqueMB, bufferMB) > -1){
     	return 0;
     } else{
     	return -1;
     }
 }

/*
Lee un determinado bit del mapa de bits.
Igual que la función anterior, pero en vez de escribir,
lee el bit correspondiente utilizando un desplazamiento a la derecha:
*/
 unsigned char leer_bit(unsigned int nbloque){
 	struct superbloque SB;
 	if(bread(posSB,&SB)==-1) return -1;

 	int posbyte = nbloque / 8;
 	int posbit = nbloque % 8;

 	unsigned char bufferMB [BLOCKSIZE];
 	int nblocMB = (posbyte / BLOCKSIZE) + SB.posPrimerBloqueMB;
 	posbyte %= BLOCKSIZE;
 	if(bread(nblocMB,bufferMB)==-1) return -1;
 	unsigned char mascara = 128;
 	posbyte = posbyte % BLOCKSIZE;
 	bread(nblocMB,bufferMB);
	mascara >>= posbit; // desplazamiento de bits a la derecha
	mascara &= bufferMB[posbyte]; // operador AND para bits
	mascara >>= (7-posbit); // desplazamiento de bits a la derecha
	return mascara;
}

/*
Encuentra el primer bloque libre (ver cálculos para el mapa de bits),
lo ocupa (con la ayuda de la función escribir_bit()) y devuelve su posición.
*/
int reservar_bloque(){
  struct superbloque sb;
  int valido = 1;
  int e = 0;
  int numbloque, bloqueMB;
  unsigned char buffer [BLOCKSIZE];
  unsigned char bufferAux [BLOCKSIZE];
  unsigned char mascara = 128;
  memset(bufferAux, 255, BLOCKSIZE);
  if(bread(posSB, &sb) == -1)return -1;
  if(sb.cantBloquesLibres > 0){   //Comprobamos la variable del superbloque que nos indica si quedan bloques libres.
    for(bloqueMB = sb.posPrimerBloqueMB; bloqueMB <= sb.posUltimoBloqueMB && e == 0 && valido > 0; bloqueMB++){
      valido = bread(bloqueMB, buffer);
      e = memcmp(buffer,bufferAux,BLOCKSIZE);
    }
    bloqueMB--;
    if(valido > 0){
      for(int posbyte = 0; posbyte < BLOCKSIZE; posbyte++){
       if(buffer[posbyte] < 255) {
         int posbit = 0;
         while(buffer[posbyte] & mascara){
           posbit++;
           buffer[posbyte] <<= 1;
         }
         numbloque = ((bloqueMB - sb.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
         escribir_bit(numbloque, 1);
         sb.cantBloquesLibres--;
         if(bwrite(posSB, &sb) == -1) return -1;
         memset(bufferAux, 0, BLOCKSIZE);
         if(bwrite(numbloque,bufferAux) == -1) return -1;
         return numbloque;
       }
      }
    } else return -1;
  }
  fprintf(stderr, "ERROR: No quedan bloques libres\n");
  return -1;
}

/*
Libera un bloque determinado (con la ayuda de la función escribir_bit()).
*/
int liberar_bloque(unsigned int nbloque){
	//Ponemos a 0 en el mapa de bits el bloque correspondiente (lo recibimos como
	//parámetro) y actualizamos la cantidad de bloques libres en el superbloque, pero no limpiamos
	//el bloque en la zona de datos; se queda basura pero se interpreta como espacio libre.
	struct superbloque SB;
	if(bread(posSB,&SB)==-1) return -1;
	int nblog = escribir_bit(nbloque, 0);
	if (nblog < 0) {	//si hi no s'ha pogut escriure
		return -1;
} else {
	SB.cantBloquesLibres++;
	int escrit = bwrite(posSB, &SB);
	if (escrit < 0) {
		return -1;
	} else {
			//Devolvemos el nº de bloque liberado.
		return nbloque;
	}
}
}


/*
Escribe el contenido de una variable de tipo struct inodo en un
determinado inodo del array de inodos.
*/
int escribir_inodo(unsigned int ninodo, struct inodo inodo){
	//Leemos el superbloque para obtener la localización del array de inodos
	struct superbloque SB;
	if(bread(posSB,&SB)==-1) return -1;

	//Obtenemos el nº de bloque del array de inodos que tiene el inodo solicitado
	int nbloc = ((ninodo * INODOSIZE) / BLOCKSIZE)+ SB.posPrimerBloqueAI;
	//Empleamos un array de inodos, del tamaño de la cabenantidad de inodos que caben en
	//un bloque (BLOCKSIZE/INODOSIZE), como buffer de lectura del bloque que hemos de leer
	struct inodo arrinodos [(BLOCKSIZE / INODOSIZE)]; 											//+ sb.posPrimerBloqueAI;??????????????????????
//memset(arrinodos, 0, BLOCKSIZE);
	int escritura = bread(nbloc, &arrinodos); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>ESTO ANTES ERA UN BWRITE
	if(escritura<0)return -1;
		//Una vez que tenemos el bloque en memoria escribimos el inodo en el lugar correspondiente
		//del array (ninodo%(BLOCKSIZE/INODOSIZE))
	arrinodos[(ninodo%(BLOCKSIZE/INODOSIZE))]= inodo;
		//Escribimos el bloque modificado en el dispositivo virtual utilizando la función bwrite()
	escritura = bwrite(nbloc, &arrinodos);
	return escritura;
}

/*
Lee un determinado inodo del array de inodos para volcarlo en una variable
de tipo struct inodo pasada por referencia.
*/
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
	//Leemos el superbloque para obtener la localización del array de inodos
	struct superbloque SB;
	if(bread(posSB,&SB)==-1) return -1;

	//Obtenemos el nº de bloque del array de inodos que tiene el inodo solicitado
	int nbloc = ((ninodo * INODOSIZE) / BLOCKSIZE) + SB.posPrimerBloqueAI;

	//Empleamos un array de inodos, del tamaño de la cantidad de inodos que caben en
	//un bloque (BLOCKSIZE/INODOSIZE), como buffer de lectura del bloque que hemos de leer
	struct inodo arrinodos[(BLOCKSIZE/INODOSIZE)];
//memset(arrinodos, 0, BLOCKSIZE);
	int lectura = bread(nbloc,arrinodos);
//memset(arrinodos, 0, BLOCKSIZE);
	//El inodo solicitado está en la posición ninodo%(BLOCKSIZE/INODOSIZE) del buffer
	*inodo = arrinodos[ninodo%(BLOCKSIZE/INODOSIZE)];
	if(lectura<0){
		return -1;
	}else{
		return 0;
	}
}
/*
Encuentra el primer inodo libre (información almacenada en el superbloque),
lo reserva (con la ayuda de la función escribir_inodo()), devuelve su número
 y actualiza la lista enlazada de inodos libres.
Si no hay inodos libres indicar error.
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos){
	struct superbloque SB;
	struct inodo inodo;
	int i, sig_inodo_libre;
	//leer superbloque
	if(bread(posSB, &SB) == -1){
		printf("ERROR: ficheros_basico: reservar_inodo: La lectura del superbloque ha fallado\n");
		return -1;
	}
	//comprobacion que hay inodos libres
	if(SB.cantInodosLibres == 0){
		printf("ERROR: ficheros_basico: reservar_inodo: No hay inodos libres\n");
		return -1;
	}
	//leemos primer inodo libre para coger donde apunta el siguiente libre
	int ninodo = SB.posPrimerInodoLibre;
	leer_inodo(ninodo,&inodo);
	sig_inodo_libre = inodo.punterosDirectos[0];
  //Modificar el contenido del inodo
	inodo.tipo = tipo;
	inodo.permisos = permisos;
	inodo.nlinks = 1;
	inodo.tamEnBytesLog = 0;
	inodo.atime = time(NULL);
	inodo.mtime = time(NULL);
	inodo.ctime = time(NULL);
	inodo.numBloquesOcupados = 0;
	for (i = 0; i < 12; i++) {
		inodo.punterosDirectos[i] = 0;
	}
	for (i = 0; i < 3; i++) {
		inodo.punterosIndirectos[i] = 0;
	}
	//Escribir el inodo en la posicion indicada
	if(escribir_inodo(ninodo, inodo) == -1){
		printf("ERROR: ficheros_basico: reservar_inodo: Error al escribir el inodo\n");
		return -1;
	}
	//Guardamos el siguiente inodo libre
	SB.posPrimerInodoLibre = sig_inodo_libre;
	//Decrementamos bloques libre de inodos
	SB.cantInodosLibres--;
	//Actualizamos el superbloque
	if(bwrite(posSB, &SB) == -1) {
		printf("ERROR: ficheros_basico: reservar_inodo: Error actualizando el superbloque\n");
		return -1;
	}
	return ninodo;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////FASE 4////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



int obtener_nrangoBL (struct inodo inodo,  int nblogico,int *ptr){
	int npunteros = BLOCKSIZE/sizeof(unsigned int);
	int directos = 12;
	int indirectos0 = npunteros + directos;
	int indirectos1 = npunteros*npunteros + indirectos0;
	int indirectos2 = npunteros*npunteros*npunteros + indirectos1;
	if (nblogico < directos) {
		*ptr = inodo.punterosDirectos[nblogico];
		return 0;
	} else if (nblogico < indirectos0) {
		*ptr = inodo.punterosIndirectos[0];
		return 1;
	} else if (nblogico < indirectos1) {
		*ptr = inodo.punterosIndirectos[1];
		return 2;
	} else if (nblogico < indirectos2) {
		*ptr = inodo.punterosIndirectos[2];
		return 3;
	} else {
		*ptr = 0;
		return -1;
	}
}

int obtener_indice ( int nblogico,  int nivel_punteros){
	int npunteros = BLOCKSIZE/sizeof(unsigned int);
	int directos = 12;
	int indirectos0 = npunteros + directos;
	int indirectos1 = npunteros*npunteros + indirectos0;
	int indirectos2 = npunteros*npunteros*npunteros + indirectos1;
	if (nblogico < directos) {
		return nblogico;
	} else if (nblogico < indirectos0) {
		return nblogico-directos;
	} else if (nblogico < indirectos1) {
		if (nivel_punteros == 2) {
			return ((nblogico-indirectos0)/(npunteros));
		} else if (nivel_punteros == 1) {
			return ((nblogico-indirectos0)%(npunteros));
		}
	} else if (nblogico < indirectos2) {
		if (nivel_punteros == 3) {
			return ((nblogico-indirectos1)/(npunteros*npunteros));
		} else if (nivel_punteros == 2) {
			return (((nblogico-indirectos1)%(npunteros*npunteros))/npunteros);
		} else if (nivel_punteros == 1) {
			return (((nblogico-indirectos1)%(npunteros*npunteros))%npunteros);
		}
	}
	return -1;
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar)
{
	struct inodo inodo;
	int ptr , ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, buffer[NPUNTEROS], indice;
	if(leer_inodo(ninodo, &inodo) == -1) return -1;
	ptr = 0; ptr_ant = 0; salvar_inodo = 0;
	nRangoBL = obtener_nrangoBL(inodo, nblogico, &ptr); //0:D, 1:I0, 2:I1, 3:I2
	nivel_punteros = nRangoBL;
    while(nivel_punteros > 0) //iterar para cada nivel de indirectos
    {
    	if(ptr == 0) //no cuelgan bloques de punteros
    	{
        	if(reservar == 0) return -1; //error lectura bloque inexistente
    		else //construir bloques intermedios y enlaces desde inodo hasta datos
    		{
    			salvar_inodo = 1;
            	if((ptr = reservar_bloque()) == -1) return -1; //de punteros
            	inodo.numBloquesOcupados++;
            	inodo.ctime = time(NULL);
            	if(nivel_punteros == nRangoBL) //el bloque cuelga directamente del inodo
            	{
            		inodo.punterosIndirectos[nRangoBL-1] = ptr;
            	} else  //el bloque cuelga de otro bloque de punteros
            	{
            		buffer[indice] = ptr;
            		if(bwrite(ptr_ant, buffer) == -1) return -1;
            	}
            }
        }
        if(bread(ptr, buffer) == -1) return -1;
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
   	}   //al salir de este bucle ya estamos al nivel de datos

   	if(ptr == 0) //no existe bloque de datos
   	{
      	if(reservar == 0) return -1;  //error lectura ∄ bloque
      	else
      	{
      		salvar_inodo = 1;
         	if((ptr = reservar_bloque()) == -1) return -1; //de datos
         	inodo.numBloquesOcupados++;
         	inodo.ctime = time(NULL);
         	if(nRangoBL == 0)
         	{
         		inodo.punterosDirectos[nblogico] = ptr;
         	}
         	else
         	{
         		buffer[indice] = ptr;
         		if(bwrite(ptr_ant, buffer) == -1) return -1;
         	}
         }
     }

     if(salvar_inodo == 1)
     {
     	if(escribir_inodo(ninodo, inodo) == -1) return -1;
      //sólo si lo hemos actualizado
     }

    return ptr; //nbfisico
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////FASE 5////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


int liberar_inodo(unsigned int ninodo){
// Llamar a la función auxiliar liberar_bloques_inodo() para liberar todos
//los bloques del inodo, explicada a continuación. El argumento nblogico valdrá 0.
	liberar_bloques_inodo(ninodo,0);
// Leer el inodo actualizado
	struct inodo ino;
	leer_inodo(ninodo, &ino);
// Marcar el inodo como tipo libre
	ino.tipo = 'l';
// Leer el superbloque
	struct superbloque SB;
	if(bread(posSB,&SB)==-1) return -1;
// Incluir el inodo en la lista de inodos libres (por el principio),actualizando
//el superbloque para que la variable del 1er inodo libre lo apunte
	ino.punterosDirectos[0] = SB.posPrimerInodoLibre;
	SB.posPrimerInodoLibre = ninodo;
// Incrementar la cantidad de inodos libres en el superbloque
// (la cantidad de bloques libres en el superbloque ya queda decrementada en la función liberar_bloque())
	SB.cantInodosLibres++;
// Escribir el inodo
	escribir_inodo(ninodo, ino);
// Escribir el superbloque
	if(bwrite(posSB,&SB)==-1) return -1;
// Devolver el nº del inodo liberado
	return ninodo;
}

int liberar_bloques_inodo(unsigned int ninodo, unsigned int nblogico)
{
// Versión iterativa de liberar_bloques_inodo()
	struct inodo inodo;
	int nRangoBL, nivel_punteros, indice, ptr, nblog, ultimoBL;
	unsigned char bufferAux[BLOCKSIZE];
	int bloques_punteros [3] [NPUNTEROS];
    int ptr_nivel [3];   //punteros auxiliares a bloques
    int indices[3];    //indices auxiliares
    int liberados;
    //fprintf(stderr, "primer BL: %d, ", nblogico);
    memset (bufferAux, 0, BLOCKSIZE);
    liberados = 0;
    leer_inodo(ninodo, &inodo);
    if(inodo.tamEnBytesLog%BLOCKSIZE == 0)
    {
    	ultimoBL=inodo.tamEnBytesLog/BLOCKSIZE - 1;
    } else
    {
    	ultimoBL = inodo.tamEnBytesLog/BLOCKSIZE;
    }
  	//fprintf(stderr, "último BL: %d\n", ultimoBL);
    ptr = 0;
    for(nblog = nblogico; nblog <= ultimoBL; nblog++)
    {
    	nRangoBL = obtener_nrangoBL(inodo, nblog, &ptr);
    	nivel_punteros = nRangoBL;
    	while (ptr>0 && nivel_punteros>0)
    	{
    		bread (ptr, bloques_punteros[nivel_punteros - 1]);
    		indice = obtener_indice(nblog, nivel_punteros);
    		ptr_nivel[nivel_punteros - 1] = ptr;
    		indices[nivel_punteros - 1] = indice;
    		ptr = bloques_punteros[nivel_punteros - 1][indice];
    		nivel_punteros--;
    	}
	    if(ptr > 0) //existe bloque de datos
	    {
	    	liberar_bloque(ptr);
	    	//fprintf(stderr, "liberado bloque de datos %d para BL %d\n", ptr, nblog);
	    	liberados++;
	      	if(nRangoBL == 0) // puntero Directo
	      	{
	      		inodo.punterosDirectos[nblog] = 0;
	      	} else
	      	{
	      		while(nivel_punteros < nRangoBL)
	      		{
	      			indice = indices[nivel_punteros];
	      			bloques_punteros[nivel_punteros][indice] = 0;
	      			ptr = ptr_nivel [nivel_punteros];
		          	if(memcmp (bloques_punteros[nivel_punteros], bufferAux, BLOCKSIZE) == 0) //No cuelgan bloques ocupados, hay que liberar el bloque de punteros
		          	{
		          		liberar_bloque(ptr);
			            //fprintf(stderr, "liberado bloque de punteros %d para BL %d\n", ptr, nblog);
		          		liberados++;
		          		nivel_punteros++;
		          		if (nivel_punteros == nRangoBL) inodo.punterosIndirectos[nRangoBL-1] = 0;
		          	} else //escribimos en el dispositivo el bloque de punteros modificado
		          	{
		          		bwrite(ptr, bloques_punteros[nivel_punteros]);
		            	nivel_punteros = nRangoBL; // para salir del bucle
		            }
		        }
		    }
		}
	}
	if(escribir_inodo(ninodo, inodo) == -1) return -1;
	return liberados;
}
