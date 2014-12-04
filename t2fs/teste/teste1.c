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
	DIR2 dir;
	
	dir = mkdir2("/casa10/");
    dir = mkdir2("/casa10/casa2/");
    printf("\n\n**1**");
	chdir2("/casa10/casa2");
    printf("\n\n**12**");
    mkdir2("../casa3");
	printf("\n\n**13**");
    chdir2("../casa3");
	printf("\n\n**14**");
	handle = create2("../g.txt");
    printf("\n\n**15**");
	write2(handle, str, 27);
    printf("\n\n**16**");
	close2(handle);
	printf("\n\n**17**");
	handle = open2("/casa10/casa3/g.txt");
    printf("\n\n**18**");
	seek2(handle, -1);
    printf("\n\n**19**");
	write2(handle, str, 27);
    printf("\n\n**122**");
	seek2(handle, 0);
    printf("\n\n**123**");
	read2(handle, str2, 27*2);
    printf("\n\n**1234**");
	close2(handle);


	printf("%s", str2);

	return 0;

	//loadBitmap();
	//printBitmap();	
	//printTAAP();
	
	return 0;
}
