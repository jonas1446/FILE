#include <stdio.h>
#include <t2fs.h>


int main(int argc, char **argv)
{
	char * folder = argv[1];
	

	if(t2fs_deleteDirectory(folder) == -1)
	{
		printf("\nNão foi possível deletar o diretório.\n");
	}
	else
	{
		printf("\nDiretório %s deletado com sucesso!\n", folder);

	}


	printf("\n\n");

	return 0;

}
