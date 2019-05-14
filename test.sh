make clean
make
./mi_mkfs disco 100000
./mi_mkdir disco 6 /dir1/
./mi_mkdir disco 6 /dir1/dir2/
./mi_chmod disco 1 /dir1/dir2/
echo "Intento cambiar permisos de /dir1/dir2/ a 2"
./mi_chmod disco 2 /dir1/dir2/