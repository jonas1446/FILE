#include <stdio.h>
#include <t2fs.h>
#include <diskblocks.h>
#include <filecontrol.h>


int main()
{

    initSuperblock();
	printSuperblock();
	loadBitmap();
	printBitmap();	
	char str[30] = "Vamos Brasil! Rumo ao hexa!";
	char str2[60];
	t2fs_file handle;
	t2fs_file dir;
	
	dir = t2fs_createDirectory("/casa");
    dir = t2fs_createDirectory("/casa/casa2");
	handle = t2fs_create("/casa/casa2/copa.txt");

	t2fs_write(handle, str, 27);

	t2fs_close(handle);


	handle = t2fs_open("/copa.txt");

	t2fs_seek(handle, -1);

	t2fs_write(handle, str, 27);

	t2fs_seek(handle, 0);

	t2fs_read(handle, str2, 27*2);

	t2fs_close(handle);


	printf("%s", str2);

	return 0;

	loadBitmap();
	printBitmap();	
	//printTAAP();
	
	return 0;
}
