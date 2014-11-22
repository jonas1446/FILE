#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t2fs.h"



struct t2fs_superbloco conf = {.Id = '0'};  //Contém as variáveis de controle do superbloco e uma flag (id = '0') para setar novo disco
WORD nSetorBloco = 0;//Número Setores por bloco.
WORD nSetor = 0;//Número Setores Totais.




struct ArqStruct{  //Descritor do arquivo
	int bloco;
	int posNoBloco;
    struct t2fs_record record;
    FILE2 handler;
    int currentPos;
};
typedef struct  ArqStruct Arquivo;



Arquivo* Arquivos_Criados[20];
int NroArquivos = 0;
FILE2 ArqAtual = 0;



  char* ExtendName(char *nome)
    {
        char* extName = (char *)malloc(30);
        int i = 0;

        while (nome[i] != 0)
        {
            extName[i] = nome[i];
            i++;
        }
        while (i<30)
        {
            extName[i] = 0;
            i++;
        }
        return (extName);
    }


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
				conf.DiskSize       = *((DWORD *)(buffer + 8));
				conf.NofBlocks      = *((DWORD *)(buffer + 12));
				conf.BlockSize      = *((DWORD *)(buffer + 16));
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
     int setBitBitmap(int posicao, short int ocupado)   //seta ou reseta um bit do bitmap
    {
        char block[blockSize];
        int iBloco, posBit, posByte, iAux;

        iBloco = ctrlSize + posicao/(8*blockSize);		//posição do bloco que contém o bit desejado
        //printf("\nBit no bloco: %d ", iBloco);

        read_block(iBloco, block);        //lê o bloco

        iAux = (posicao - ((iBloco-1) * blockSize));

        posByte = iAux / 8;

        posBit = 7-(iAux % 8);

        char auxByte;
        auxByte = block[posByte];
        //printf("\nBloco antes: %x - 1 deslocado: %d - posBit: %d ", block[posByte], (1 << posBit), posBit);
        if (ocupado)
            block[posByte] = auxByte | (1 << posBit);
        else
            block[posByte] = auxByte & (254 << posBit);

        //printf("\nBloco depois: %x ", block[posByte]);
        //block[posByte] = ; //setar o bit

        //printf(" - Bit %d do byte: %d\n", posBit, posByte);

        write_block(iBloco, block);       //escreve no disco

        return 0;
    }

    int InsertFileRecord(t2fs_record* record, int *blocoPtr, int *posPtr)
    {
        int i=0, dirty = 0, iBloco = 0, lenBlkCtrl = 0;
        char buffer[256];
        lenBlkCtrl = ctrlSize + freeBlockSize;    //offset para posição do root

        for(iBloco = 0; iBloco < conf.NofBlocks; iBloco++)  //varre todo os blocos da area de dados
            {
          for (iSetor = 1; i <nSetorBloco ; iSetor++){	// cada bloco tem N setores, logo lê esses n setores.

            read_sector(iBloco*nSetorBloco + iSetor + 1, buffer);   //lê o setor, se soma 1 ao endereço de bloco * qtd de setores por bloco + nro do setor no bloco atual pois o primeiro setor é o superbloco.

            for (i = 0; i < 256; i+=64)		//varre o setor lido
            {
                if((unsigned char)buffer[i] < 161 || (unsigned char)buffer[i] > 250)  //se o registro não for válido, grava o novo registro
                {
                    //memcpy(buffer+i, record, sizeof(*record));  //grava o primeiro registro
                   // buffer[i] += 128;		//soma 128 no primeiro caracter

                   //[jonas]tem de escrever nos 4 setores do bloco

                    dirty = 1;
                    break;
                }
            }
            if(dirty)
            {
                //printf("Salvo no bloco %d\n", lenBlkCtrl + iBloco);
                //write_block(lenBlkCtrl + iBloco, buffer);       //escreve no disco
                setBitBitmap(iBloco + lenBlkCtrl, 1);//[jonas] <--- isso é um arquivou ou ponteiro para arquivo agora
                *blocoPtr = lenBlkCtrl + iBloco;
                *posPtr = i;
                break;
            }
          }
        }

        return 0;
    }

    FILE2 create2 (char *filename)
    {
        GetDiskInformation2();


        char *name;
        name = ExtendName(filename);

        Arquivo* arq = (Arquivo*)malloc(sizeof(Arquivo));
        memcpy(arq->record.name, name, 30);//sizeof(nome));
        arq->record.name[30] = 0;
        arq->record.blocksFileSize = 0;
        arq->record.bytesFileSize = 0;
        arq->record.dataPtr[0] = 0;
        arq->record.dataPtr[1] = 0;
        arq->record.singleIndPtr = 0;
        arq->record.doubleIndPtr = 0;
        arq->currentPos = 0;

        InsertFileRecord(&arq->record, &arq->bloco, &arq->posNoBloco);

        arq->handler = ArqAtual;
        ArqAtual++;
        Arquivos_Criados[NroArquivos] = arq;
        NroArquivos++;
        //printf("Tamanho em blocos: %d", t->record.blocksFileSize);
        //printf("Tamanho em blocos: %d", descritores_abertos[count_descritores]->record.blocksFileSize);

        return arq->handler;
    }








