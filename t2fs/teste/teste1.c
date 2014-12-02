#include <stdio.h>
#include <t2fs.h>
#include <diskblocks.h>
#include <filecontrol.h>


int main()
{

    initSuperblock();
	identify2("Lucas e Jonas", 13);
	printf("%s", authors);

	printSuperblock();
	loadBitmap();
	printBitmap();	
	char str[30] = "Vamos Brasil! Rumo ao hexa!";
	char str2[60];
	FILE2 handle;
	FILE2 dir;
	
	dir = mkdir2("/casa");
 	dir = mkdir2("/casa/casa2");
	handle = create2("/casa/casa2/copa.txt");

	write2(handle, str, 27);

	close2(handle);


	handle = open2("/copa.txt");

	seek2(handle, -1);

	write2(handle, str, 27);

	seek2(handle, 0);

	read2(handle, str2, 27*2);

	close2(handle);


	printf("%s", str2);

	return 0;

	loadBitmap();
	printBitmap();	
	//printTAAP();
	
	return 0;
}
