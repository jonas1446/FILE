#include <stdio.h>
#include <t2fs.h>
#include <diskblocks.h>
#include <filecontrol.h>


int main()
{
 
	printf("%s", "======================================\n");

	initSuperblock();
	
	printSuperblock();
	loadBitmap();

	int identify;
	char str1[30] = "Random Text";
	char str2[60] = "Not so random Text";

	FILE2 handle;
	FILE2 dir;

	int write;	
	int close;	
	int seek;
	int read;

//	dirt2("/casa");
/**	
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
*/
/**
	printf("1c-%d\n", create2("sisop.txt"));
	printf("2c-%d\n", create2("sisop2.txt"));
	printf("3c-%d\n", create2("sisop2sisop2.txt"));
	printf("4c-%d\n", create2("sisop2sisop5.txt"));
	printf("5c-%d\n", create2("1sisoppsisop5.txt"));
	printf("6c-%d\n", create2("3sisoppsisop5.txt"));
	printf("7c-%d\n", create2("111.txt"));
	printf("8c-%d\n", create2("copa.txt"));
	printf("9c-%d\n", create2("copa2.txt"));
	printf("10c-%d\n", create2("createsimple.txt"));
	printf("11c-%d\n", create2("normalfile.txt"));
	printf("12c-%d\n", create2("normalfileRelative.txt"));
	printf("13c-%d\n", create2("extra1.txt"));
	printf("14c-%d\n", create2("extra2.txt"));
	printf("15c-%d\n", create2("extra3.txt"));
	printf("16c-%d\n", create2("extra4.txt"));

	printf("1d-%d\n", delete2("sisop.txt"));
	printf("2d-%d\n", delete2("sisop2.txt"));
	printf("3d-%d\n", delete2("sisop2sisop2.txt"));
	printf("4d-%d\n", delete2("sisop2sisop5.txt"));
	printf("5d-%d\n", delete2("1sisoppsisop5.txt"));
	printf("6d-%d\n", delete2("3sisoppsisop5.txt"));
	printf("7d-%d\n", delete2("111.txt"));
	printf("8d-%d\n", delete2("copa.txt"));
	printf("9d-%d\n", delete2("copa2.txt"));
	printf("10d-%d\n", delete2("createsimple.txt"));
	printf("11d-%d\n", delete2("normalfile.txt"));
	printf("12d-%d\n", delete2("normalfileRelative.txt"));
	printf("13d-%d\n", delete2("extra1.txt"));
	printf("14d-%d\n", delete2("extra2.txt"));
	printf("15d-%d\n", delete2("extra3.txt"));
	printf("16d-%d\n", delete2("extra4.txt"));
**/

	printf("17-%d\n", mkdir2("/casa"));
	printf("17-%d\n", rmdir2("/casa"));
	printf("18-%d\n", mkdir2("/folder"));
	printf("18-%d\n", rmdir2("/folder"));

	identify = identify2("Lucas e Jonas\0", 14);
	if(identify == 0) printf("108 identify2 - Setando o identify2 com racacter inválido, but no error found. It return: %d\n", identify);

	identify = identify2("Lucas e Jonas{", 14);
	if(identify == 0) printf("107 identify2 - Setando o identify2 com racacter inválido, but no error found. It return: %d\n", identify);

	identify = identify2("Lucas e Jonas", 13);
	if(identify != 0) printf("109 identify2 - Setando o identify2 normalmente, mas erro encontrado. It return: %d\n", identify);
	
	handle = create2("createsimple2.txt");
	if(handle < 0) printf("121 create2 - trying to create file (absolute path implicit) return an error, when it shouldn't. It return: %d\n", handle);
	handle = delete2("createsimple2.txt");
	if(handle < 0) printf("128 delete2 - trying to delete file (absolute path) return an error, when it shouldn't. It return: %d\n", handle);

	handle = create2("copa.txt");
	if(handle < 0) printf("101 create2 - trying to create file return an error, when it shouldn't. It return: %d\n", handle);
	write = write2(handle, str1, 27);
	if(write < 0) printf("123 write2 - trying to write on a file return an error, when it shouldn't. It return: : %d\n", write);
	read = read2(handle, str1, 27);
	if(read < 0) printf("124 write2 - trying to read a file return an error, when it shouldn't. It return: : %d\n", read);
	close = close2(handle);
	if(close < 0) printf("125 close2 - trying to close a file return an error, when it shouldn't. It return: %d\n", close);
	seek = seek2(handle, 0);
	if(seek < 0) printf("126 seek2 - trying to seek a file return an error, when it shouldn't. It return: %d\n", seek);
	handle = delete2("copa.txt");
	if(handle < 0) printf("128 delete2 - trying to delete file return an error, when it shouldn't. It return: %d\n", handle);

	dir = mkdir2("/casa"); printf("%d\n", dir);
	if(dir < 0) printf("105 mkdir2 - trying to create a directory return an error. It return: %d\n", dir);

	dir = rmdir2("/casa"); printf("%d\n", dir);
	if(dir < 0) printf("129 remove2 - trying to remove a directory return an error. It return: %d\n", dir);

 	dir = mkdir2("/casa2"); printf("%d\n", dir);
	if(dir < 0) printf("106 create2 - trying to create directory return an error. It return: %d\n", dir);
	dir = chdir2("/casa2"); printf("%d\n", dir);
	if(dir < 0) printf("130 chdir2 - trying to change to directory return an error. It return: %d\n", dir);
	
	char *pathname;
	handle = getcwd2(*pathname, 40); printf("%d", strcmp(pathname, "/casa2")); printf(" %d\n", strcmp(pathname, "/casa2"));
	printf("teste1 - getcwd2 - pathname = %s\n", *pathname);
	if(strcmp(*pathname, "/casa2")!=0) printf("112 create2 - trying to create a file return an error, when it shouldn't. It return: %s\n", *pathname);	

	if(handle < 0) printf("135 getcwd2 - trying to get current pathname return an error, when it shouldn't. It return: %d\n", handle);
	dir = chdir2("/"); printf("%d\n", dir);
	if(handle < 0) printf("131 chdir2 - trying to chdir a directory return an error, when it shouldn't. It return: %d\n", handle);
 	dir = rmdir2("/casa2"); printf("%d\n", dir);
	if(handle < 0) printf("132 rmdir2 - trying to remove a directory return an error, when it shouldn't. It return: %d\n", handle);

	handle = create2("normalfile.txt");
	if(handle < 0) printf("113 create2 - trying to create file return an error, when it shouldn't. It return: %d\n", handle);
	write = write2(handle, str1, 27);
	if(write < 0) printf("114 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", write);
	close = close2(handle);
	if(close < 0) printf("115 close2 - trying to close a file return an error, when it shouldn't. It return: %d\n", close);
	handle = create2("normalfile.txt");
	if(handle == 0) printf("102 create2 - trying to create file that already exists did not return an error. It return: %d\n", handle);
	handle = delete2("normalfile.txt");
	if(handle < 0) printf("133 delete2 - trying to delete file return an error, when it shouldn't. It return: %d\n", handle);

	dir = mkdir2("/folder");
	if(handle < 1) printf("103 create2 - trying to create normal directory that return an error, when it shouldn't. It return: %d\n", handle);
	dir = mkdir2("/folder");
	if(handle == 0) printf("104 create2 - trying to create directory that already exists did not return an error. It return: %d\n", handle);
 	dir = rmdir2("/folder");
	if(handle < 0) printf("134 rmdir2 - trying to remove a directory return an error, when it shouldn't. It return: %d\n", handle);

	handle = create2("/copa.txt");
	handle = open2("/copa.txt");
	seek2(handle, -1);
	write2(handle, str1, 27);
	seek2(handle, 0);
	read2(handle, str2, 27*2);
	close2(handle);

	printRecordBlock(2);
	printRecordBlock(1152);

	printf("%s", "======================================\n");

	return 0;
}
