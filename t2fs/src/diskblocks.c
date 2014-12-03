#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <t2fs.h>
#include <apidisk.h>
#include <stdint.h>
#include <diskblocks.h>
#include <filecontrol.h>

/** superbloco global **/
t2fs_superblock * superblock = NULL;

// Procura pela entrada de diretório do arquivo com o nome informado
t2fs_record * findRecord(char * name, BYTE TypeVal, unsigned int* recordBlock)
{
	char address[MAX_SIZE_NAME];
	int i, TypeValFlag = 0;
	BYTE type;

	// Recebe o caminho absoluto do arquivo (name)
	// ex.: /dir1/dir2/dir3/dir4/arq1.txt (necessário fazer split)
	char * token = NULL;

	// Inicializa superbloco
	if(superblock == NULL)
		initSuperblock();

	// Faz uma cópia
	strcpy(address,name);

	if (!(strcmp(address, "/")))
	{
		*recordBlock = -1;// superbloco = -1
		return &superblock->RootDirReg;
	}
		

	if (TypeVal == TYPEVAL_REGULAR)
		for (i = 0; i < sizeof(address); i++)
			if(address[i] == 47) // "/" = 47
				TypeValFlag++;


	type = TYPEVAL_DIRETORIO;
	token = strtok(address,"/");
	t2fs_record * record; 
	record = &superblock->RootDirReg;

	if (record->dataPtr[0] == -1)
		return NULL;

	while( token != NULL)
	{
		DWORD dataPtr[4] = {record->dataPtr[0],record->dataPtr[1],record->dataPtr[2],record->dataPtr[3]};
		DWORD singleIndPtr = record->singleIndPtr;
		DWORD doubleIndPtr = record->doubleIndPtr;

		if(TypeVal == TYPEVAL_REGULAR && TypeValFlag == 1)
			type = 1;

		record = loadDataPtr(dataPtr[0], type, token);
		*recordBlock = dataPtr[0];

		if (record == NULL)
		{	
			if (dataPtr[1] == -1)
				return NULL;

			record = loadDataPtr(dataPtr[1], type, token);
			*recordBlock = dataPtr[1];

			if (record == NULL)
			{	
				if (dataPtr[2] == -1)
					return NULL;

				record = loadDataPtr(dataPtr[2], type, token);
				*recordBlock = dataPtr[2];

				if (record == NULL)
				{	
					if (dataPtr[3] == -1)
						return NULL;

					record = loadDataPtr(dataPtr[3], type, token);
					*recordBlock = dataPtr[3];

					if (record == NULL)
					{
						if (singleIndPtr == -1)
							return NULL;
				
						record = loadSingleIndPtr(singleIndPtr, type, token, recordBlock);
						if (record == NULL)
						{
							if (doubleIndPtr == -1)
								return NULL;
						
							record = loadDoubleIndPtr(doubleIndPtr, type, token, recordBlock);

							if (record == NULL)
							{
								return NULL;
							}
						}
					}
				}
					
			}
		}
		token = strtok(NULL,"/");
		TypeValFlag--;
	}


	return record; 
}

t2fs_record* loadDataPtr(unsigned int block, BYTE TypeVal, char* token){

	t2fs_record * loadedBlock;
	int i = 0;

	if(block <= 0 || block >= NOF_BLOCKS)
		return NULL;
	
	loadedBlock = loadRecordsBlock(block);

	while(i < BLOCK_SIZE / RECORD_SIZE )
	{
		// Se os nomes são iguais e tipo do descritor é igual (diretório ou arquivo), existe a pasta ou arquivo
		if(!(strcmp(loadedBlock[i].name,token)) && ( loadedBlock[i].TypeVal == TypeVal)) 
		{	
			return &loadedBlock[i];
			
		}
		else
		{
			// Nomes não são iguais, procura pelo próximo
			i++;
		}	
		
	}

	return NULL;


}

t2fs_record* loadSingleIndPtr(unsigned int block, BYTE TypeVal, char* token, unsigned int* recordBlock){

	DWORD * loadedBlock;
	int i = 0;
	t2fs_record* record;

	if(block <= 0 || block >= NOF_BLOCKS)
		return NULL;

	loadedBlock = loadIndexBlock(block);

	while (i != sizeof(loadedBlock) && loadedBlock[i] != -1){

		if ( (record = loadDataPtr(loadedBlock[i], TypeVal, token)) != NULL){
			*recordBlock = loadedBlock[i];
			return record;
		}
		else
			i++;
	}

	return NULL;
 
}

t2fs_record* loadDoubleIndPtr(unsigned int block, BYTE TypeVal, char* token, unsigned int* recordBlock){

	DWORD * loadedBlock;
	int i = 0;
	t2fs_record* record;

	if (block <= 0 && block >= NOF_BLOCKS)
		return NULL;

	loadedBlock = loadIndexBlock(block);

	while (i != sizeof(loadedBlock) && loadedBlock[i] != -1){


		if ( (record = loadSingleIndPtr(loadedBlock[i], TypeVal, token, recordBlock)) != NULL)
			return record;
		else
			i++;
	}

	return NULL;	
}


DWORD* loadIndexBlock(unsigned int block){

	DWORD * loadedBlock;
	char * buffer;
	int i;
	buffer = (char*) malloc(BLOCK_SIZE);

	buffer = loadBlock(block);

	// Aloca área para estrutura
	loadedBlock = (DWORD *) malloc(BLOCK_SIZE);

	if(loadedBlock == NULL)
		error_read(block);

	for(i=0; i < BLOCK_SIZE / sizeof(DWORD) ; i++)
	{
		memcpy(&loadedBlock[i], buffer+sizeof(DWORD)*i, sizeof(DWORD));

	}

	return loadedBlock;


}

// Carrega bloco estrutura interna de entrada de diretório
t2fs_record * loadRecordsBlock(unsigned int block)
{
	t2fs_record * loadedBlock;
	char * buffer;
	int i;
	buffer = (char*) malloc(BLOCK_SIZE);

	buffer = loadBlock(block);

	// Aloca área para estrutura
	loadedBlock = (t2fs_record *) malloc(BLOCK_SIZE);

	if(loadedBlock == NULL)
		error_read(block);

	for(i=0; i < BLOCK_SIZE / RECORD_SIZE ; i++)
	{
		memcpy(&loadedBlock[i].TypeVal, buffer+RECORD_SIZE*i, 1);
		memcpy(&loadedBlock[i].name, buffer+1+RECORD_SIZE*i, 31); // Colocar \0 ??? testar
		memcpy(&loadedBlock[i].blocksFileSize, buffer+32+RECORD_SIZE*i,4);
		memcpy(&loadedBlock[i].bytesFileSize, buffer+36+RECORD_SIZE*i,4);
		memcpy(&loadedBlock[i].dataPtr, buffer+40+RECORD_SIZE*i,16);
		memcpy(&loadedBlock[i].singleIndPtr, buffer+56+RECORD_SIZE*i,4);
		memcpy(&loadedBlock[i].doubleIndPtr, buffer+60+RECORD_SIZE*i,4);

	}

	return loadedBlock;

}



/** SUPERBLOCK **/

void initSuperblock(void)
{
	char * buffer;
	int read;
	unsigned int superblock_sector = 0;

	// Aloca área para os bytes lidos do disco
	buffer = (char*) malloc(SECTOR_SIZE);
	// Lê bloco do disco
	read = read_sector(superblock_sector, buffer);

	if(read != NOT_ERROR)
		error_read(superblock_sector);

	// Armazena o bloco lido no superbloco (em memória)
	superblock = (t2fs_superblock *) malloc(sizeof(t2fs_superblock));
	memcpy(superblock->Id, buffer, 4);
	memcpy(&superblock->Version, buffer+4,2);
	memcpy(&superblock->SuperBlockSize, buffer+6,2);
	memcpy(&superblock->DiskSize, buffer+8,4);
	memcpy(&superblock->NofBlocks, buffer+12,4);
	memcpy(&superblock->BlockSize, buffer+16,4);
	memcpy(&superblock->Reserved, buffer+20,108);
	memcpy(&superblock->BitMapReg, buffer+128,64);
	memcpy(&superblock->BitMapReg, buffer+128,64);
	memcpy(&superblock->RootDirReg, buffer+192,64);

	
	free(buffer);

}

void printSuperblock()
{

	printf("\n ************CONFIG****************\n");
	printf("superblock->Version       ->HEX:%8x  DEC:%8d\n",superblock->Version,superblock->Version);
	printf("superblock->SuperBlockSize->HEX:%8x  DEC:%8d\n",superblock->SuperBlockSize,superblock->SuperBlockSize);
	printf("superblock->DiskSize      ->HEX:%8x  DEC:%8d\n",superblock->DiskSize,superblock->DiskSize);
	printf("superblock->NofBlocks     ->HEX:%8x  DEC:%8d\n",superblock->NofBlocks,superblock->NofBlocks);
	printf("superblock->BlockSize     ->HEX:%8x  DEC:%8d\n",superblock->BlockSize,superblock->BlockSize);
	//printf("Setores por Bloco  ->HEX:xxx       DEC:%8d\n",nSetorBloco);
	//printf("Setores  do disco  ->HEX:xxx       DEC:%8d\n",nSetor);
	printf("\n ***********************************\n");


	printf("\n *********BITMAP CONFIG*************\n");
	printf("superblock->BitMapReg.name: %s\n",superblock->BitMapReg.name);
	printf("superblock->BitMapReg.blocksFileSize ->HEX:%8x  DEC:%8d\n",superblock->BitMapReg.blocksFileSize,superblock->BitMapReg.blocksFileSize);
	printf("superblock->BitMapReg.bytesFileSize  ->HEX:%8x  DEC:%8d\n",superblock->BitMapReg.bytesFileSize,superblock->BitMapReg.bytesFileSize);
	printf("superblock->BitMapReg.doubleIndPtr   ->HEX:%8x  DEC:%8d\n",superblock->BitMapReg.doubleIndPtr,superblock->BitMapReg.doubleIndPtr);
	printf("superblock->BitMapReg.singleIndPtr   ->HEX:%8x  DEC:%8d\n",superblock->BitMapReg.singleIndPtr,superblock->BitMapReg.singleIndPtr);
	printf("superblock->BitMapReg.dataPtr[0]     ->HEX:%8x  DEC:%8d\n",superblock->BitMapReg.dataPtr[0],superblock->BitMapReg.dataPtr[0]);
	printf("superblock->BitMapReg.dataPtr[1]     ->HEX:%8x  DEC:%8d\n",superblock->BitMapReg.dataPtr[1],superblock->BitMapReg.dataPtr[1]);
	printf("superblock->BitMapReg.dataPtr[2]     ->HEX:%8x  DEC:%8d\n",superblock->BitMapReg.dataPtr[2],superblock->BitMapReg.dataPtr[2]);
	printf("superblock->BitMapReg.dataPtr[3]     ->HEX:%8x  DEC:%8d\n",superblock->BitMapReg.dataPtr[3],superblock->BitMapReg.dataPtr[3]);
	printf("\n ***********************************\n");


	printf("\n ********* ROOT CONFIG *************\n");
	printf("superblock->RootDirReg.name: %s\n",superblock->RootDirReg.name);
	printf("superblock->RootDirReg.blocksFileSize ->HEX:%8x  DEC:%8d\n",superblock->RootDirReg.blocksFileSize,superblock->RootDirReg.blocksFileSize);
	printf("superblock->RootDirReg.bytesFileSize  ->HEX:%8x  DEC:%8d\n",superblock->RootDirReg.bytesFileSize,superblock->RootDirReg.bytesFileSize);
	printf("superblock->RootDirReg.doubleIndPtr   ->HEX:%8x  DEC:%8d\n",superblock->RootDirReg.doubleIndPtr,superblock->RootDirReg.doubleIndPtr);
	printf("superblock->RootDirReg.singleIndPtr   ->HEX:%8x  DEC:%8d\n",superblock->RootDirReg.singleIndPtr,superblock->RootDirReg.singleIndPtr);
	printf("superblock->RootDirReg.dataPtr[0]     ->HEX:%8x  DEC:%8d\n",superblock->RootDirReg.dataPtr[0],superblock->RootDirReg.dataPtr[0]);
	printf("superblock->RootDirReg.dataPtr[1]     ->HEX:%8x  DEC:%8d\n",superblock->RootDirReg.dataPtr[1],superblock->RootDirReg.dataPtr[1]);
	printf("superblock->RootDirReg.dataPtr[2]     ->HEX:%8x  DEC:%8d\n",superblock->RootDirReg.dataPtr[2],superblock->RootDirReg.dataPtr[2]);
	printf("superblock->RootDirReg.dataPtr[3]     ->HEX:%8x  DEC:%8d\n",superblock->RootDirReg.dataPtr[3],superblock->RootDirReg.dataPtr[3]);
	printf("\n ***********************************\n");

}

char * getSuperblock_id(void)
{
	if(superblock == NULL)
		initSuperblock();

	return superblock->Id;
}

WORD getSuperblockVersion(void)
{
	if(superblock == NULL)
		initSuperblock();

	return superblock->Version;
}

WORD getSuperBlocksize(void)
{
	if(superblock == NULL)
		initSuperblock();

	return superblock->SuperBlockSize;
}

DWORD getDisksize(void)
{
	if(superblock == NULL)
		initSuperblock();

	return superblock->DiskSize;
}

DWORD getNofblocks(void)
{
	if(superblock == NULL)
		initSuperblock();

	return superblock->NofBlocks;
}

DWORD getBlocksize(void)
{
	if(superblock == NULL)
		initSuperblock();

	return superblock->BlockSize;
}

char * getReserved(void)
{
	if(superblock == NULL)
		initSuperblock();

	return superblock->Reserved;
}

t2fs_record * getBitmapreg(void)
{
	if(superblock == NULL)
		initSuperblock();

	return &superblock->BitMapReg;
}

t2fs_record * getRootdirreg(void)
{
	if(superblock == NULL)
		initSuperblock();

	return &superblock->RootDirReg;
}


// Carrega um bloco do disco (equivale a 4 setores)
char * loadBlock(unsigned int block)
{
	char * buffer;
	//uint8_t * castbuffer;
	int read;
	int i;

	// Não é possível carregar o bloco
	if(block < 0)
	{	
		printf("Bloco invalido!\n");
		return NULL;
	}


	// Aloca área para os bytes lidos do disco
	buffer = (char*) malloc(BLOCK_SIZE);

	// Carrega setores (block*4+1) - (block*4+2) - (block*4+3) - (block*4+4)
	for(i = 1; i <= 4 ; i++)
	{
		read = read_sector(block*4+i, &buffer[SECTOR_SIZE*(i-1)]);
		if(read != NOT_ERROR)
			error_read(block);
	}

	return buffer;	
}

int writeBlock(unsigned int block, char * buffer)
{
	int read;
	int i;

	// Não é possível carregar o bloco
	if(block < 0)
	{	
		printf("Bloco invalido!\n");
		return -1;
	}

	// Grava nos setores setores (block*4+1) - (block*4+2) - (block*4+3) - (block*4+4)
	for(i = 1; i <= 4 ; i++)
	{
		read = write_sector(block*4+i, &buffer[SECTOR_SIZE*(i-1)]);
		if(read != NOT_ERROR)
		{
			error_read(block);
			return -1;	
		}
	}

	return 1;
}

int writeRecordsBlock(unsigned int block, t2fs_record * record)
{

	char * buffer;
	int i;
	buffer = (char*) malloc(BLOCK_SIZE);

	if(block < 0)
	{	
		printf("Bloco invalido!\n");
		return -1;
	}


	for(i=0; i < BLOCK_SIZE / RECORD_SIZE ; i++)
	{
		memcpy(buffer+RECORD_SIZE*i, &record[i].TypeVal, 1);
		memcpy(buffer+1+RECORD_SIZE*i, &record[i].name, 31);
		memcpy(buffer+32+RECORD_SIZE*i, &record[i].blocksFileSize,4);
		memcpy(buffer+36+RECORD_SIZE*i, &record[i].bytesFileSize, 4);
		memcpy(buffer+40+RECORD_SIZE*i, &record[i].dataPtr, 16);
		memcpy(buffer+56+RECORD_SIZE*i, &record[i].singleIndPtr, 4);
		memcpy(buffer+60+RECORD_SIZE*i, &record[i].doubleIndPtr, 4);
	}

	writeBlock(block,buffer);

	return 1;

}

int writeIndexBlock(unsigned int block, DWORD * indexBlock)
{
 
        char * buffer;
        int i;
        buffer = (char*) malloc(BLOCK_SIZE);

	
 
        for(i=0; i < BLOCK_SIZE / sizeof(DWORD) ; i++)
                memcpy(buffer+sizeof(DWORD)*i, &indexBlock[i], sizeof(DWORD));
 
        writeBlock(block,buffer);

	return 1;

}

// Desaloca bloco de dados do disco
int deallocateDataBlock(unsigned int block)
{
	setBitOff(block);

	return 0;
}

// Desaloca bloco de records
int deallocateRecordBlock(unsigned int block)
{
	setBitOff(block);

	return 0;
}

void error_read(unsigned int sector)
{
	printf("It's impossible to read sector %d.\n",sector);
	exit(0);
}

void error_write(unsigned int sector)
{
	printf("It's impossible to write sector %d.\n",sector);
	exit(0);
}

int allocateIndexBlock(unsigned int block)
{
	int i;

	if (block >= NOF_BLOCKS)
 		return -1;
  
	setBitOn(block);

	DWORD * indexBlock;

	indexBlock = loadIndexBlock(block);

	// Demais ponteiros ficam zerados
	for(i=0; i < BLOCK_SIZE / sizeof(DWORD) ; i++)
		indexBlock[i] = -1;

	if(!(writeIndexBlock(block, indexBlock)))
	{
		free(indexBlock);
		setBitOff(block);
		return -1;
	}
	return 0;

}

// Aloca bloco para records
int allocateRecordsBlock(unsigned int block)
{
	int i;

	if (block >= NOF_BLOCKS)
		return -1;
  
	setBitOn(block);

	t2fs_record * recordsBlock;

	recordsBlock = loadRecordsBlock(block);

	for(i=0; i < BLOCK_SIZE / sizeof(t2fs_record) ; i++)
		recordsBlock[i].TypeVal = 0;

	if(!(writeRecordsBlock(block, recordsBlock)))
	{
		free(recordsBlock);
		setBitOff(block);
		return -1;
	}
	return 0;

}

// Aloca bloco para dados
int allocateDataBlock(unsigned int block)
{
	int i;

	if (block >= NOF_BLOCKS)
		return -1;
  
	setBitOn(block);

	char * dataBlock = (char*) malloc(BLOCK_SIZE);
	
	for(i=0; i < BLOCK_SIZE / sizeof(char) ; i++)
		dataBlock[i] = 0;

	if(!(writeBlock(block, dataBlock)))
	{
		free(dataBlock);
		setBitOff(block);
		return -1;
	}
	return 0;

}

void setSuperblockDataPtr0(DWORD block)
{
	if(superblock == NULL)
		initSuperblock();

	superblock->RootDirReg.dataPtr[0] = block;

	writeSuperblock();

}

void setSuperblockDataPtr1(DWORD block)
{
	if(superblock == NULL)
		initSuperblock();

	superblock->RootDirReg.dataPtr[1] = block;

	writeSuperblock();

}

void setSuperblockDataPtr2(DWORD block)
{
	if(superblock == NULL)
		initSuperblock();

	superblock->RootDirReg.dataPtr[2] = block;

	writeSuperblock();

}

void setSuperblockDataPtr3(DWORD block)
{
	if(superblock == NULL)
		initSuperblock();

	superblock->RootDirReg.dataPtr[3] = block;

	writeSuperblock();

}

void setSuperblockSingleIndPtr(DWORD block)
{
	if(superblock == NULL)
		initSuperblock();

	superblock->RootDirReg.singleIndPtr = block;

	writeSuperblock();

}

void setSuperblockDoubleIndPtr(DWORD block)
{
	if(superblock == NULL)
		initSuperblock();

	superblock->RootDirReg.doubleIndPtr = block;

	writeSuperblock();
}

void setSuperblockFileSize(DWORD bytes, DWORD blocks)
{
	if(superblock == NULL)
		initSuperblock();

	superblock->RootDirReg.bytesFileSize = bytes;
	superblock->RootDirReg.blocksFileSize = blocks;

	writeSuperblock();
}

void writeSuperblock(void)
{
	char * buffer;
	int write;
	unsigned int superblock_sector = 0;

	// Aloca área para os bytes lidos do disco
	buffer = (char*) malloc(SECTOR_SIZE);

	memcpy(buffer, &superblock->Id, 4);
	memcpy(buffer+4, &superblock->Version, 2);
	memcpy(buffer+6, &superblock->SuperBlockSize, 2);
	memcpy(buffer+8, &superblock->DiskSize, 4);
	memcpy(buffer+12, &superblock->NofBlocks, 4);
	memcpy(buffer+16, &superblock->BlockSize, 4);
	memcpy(buffer+20, &superblock->Reserved, 108);
	memcpy(buffer+128, &superblock->BitMapReg, 64);
	memcpy(buffer+192, &superblock->RootDirReg, 64);


	// Grava superbloco no disco
	write = write_sector(superblock_sector, buffer);

	if(write != NOT_ERROR)
		error_write(superblock_sector);

}








