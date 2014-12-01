#include <stdio.h>
#include <t2fs.h>


/*
./program hello world

    argc would be 3.
    argv[0] would be "./program".
    argv[1] would be "hello".
    argv[2] would be "world".

*/


int main(int argc, char **argv)
{
	char * origin = argv[1];
	char * destination = argv[2];
    char data[1024];
	FILE * originFile;
	//FILE * destinationFile;
	t2fs_file destinationFile;

	originFile = fopen(origin, "r");

	t2fs_create(destination);

	//destinationFile = fopen(destination,"w");
	destinationFile = t2fs_open(destination);

	if(originFile == NULL || destination == -1)
	{
			printf("\nErro, nao foi possivel abrir o arquivo\n");
			return -1;
	}
	else
	{
		while( (fgets(data, sizeof(data), originFile)) != NULL )
		{

			t2fs_write(destinationFile, data, 1024);

            //fputs(data,destinationFile);
		    //printf("%s", data);
		}

        fclose(originFile);
        t2fs_close(destinationFile);

	}
    printf("\n\n");
	return 0;
}
