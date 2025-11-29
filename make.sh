
gcc -c *.c
gcc -o game *.o -lraylib -lm -D_REETRANT -I/usr/local/include -L/usr/local/lib
rm *.o
./game
