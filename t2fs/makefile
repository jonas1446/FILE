all: clear t2fs.o
	gcc -o chamadorold chamadorold.c t2fs.o libapidisk.a -W
	./chamadorold

t2fs.o:
	gcc -c t2fs.c -W

clear:
	rm -rf *.o
