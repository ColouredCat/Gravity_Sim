
gcc -c *.c
gcc -o game *.o -lraylib -lm -D_REETRANT -I/usr/include/
rm *.o
./game
