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

	int identify;

	identify = identify2("Lucas e Jonas\0", 14);
	if(identify == 0) printf("108 identify2 - Setando o identify2 com racacter inválido, but no error found. It return: %d\n", identify);

	identify = identify2("Lucas e Jonas{", 14);
	if(identify == 0) printf("107 identify2 - Setando o identify2 com racacter inválido, but no error found. It return: %d\n", identify);

	identify = identify2("Lucas e Jonas", 13);
	if(identify != 0) printf("109 identify2 - Setando o identify2 normalmente, mas erro encontrado. It return: %d\n", identify);

	char str1[30] = "Random Text";
	char str2[60] = "Not so random Text";

	FILE2 handle;
	FILE2 dir;
	
	// should not return error
	handle = create2("/createsimple.txt");
	if(handle < 0) printf("116 create2 - trying to create file (absolute path) return an error, when it shouldn't. It return: %d\n", handle);

	// should not return error
	handle = create2("./createsimple2.txt");
	if(handle < 0) printf("118 create2 - trying to create file (relative path) return an error, when it shouldn't. It return: %d\n", handle);

	// should return error
	handle = create2("/notexist/copa.txt");
	if(handle == 0) printf("101 create2 - trying to create file on a directory that does not exists did not return an error. It return: %d\n", handle);

	// should not return an error
	dir = mkdir2("/casa");
	if(dir < 0) printf("105 create2 - trying to create directory return an error. It return: %d\n", dir);
 	dir = mkdir2("/casa/casa2");
	if(dir < 0) printf("106 create2 - trying to create directory inside a directory return an error. It return: %d\n", dir);
	handle = create2("/casa/casa2/copa.txt");
	if(handle < 0) printf("112 create2 - trying to create a file return an error, when it shouldn't. It return: %d\n", handle);
	int write;	
	write = write2(handle, str1, 27);
	if(write < 0) printf("110 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", write);
	int close;	
	close = close2(handle);
	if(close < 0) printf("111 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", close);


	handle = create2("/normalfile.txt");
	if(handle < 0) printf("113 create2 - trying to create directory inside a directory return an error. It return: %d\n", handle);
	write = write2(handle, str1, 27);
	if(write < 0) printf("114 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", write);
	close = close2(handle);
	if(close < 0) printf("115 close2 - trying to close a file return an error, when it shouldn't. It return: %d\n", close);
	handle = create2("/copa2.txt");
	if(handle < 0) printf("102 create2 - trying to create file that already exists did not return an error. It return: %d\n", handle);

	handle = create2("./normalfileRelative.txt");
	if(handle < 0) printf("117 create2 - trying to create directory inside a directory return an error. It return: %d\n", handle);
	write = write2(handle, str1, 27);
	if(write < 0) printf("118 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", write);
	close = close2(handle);
	if(close < 0) printf("119 close2 - trying to close a file return an error, when it shouldn't. It return: %d\n", close);
	handle = create2("/copa2.txt");
	if(handle < 0) printf("120 create2 - trying to create file that already exists did not return an error. It return: %d\n", handle);

	dir = mkdir2("/folder");
	if(handle == 1) printf("103 create2 - trying to create normal directory that return an error, when it shouldn't. It return: %d\n", handle);
	dir = mkdir2("/folder");
	if(handle != -1) printf("104 create2 - trying to create directory that already exists did not return an error. It return: %d\n", handle);

	handle = open2("/copa.txt");
	seek2(handle, -1);
	write2(handle, str1, 27);
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
