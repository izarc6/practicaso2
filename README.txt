		// ENTREGA PARCIAL II //
Sistemas Operativos II - Niveles de 1 a 11

		 *** Grupo KOC ***
	Castorina, Izar Maria Pietro
	    Lòpez Cortés, Andreu
		  Rocha, Lisandro

--------------------------------------------------------------------

Implementación de un sistema de ficheros basado en el formato EXT2 de Linux (cont.)

En esta segunda entrega, hemos añadido a nuestro sistema de ficheros
la posibilidad de contener carpetas, una función básica de cualquier
sistema de ficheros digno de ese nombre, además de la posibilidad de
crear enlaces entre diferentes elementos.

No ha sido fácil, igual que en la primera entrega parcial, pero
hemos conseguido hacer funcionar todo y llorando muy poco en el
proceso! Hemos aprendido a amar/odiar aún más la capacidad del
lenguaje C de trabajar a tan bajo nivel (pero sin ser tan
incomprensible cómo puede llegar a ser el Ensamblador).


Hemos también añadido el fichero opcional 'mi_touch'.

--------------------------------------------------------------------

mi_mkfs:  	Permite crear el sistema de ficheros.
Sintaxis: 	mi_mkfs <nombre SF> <tamaño en bloques>

escribir: 	Permite... pues, escribir, dándole un fichero como fuente o una linea
		de texto cualquiera. Se puede también especificar un offset.
Sintaxis: 	escribir <nombre SF> <\"$(cat fichero)\"> <diferentes_inodos>
		Offsets: 0, 5120, 256000, 30720000, 71680000
		Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets

leer:		Permite leer los contenidos del inodo especificado.
Sintaxis: 	leer <nombre SF> <n. de inodo>

truncar:  	Permite eliminar los bytes que siguen el especificado de un inodo.
Sintaxis: 	truncar <nombre SF> <n. de inodo> <n. de bytes>

permitir: 	Permite (heh) modificar los permisos de un determinado inodo.
Sintaxis: 	permitir <nombre SF> <n. de inodo> <permisos>

leer_sf:  	Permite realizar pruebas sobre el sistema de ficheros.
Sintaxis: 	leer_sf <nombre SF>

mi_cat:   	Equivalente a la función 'cat' de Linux/Unix.
		Visualiza los contenidos de un fichero de texto.
Sintaxis: 	mi_cat <nombre SF> </ruta fichero>

mi_chmod:   	Equivalente a la función 'chmod' de Linux/Unix.
		Cambia los permisos de un fichero o directorio.
Sintaxis: 	mi_chmod <nombre SF> <permisos> </ruta fichero>

mi_escribir:   	Permite escribir un texto dentro de un fichero aplicando un offset.
Sintaxis: 	mi_escribir <nombre SF> </ruta fichero> <texto> <offset>

mi_link:	Permite crear un enlace entre dos rutas.
Sintaxis:	mi_link <nombre SF> </ruta fichero origen> </ruta enlace>

mi_ls:   	Equivalente a la función 'ls' de Linux/Unix.
		Lista los detalles de los contenidos de un directorio.
Sintaxis: 	mi_ls <nombre SF> </ruta directorio>

mi_mkdir:   	Equivalente a la función 'mkdir' de Linux/Unix.
		Permite crear un nuevo directorio.
Sintaxis: 	mi_mkdir <nombre SF> <permisos ></ruta>

mi_rm:   	Equivalente a la función 'rm' de Linux/Unix.
		Permite eliminar un elemento del sistema de ficheros.
Sintaxis: 	mi_rm <nombre SF> </ruta>

mi_stat:	Permite visualizar detalles acerca del nodo de un fichero o de un directorio.
Sintaxis:	mi_stat <nombre SF> </ruta>

mi_touch:   	Permite crear un nuevo fichero vacío.
Sintaxis: 	mi_touch <nombre SF> <permisos ></ruta>



