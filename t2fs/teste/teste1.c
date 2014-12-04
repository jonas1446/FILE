#include <stdio.h>
#include <t2fs.h>
#include <diskblocks.h>
#include <filecontrol.h>


int main()
{
 
	
    //initSuperblock();
	//printSuperblock();
	//loadBitmap();
	//printBitmap();	
	char str[30] = "Vamos Brasil! Rumo ao tetra!";
	char str2[60];
	FILE2 handle;
	FILE2 dir;
	
	dir = t2fs_createDirectory("/casa10/");
    dir = t2fs_createDirectory("/casa10/casa2/");
    printf("\n\n**1**");
	chdir2("/casa10/casa2");
    printf("\n\n**12**");
    t2fs_createDirectory("../casa3");
	printf("\n\n**13**");
    chdir2("../casa3");
	printf("\n\n**14**");
	handle = t2fs_create("../g.txt");
    printf("\n\n**15**");
	t2fs_write(handle, str, 27);
    printf("\n\n**16**");
	t2fs_close(handle);
	printf("\n\n**17**");
	handle = t2fs_open("/casa10/casa3/g.txt");
    printf("\n\n**18**");
	t2fs_seek(handle, -1);
    printf("\n\n**19**");
	t2fs_write(handle, str, 27);
    printf("\n\n**122**");
	t2fs_seek(handle, 0);
    printf("\n\n**123**");
	t2fs_read(handle, str2, 27*2);
    printf("\n\n**1234**");
	t2fs_close(handle);


	printf("%s", str2);

	return 0;

	//loadBitmap();
	//printBitmap();	
	//printTAAP();
	
	return 0;
}
