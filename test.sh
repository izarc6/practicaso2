make clean
make
./mi_mkfs disco 100000
echo -e "Creado sistema de ficheros \"disco\"\n"
time ./simulacion disco
echo -e "\nA ver si ha hecho cosas de verdad\n"
./mi_ls disco /
