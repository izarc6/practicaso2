		// ENTREGA PARCIAL //
Sistemas Operativos II - Niveles de 1 a 7

		 *** Grupo KOC ***
	Castorina, Izar Maria Pietro
	    Lòpez Cortés, Andreu
		  Rocha, Lisandro

--------------------------------------------------------------------

Implementación de un sistema de ficheros basado en el formato EXT2 de Linux

Durante la presentación de la práctica, no nos creíamos capables de poder
implementar un sistema de ficheros. Pensábamos que iba a ser imposible crear
algo tan complejo. Ahora, después de 7 niveles y muchas, muchas horas de
trabajo, estamos orgullosos de haberlo conseguido.
Además de tener un sistema de ficheros bien implementado y completamente
funcionante, aún mas importante ha sido el haber entendido cómo funciona un
file system a tan bajo nivel, llegando a manipular hasta los bits individuales.

Ampliando cada vez más los programas que hemos creado, hemos conseguido
implementar todas las funciones requeridas.
Actualmente, nuestro sistema no permite gestionar carpetas, y no es accesible
como los sistemas de ficheros que utilizamos diariamente en nuestros
ordenadores. Su manipulación se efectua a través de linea de comandos, los
cuales nos permiten crear el file system del tamaño deseado, gestionar sus
contenidos a nivel de bloques e inodos, efectuar pruebas de las funciones que
hemos desarrollado, además de escribir y leer texto.


Creemos que la dificultad más grande no haya sido ni implementar las funciones
ni entender los mecanismos internos del sistema, si no la constante creación de
nuevos errores a la hora de ampliar lo que ya teníamos hecho.
En cada nivel encontrábamos nuevos fallos, y a medida que el proyecto avanzaba
se volvía más y más difícil encontrar la función que generaba el error.

--------------------------------------------------------------------

mi_mkfs:  Permite crear el sistema de ficheros.
Sintaxis: mi_mkfs <nombre SF> <tamaño en bloques>

escribir: Permite... pues, escribir, dándole un fichero como fuente o una linea
		  de texto cualquiera. Se puede también especificar un offset.
Sintaxis: escribir <nombre SF> <\"$(cat fichero)\"> <diferentes_inodos>
		  Offsets: 0, 5120, 256000, 30720000, 71680000
		  Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets

leer:	  Permite leer los contenidos del inodo especificado.
Sintaxis: leer <nombre SF> <n. de inodo>

truncar:  Permite eliminar los bytes que siguen el especificado de un inodo.
Sintaxis: truncar <nombre SF> <n. de inodo> <n. de bytes>

permitir: Permite (heh) modificar los permisos de un determinado inodo.
Sintaxis: permitir <nombre SF> <n. de inodo> <permisos>

leer_sf:  Permite realizar pruebas sobre el sistema de ficheros.
Sintaxis: leer_sf <nombre SF>
