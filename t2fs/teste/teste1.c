#include <stdio.h>
#include <t2fs.h>
#include <diskblocks.h>
#include <filecontrol.h>


int main()
{

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

	printRecordBlock(1);

//	printf("1c-%d\n", create2("/sisop.txt"));
//	printf("1d-%d\n", delete2("/sisop.txt"));

/**
	printRecordBlock(2);

	printf("1c-%d\n", create2("/sisop.txt"));
	printf("2c-%d\n", create2("/sisop2.txt"));
	printf("3c-%d\n", create2("/sisop2sisop2.txt"));
	printf("4c-%d\n", create2("/sisop2sisop5.txt"));
	printf("5c-%d\n", create2("/1sisoppsisop5.txt"));
	printf("6c-%d\n", create2("/3sisoppsisop5.txt"));
	printf("7c-%d\n", create2("/111.txt"));
	printf("8c-%d\n", create2("/copa.txt"));
	printf("9c-%d\n", create2("/copa2.txt"));
	printf("10c-%d\n", create2("/createsimple.txt"));
	printf("11c-%d\n", create2("/normalfile.txt"));
	printf("12c-%d\n", create2("/normalfileRelative.txt"));
	printf("13c-%d\n", create2("/extra1.txt"));
	printf("14c-%d\n", create2("/extra2.txt"));
	printf("15c-%d\n", create2("/extra3.txt"));
	printf("16c-%d\n", create2("/extra4.txt"));

	printf("1d-%d\n", delete2("/sisop.txt"));
	printf("2d-%d\n", delete2("/sisop2.txt"));
	printf("3d-%d\n", delete2("/sisop2sisop2.txt"));
	printf("4d-%d\n", delete2("/sisop2sisop5.txt"));
	printf("5d-%d\n", delete2("/1sisoppsisop5.txt"));
	printf("6d-%d\n", delete2("/3sisoppsisop5.txt"));
	printf("7d-%d\n", delete2("/111.txt"));
	printf("8d-%d\n", delete2("/copa.txt"));
	printf("9d-%d\n", delete2("/copa2.txt"));
	printf("10d-%d\n", delete2("/createsimple.txt"));
	printf("11d-%d\n", delete2("/normalfile.txt"));
	printf("12d-%d\n", delete2("/normalfileRelative.txt"));
	printf("13d-%d\n", delete2("/extra1.txt"));
	printf("14d-%d\n", delete2("/extra2.txt"));
	printf("15d-%d\n", delete2("/extra3.txt"));
	printf("16d-%d\n", delete2("/extra4.txt"));

	//printf("13-%d\n", rmdir2("/casa"));
	//printf("14-%d\n", rmdir2("/folder"));

	printRecordBlock(2);
*/

/**
	identify = identify2("Lucas e Jonas\0", 14);
	if(identify == 0) printf("108 identify2 - Setando o identify2 com racacter inválido, but no error found. It return: %d\n", identify);

	identify = identify2("Lucas e Jonas{", 14);
	if(identify == 0) printf("107 identify2 - Setando o identify2 com racacter inválido, but no error found. It return: %d\n", identify);

	identify = identify2("Lucas e Jonas", 13);
	if(identify != 0) printf("109 identify2 - Setando o identify2 normalmente, mas erro encontrado. It return: %d\n", identify);
	
	handle = create2("/createsimple.txt");
	if(handle < 0) printf("116 create2 - trying to create file (absolute path) return an error, when it shouldn't. It return: %d\n", handle);

	handle = create2("createsimple2.txt");
	if(handle < 0) printf("121 create2 - trying to create file (absolute path implicit) return an error, when it shouldn't. It return: %d\n", handle);

	handle = create2("./createsimple3.txt");
	if(handle < 0) printf("122 create2 - trying to create file (relative path) return an error, when it shouldn't. It return: %d\n", handle);

	handle = create2("/notexist/copa.txt");
	if(handle == 0) printf("101 create2 - trying to create file on a directory that does not exists did not return an error. It return: %d\n", handle);
	write = write2(handle, str1, 27);
	if(write == 0) printf("123 write2 - trying to write on a file with error, did not return an error. It return: %d\n", write);
	read = read2(handle, str1, 27);
	if(read == 0) printf("124 write2 - trying to read a file with error, did not return an error. It return: %d\n", read);
	close = close2(handle);
	if(close == 0) printf("125 close2 - trying to close a file with error, did not return an error. It return: %d\n", close);
	seek = seek2(handle, 0);
	if(seek == 0) printf("126 seek2 - trying to seek a file with error, did not return an error. It return: %d\n", seek);

	dir = mkdir2("/casa");
	if(dir < 0) printf("105 create2 - trying to create directory return an error. It return: %d\n", dir);

 	dir = mkdir2("/casa/casa2");
	if(dir < 0) printf("106 create2 - trying to create directory inside a directory return an error. It return: %d\n", dir);
	handle = create2("/casa/casa2/copa.txt");
	if(handle < 0) printf("112 create2 - trying to create a file return an error, when it shouldn't. It return: %d\n", handle);
	write = write2(handle, str1, 27);
	if(write < 0) printf("110 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", write);
	close = close2(handle);
	if(close < 0) printf("111 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", close);

	handle = create2("/normalfile.txt");
	if(handle < 0) printf("113 create2 - trying to create file return an error, when it shouldn't. It return: %d\n", handle);
	write = write2(handle, str1, 27);
	if(write < 0) printf("114 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", write);
	close = close2(handle);
	if(close < 0) printf("115 close2 - trying to close a file return an error, when it shouldn't. It return: %d\n", close);
	handle = create2("/normalfile.txt");
	if(handle == 0) printf("102 create2 - trying to create file that already exists did not return an error. It return: %d\n", handle);

	handle = create2("./normalfileRelative.txt");
	if(handle < 0) printf("117 create2 - trying to create normal file using relative path return an error. It return: %d\n", handle);
	write = write2(handle, str1, 27);
	if(write < 0) printf("118 write2 - trying to write a file return an error, when it shouldn't. It return: %d\n", write);
	close = close2(handle);
	if(close < 0) printf("119 close2 - trying to close a file return an error, when it shouldn't. It return: %d\n", close);
	handle = create2("/normalfileRelative.txt");
	if(handle == 0) printf("120 create2 - trying to create file that already exists did not return an error. It return: %d\n", handle);

	dir = mkdir2("/folder");
	if(handle < 1) printf("103 create2 - trying to create normal directory that return an error, when it shouldn't. It return: %d\n", handle);
	dir = mkdir2("/folder");
	if(handle == 0) printf("104 create2 - trying to create directory that already exists did not return an error. It return: %d\n", handle);

	handle = create2("/copa.txt");
	handle = open2("/copa.txt");
	seek2(handle, -1);
	write2(handle, str1, 27);
	seek2(handle, 0);
	read2(handle, str2, 27*2);
	close2(handle);
*/
	return 0;
}
