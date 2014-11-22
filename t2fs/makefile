all: clear t2fs.o
	gcc -o src/chamadorold src/chamadorold.c t2fs.o lib/libapidisk.a -W
	./chamadorold

t2fs.o:
	gcc -c src/t2fs.c -W

clear:
	rm -rf *.o
