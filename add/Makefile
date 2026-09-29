cacu: outputs/add_int.o outputs/add_float.o outputs/sub_int.o outputs/sub_float.o outputs/main.o
	gcc -o cacu outputs/add_int.o outputs/add_float.o outputs/sub_int.o outputs/sub_float.o outputs/main.o
outputs/add_int.o: add_int.c add.h | outputs
	gcc -c -o outputs/add_int.o add_int.c
outputs/add_float.o: add_float.c add.h | outputs
	gcc -c -o outputs/add_float.o add_float.c
outputs/sub_int.o: sub_int.c sub.h | outputs
	gcc -c -o outputs/sub_int.o sub_int.c
outputs/sub_float.o: sub_float.c sub.h | outputs
	gcc -c -o outputs/sub_float.o sub_float.c
outputs/main.o: main.c add.h sub.h | outputs
	gcc -c -o outputs/main.o main.c -Iinclude
outputs:
	mkdir -p outputs
clean:
	rm -f cacu outputs/*.o
