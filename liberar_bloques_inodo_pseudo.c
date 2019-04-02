funcion liberar_bloques_inodo(ninodo:unsigned ent, nblogico:unsigned ent) devolver liberados:ent 

  var
     inodo: estructura inodo
     nRangoBL, nivel_punteros, indice, ptr, nblog, ultimoBL: unsigned ent
     bloques_punteros [3] [NPUNTEROS]: ent //array de bloques de punteros
     ptr_nivel [3]: ent  //punteros a bloques de punteros de cada nivel
     indices[3]: ent  //indices de cada nivel
     liberados, salvar_inodo: ent  // nº de bloques liberados
  fvar

  liberados:=0, salvar_inodo := 0
  leer_inodo(ninodo, &inodo)
  si inodo.tamEnBytesLog = 0 entonces devolver 0 fsi // el fichero vacío
  //obtenemos el último bloque lógico del inodo
  si inodo.tamEnBytesLog % BLOCKSIZE = 0 entonces
    ultimoBL := inodo.tamEnBytesLog / BLOCKSIZE - 1
  si_no  ultimoBL := inodo.tamEnBytesLog / BLOCKSIZE
  fsi
  ptr:= 0
  para nblog := nblogico hasta nblog = ultimoBL paso 1 hacer //recorrido BLs 
    nRangoBL := obtener_nrangoBL(inodo, nblog, &ptr)
    si nRangoBL < 0 entonces devolver ERROR fsi
    nivel_punteros := nRangoBL //el nivel_punteros +alto cuelga del inodo

    mientras (ptr > 0 && nivel_punteros > 0) hacer //cuelgan bloques de punteros
      bread(ptr, bloques_punteros[nivel_punteros-1]) 
      indice := obtener_indice(nblog, nivel_punteros)
      ptr_nivel[nivel_punteros-1] := ptr
      indices[nivel_punteros-1] := indice
      ptr := bloques_punteros[nivel_punteros-1][indice]
      nivel_punteros--
    fmientras

    si ptr > 0 entones //si existe bloque de datos
      liberar_bloque(ptr)
      liberados++
      si nRangoBL = 0 entonces //es un puntero Directo
        inodo.punterosDirectos[nblog]:= 0, salvar_inodo:=1
      si_no 
        mientras nivel_punteros < nRangoBL hacer
          indice := indices[nivel_punteros]
          bloques_punteros[nivel_punteros][indice] := 0
          ptr := ptr_nivel [nivel_punteros]         
          si bloques_punteros[nivel_punteros] = 0  entonces
            //No cuelgan bloques ocupados, hay que liberar el bloque de punteros
            liberar_bloque(ptr)
            liberados++
            nivel_punteros++
            si nivel_punteros = nRangoBL entonces                
              inodo.punterosIndirectos[nRangoBL-1] := 0, salvar_inodo:=1
            fsi 
          si_no  //escribimos en el dispositivo el bloque de punteros modificado
            bwrite(ptr, bloques_punteros[nivel_punteros]) 
            nivel_punteros := nRangoBL // para salir del bucle
          fsi
        fmientras
      fsi
    fsi
  fpara
  si salvar_inodo=1 entonces
      escribir_inodo(ninodo, inodo)  //sólo si lo hemos actualizado
  fsi
  devolver liberados
ffuncion
