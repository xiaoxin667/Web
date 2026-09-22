cacu: add_int.o add_float.o sub_int.o sub_float.o main.o
	gcc -o cacu add_int.o add_float.o sub_int.o sub_float.o main.o
add_int.o: add_int.c add.h
	gcc -c -o add_int.o add_int.c
add_float.o: add_float.c add.h
	gcc -c -o add_float.o add_float.c
sub_int.o: sub_int.c sub.h
	gcc -c -o sub_int.o sub_int.c
sub_float.o: sub_float.c sub.h
	gcc -c -o sub_float.o sub_float.c
main.o: main.c add.h sub.h
	gcc -c -o main.o main.c -Iinclude
clean:
	rm -f cacu add_int.o add_float.o sub_int.o sub_float.o main.o
