
gcc -c *.c -lraylib -lm -D_REETRANT 
gcc -o game *.o -lraylib -lm -D_REETRANT 
rm *.o
./game
