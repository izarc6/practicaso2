make clean
make
./mi_mkfs disco 100000
time ./simulacion disco
echo "A ver si ha hecho cosas de verdad"
./mi_ls disco /
