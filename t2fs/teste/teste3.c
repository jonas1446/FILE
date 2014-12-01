#include <t2fs.h>
#include <stdio.h>

int main()
{
	char str[25] = "Carissimi é o cara!";
	char str2[25];
	t2fs_file handle;

	handle = t2fs_create("/asc.txt");

	t2fs_write(handle, str, 21);

	t2fs_close(handle);


	handle = t2fs_open("/asc.txt");

	t2fs_read(handle, str2, 21);

	t2fs_close(handle);

	t2fs_delete("/asc.txt");

	printf("%s", str2);

	return 0;
}
