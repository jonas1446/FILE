makefile: libt2fs.a
	gcc -o bin/teste1 teste/teste1.c -I./include -L./lib -lt2fs -lapidisk -Wall
	gcc -o bin/testes teste/testes.c -I./include -L./lib -lt2fs -lapidisk -Wall

libt2fs.a: bin/t2fs.o bin/diskblocks.o bin/filecontrol.o
	ar crs lib/libt2fs.a bin/t2fs.o bin/diskblocks.o bin/filecontrol.o lib/libapidisk.a

bin/t2fs.o:  src/t2fs.c
	gcc -c src/t2fs.c -o bin/t2fs.o -I./include -Wall

bin/diskblocks.o: src/diskblocks.c
	gcc -c src/diskblocks.c -o bin/diskblocks.o -I./include -Wall

bin/filecontrol.o: src/filecontrol.c
	gcc -c src/filecontrol.c -o bin/filecontrol.o -I./include -Wall

clean:
	rm lib/libt2fs.a bin/*.o
	rm bin/teste1
