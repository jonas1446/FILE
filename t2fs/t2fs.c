#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t2fs.h"



struct t2fs_superbloco conf = {.Id = '0'};  //Contém as variáveis de controle do superbloco e uma flag (id = '0') para setar novo disco
WORD nSetorBloco = 0;//Número Setores por bloco.
WORD nSetor = 0;//Número Setores Totais.


FILE2 ArqAtual = 0;


struct ArqStruct{  //Descritor do arquivo
	int bloco;
	int posNoBloco;
    t2fs_record record;
    t2fs_file handler;
    int currentPos;
};
typedef struct  ArqStruct Arquivo;



	void GetDiskInformation2()
	{

	   if(*conf.Id == '0')
		{

			char buffer[256];

			read_sector(0,buffer);

			if(buffer[0] == 'T' && buffer[1] == '2' && buffer[2] == 'F' && buffer[3] == 'S')
			{

				strcpy(conf.Id,"T2FS");
				conf.Version        = *((WORD *)(buffer + 4));
				conf.SuperBlockSize = *((WORD *)(buffer + 6));
				conf.DiskSize       =*((DWORD *)(buffer + 8));
				conf.NofBlocks      =*((DWORD *)(buffer + 12));
				conf.BlockSize      =*((DWORD *)(buffer + 16));
		        nSetorBloco= conf.BlockSize/256; // Numero de setores por bloco
                nSetor = conf.NofBlocks * nSetorBloco ; // Numero de setores totais do disco
				  printf("\n ************CONFIG****************\n");
				  printf("conf.Version       ->HEX:%8x  DEC:%8d\n",conf.Version,conf.Version);
				  printf("conf.SuperBlockSize->HEX:%8x  DEC:%8d\n",conf.SuperBlockSize,conf.SuperBlockSize);
				  printf("conf.DiskSize      ->HEX:%8x  DEC:%8d\n",conf.DiskSize,conf.DiskSize);
				  printf("conf.NofBlocks     ->HEX:%8x  DEC:%8d\n",conf.NofBlocks,conf.NofBlocks);
				  printf("conf.BlockSize     ->HEX:%8x  DEC:%8d\n",conf.BlockSize,conf.BlockSize);
				  printf("Setores por Bloco  ->HEX:xxx       DEC:%8d\n",nSetorBloco);
				  printf("Setores  do disco  ->HEX:xxx       DEC:%8d\n",nSetor);
				  printf("\n ***********************************\n");
			}
			else
			{
				  exit(1);//Não é um disco válido
			}
		}
	}


FILE2 create2 (char *filename);
{
    GetDiskInformation2();

    char *name;
    name = ExtendName(filename);

    Arquivo* arq = (Arquivo*)malloc(sizeof(Arquivo));
    memcpy(arq->record.name, name, 40);//sizeof(nome));
    arq->record.name[39] = 0;
    arq->record.blocksFileSize = 0;
    arq->record.bytesFileSize = 0;
    arq->record.dataPtr[0] = 0;
    arq->record.dataPtr[1] = 0;
    arq->record.singleIndPtr = 0;
    arq->record.doubleIndPtr = 0;
	arq->currentPos = 0;

    InsertFileRecord(&t->record, &t->bloco, &t->posNoBloco);

    arq->handler = ArqAtual;

    ArqAtual++;


	//printf("Tamanho em blocos: %d", t->record.blocksFileSize);
	//printf("Tamanho em blocos: %d", descritores_abertos[count_descritores]->record.blocksFileSize);

    return t->handler;
}








