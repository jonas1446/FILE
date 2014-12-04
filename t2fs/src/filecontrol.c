#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <t2fs.h>
#include <apidisk.h>
#include <stdint.h>
#include <diskblocks.h>
#include <filecontrol.h>

#define MAX_OPEN_FILES 20

/** bitmap global **/
uint8_t * bitmap = NULL;

// Tabela de descritores de arquivos abertos
t2fs_tdaa tdaa[MAX_OPEN_FILES];
t2fs_taap* taap = NULL;

char* curaddr = "/"; //Diretório corrente; inicia na raiz
char* curupaddr = "/"; //Diretório pai do corrente; inicia na raiz

void setCurDir(char* addr);
char* getCurDir(void);
void setCurUpDir(char* addr);
char* getCurUpDir(void);



// Conta quantos arquivos abertos
unsigned int openedFilesInTDAA = 0;
unsigned int TAAPHandle = 0;
freeHandles * freeTAAPHandles = NULL;
// Indice de entradas livres da tdaa
unsigned int tdaaFreeEntry[MAX_OPEN_FILES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


/* BITMAP */
void loadBitmap(void);
int writeBitmap(void);
void setBitOn(unsigned int block);
void setBitOff(unsigned int block);
int bitStatus(unsigned int block);
unsigned int findFreeBlock();
int areThereFreeBlocks(unsigned int numberOfBlocks);
void printBitmap();

/** Funções de manipulação da TDAA **/
t2fs_file insertFileTDAA(t2fs_record * fileRecord, DWORD fileBlock);
int canOpenMoreFiles();
void printOpenedFiles();
void printTAAP();
t2fs_record * getTDAARecord(t2fs_file handle);
DWORD getTDAABlock(t2fs_file handle);
unsigned int * getTAAPcurrentPointer(t2fs_file handle);
int removeFileTDAA(t2fs_file handle);

t2fs_file isNewFile(t2fs_record* fileRecord);

void insertNewFreeHandle(t2fs_file handle);
int removeTAAPFile(t2fs_file handle);
t2fs_file addProcessTAAP(t2fs_file TDAAEntry);
int removeFileTAAP(t2fs_file handle);
t2fs_file insertFileTAAP(t2fs_record * fileRecord, DWORD fileBlock);


/** TDAA **/
// Insere arquivo na lista de arquivos abertos
t2fs_file insertFileTAAP(t2fs_record * fileRecord, DWORD fileBlock)
{

	int i;
	t2fs_file handle = -1;
	t2fs_file freeEntry;
	t2fs_file currentFreeHandle;


	freeEntry = isNewFile(fileRecord);
	if (freeEntry  == -1)
	{
		if(canOpenMoreFiles() == -1)
			return handle;

		for(i=0;i<MAX_OPEN_FILES;i++)
		{
			if(!tdaaFreeEntry[i])
			{
				if(taap == NULL)
				{
					taap = (t2fs_taap*) malloc (sizeof(t2fs_taap));
					taap->nextFile = NULL;
					taap->currentPointer = 0;
					taap->TDAAEntry = i;
					taap->handle = TAAPHandle;
					currentFreeHandle = TAAPHandle;
					TAAPHandle++;
					
				}
				else
					currentFreeHandle = addProcessTAAP(i);

				freeEntry = i;
				i = MAX_OPEN_FILES;
			}

		}

		tdaaFreeEntry[freeEntry] = 1;

		tdaa[freeEntry].numberOfInstances = 1;
		tdaa[freeEntry].block = fileBlock;
		tdaa[freeEntry].record.TypeVal = (fileRecord->TypeVal);
		strcpy(tdaa[freeEntry].record.name,(fileRecord->name));
		tdaa[freeEntry].record.blocksFileSize = fileRecord->blocksFileSize;
		tdaa[freeEntry].record.bytesFileSize = fileRecord->bytesFileSize;
		tdaa[freeEntry].record.dataPtr[0] = (fileRecord->dataPtr[0]);
		tdaa[freeEntry].record.dataPtr[1] = (fileRecord->dataPtr[1]);
		tdaa[freeEntry].record.dataPtr[2] = (fileRecord->dataPtr[2]);
		tdaa[freeEntry].record.dataPtr[3] = (fileRecord->dataPtr[3]);
		tdaa[freeEntry].record.singleIndPtr = (fileRecord->singleIndPtr);
		tdaa[freeEntry].record.doubleIndPtr = (fileRecord->doubleIndPtr);

		openedFilesInTDAA++;

		// Retorna handle
	}
	else
	{
		currentFreeHandle = addProcessTAAP(freeEntry);
		tdaa[freeEntry].numberOfInstances++;

	}

	handle =  currentFreeHandle;
		
	return handle;

}

// Verifica se é possível abrir mais um arquivo
int canOpenMoreFiles()
{
	if(openedFilesInTDAA == MAX_OPEN_FILES)
		return -1;
	else
		return 0;
}


void  printBits(size_t const size, void const * const ptr){
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            printf("%u", byte);
        }
    }

}


// Remove arquivo da TAAP
int removeFileTAAP(t2fs_file handle)
{
	t2fs_taap * fileToBeClosed;

	fileToBeClosed = taap;
	int tdaaEntry;

	if(openedFilesInTDAA > 0)
	{
		while(fileToBeClosed->nextFile != NULL && fileToBeClosed->handle != handle)
			fileToBeClosed = (t2fs_taap*) fileToBeClosed->nextFile; 

		if (fileToBeClosed->handle != handle)
			return -1;
		else
		{

			tdaaEntry = removeTAAPFile(fileToBeClosed->handle);
			if( handle == TAAPHandle)
				TAAPHandle--;
			else
				insertNewFreeHandle(fileToBeClosed->handle);

			tdaa[tdaaEntry].numberOfInstances--;
			if (tdaa[tdaaEntry].numberOfInstances == 0)
				removeFileTDAA(tdaaEntry);

			return 0;

		}
		
	}

	return -1;

}


int removeFileTDAA(t2fs_file handle)
{
	// Verifica se pode remover
	if(openedFilesInTDAA > 0  && tdaaFreeEntry[handle] == 1)
	{
		tdaaFreeEntry[handle] = 0;
		openedFilesInTDAA--;

		if(openedFilesInTDAA == 0){
			

			if(freeTAAPHandles != NULL)
			{
				free(freeTAAPHandles);
				freeTAAPHandles = NULL;

			}

			free(taap);	
			taap = NULL;
		}
			

		return 0;
	}
	else
		return -1;
}

// Retorna record de um handle
t2fs_record * getTDAARecord(t2fs_file handle)
{
	t2fs_taap* lastFile;

	lastFile = taap;

	while(lastFile->nextFile != NULL && lastFile->handle != handle)
		lastFile = (t2fs_taap*) lastFile->nextFile;

	if (lastFile->handle != handle)
		return -1;
	else
		return  &(tdaa[lastFile->TDAAEntry].record);
}
// Retorna bloco de um handle
DWORD getTDAABlock(t2fs_file handle)
{

	t2fs_taap* lastFile;

	lastFile = taap;

	while(lastFile->nextFile != NULL && lastFile->handle != handle)
		lastFile = (t2fs_taap*) lastFile->nextFile;

	if (lastFile->handle != handle)
		return -1;
	else
		return  tdaa[lastFile->TDAAEntry].block;
}

// Retorna record de um handle
unsigned int * getTAAPcurrentPointer(t2fs_file handle)
{
	t2fs_taap* lastFile;

	lastFile = taap;

	while(lastFile->nextFile != NULL && lastFile->handle != handle)
		lastFile = lastFile->nextFile;

	if (lastFile->handle != handle)
		return -1;
	else
		return &lastFile->currentPointer;
}


void printOpenedFiles()
{
	int i;
	// printa o tdaaFreeEntry
	printf("tdaaFreeEntry: ");
	for(i=0;i<MAX_OPEN_FILES;i++)
		printf(" %d", tdaaFreeEntry[i]);

	printf("\n\n\n");


	printf("tdaa: ");
	// printa tdaa
	for(i=0;i<MAX_OPEN_FILES;i++)
	{
			printf("Block: %d\n", tdaa[i].block);
			//printf("currentPointer: %d\n", tdaa[i].currentPointer);
			printf("TypeVal: %d\n", tdaa[i].record.TypeVal);
			printf("Name: %s\n", tdaa[i].record.name);
			printf("bytesFileSize: %d\n", tdaa[i].record.bytesFileSize);
			printf("numberOfInstances: %d\n", tdaa[i].numberOfInstances);

	}

}

void printTAAP()
{
	printf("\n\n");

	t2fs_taap* currentFile;


	currentFile = taap;

	printf("\nTAAP:\n ");

		
	printf("Handle: %d\n", currentFile->handle);
	printf("currentPointer: %d\n", currentFile->currentPointer);

	printf("Name: %s\n\n", tdaa[currentFile->TDAAEntry].record.name);

	

	while(currentFile->nextFile != NULL){


		currentFile = (t2fs_taap *) currentFile->nextFile;	

		printf("Handle: %d\n", currentFile->handle);
		printf("currentPointer: %d\n", currentFile->currentPointer);
		printf("Name: %s\n\n", tdaa[currentFile->TDAAEntry].record.name);
		
	}

}



/** BITMAP **/

void loadBitmap(void)
{

	int bytesFileSize = getBitmapreg()->bytesFileSize;

	// bitmap com n blocos de tamanho
	bitmap = (uint8_t*) malloc(bytesFileSize);
	char * buffer = (char*) malloc(BLOCK_SIZE);

	buffer = loadBlock(getBitmapreg()->dataPtr[0]);
	 // buffer = loadBlock(2);
	// Copia os 128 (bytesFileSize) primeiros bytes do bloco
	memcpy(bitmap,buffer,bytesFileSize);

	// 1111 1111 0000 0111
	// Blocos
	// 7 6 5 4 3 2 1 0 | 15 14 13 12 11 10 9 8

	free(buffer);

}

void printBitmap()
{
	int j;

	if(bitmap == NULL)
		loadBitmap();

	
	
		
		          printf("\n ********** BITMAP MAP  ************\n");
                  printf("\n ****[0] -> b7 b6 b5 b4 b3 b2 b1****\n");
                  printf("\n ****[1] -> .............. b9 b8****\n");
                  printf("\n ***********************************\n");

                  for(j=0;j<128;j++)
                  {

                    printf( "[%3d] -> ", j);

                    printBits(sizeof(bitmap[j]),&bitmap[j]);

                    printf( "| ");

                    if(j%4==3 && j != 0)
                     printf( "\n");

                  }

                  printf("\n ***********************************\n");
		
		


}

// Escreve bitmap no disco
int writeBitMap()
{
	if(writeBlock(getBitmapreg()->dataPtr[0], bitmap) == -1)
		return -1;
	else
		return 0;
}

// Liga um determinado bit do bitmap
void setBitOn(unsigned int block)
{

	// bit alterado
	int bitOffset = block%BITS_PER_WORD;
	// caracter alterado
	int wordOffset = block/BITS_PER_WORD;

	if(block < 0)
		return;

	if(bitmap == NULL)
		loadBitmap();

	bitmap[wordOffset] |= (1 << bitOffset);

	writeBitMap();

}

// Desliga um determinado bit do bitmap
void setBitOff(unsigned int block)
{

	// bit alterado
	int bitOffset = block%BITS_PER_WORD;
	// caracter alterado
	int wordOffset = block/BITS_PER_WORD;

	if(block < 0)
		return;

	if(bitmap == NULL)
		loadBitmap();

	bitmap[wordOffset] &= ~(1 << bitOffset);

	writeBitMap();
}

// Verifica se bloco está livre ou ocupado
// Retorna 1 se está ocupado e 0 se está livre
int bitStatus(unsigned int block)
{
	// bit verificado
	int bitOffset = block%BITS_PER_WORD;
	// caracter verificado
	int wordOffset = block/BITS_PER_WORD;
   
	if(bitmap == NULL)
		loadBitmap();

	if(block < 0 || block > NOF_BLOCKS)
		return -1;

	if(bitmap == NULL)
		loadBitmap();
	
	return (bitmap[wordOffset] & (1 << bitOffset));
}

// Procura pelo primeiro bloco livre
unsigned int findFreeBlock()
{

	unsigned int i;

	if(bitmap == NULL)
		loadBitmap();

	for(i=0;i<NOF_BLOCKS;i++)
		if(!bitStatus(i))
			return i;

	// Não achou nenhum bloco livre
	return -1;

}

// Retorna 0 se não existem numberOfBlocks livres
// Retorna 1 se existem numberOfBlocks livres
int areThereFreeBlocks(unsigned int numberOfBlocks)
{
	int i;
	int count = 0;

	if( numberOfBlocks > NOF_BLOCKS || numberOfBlocks < 0 )
		return -1;

	for(i=0;i<NOF_BLOCKS;i++)
	{
		if(!bitStatus(i))//Bloco está alocado?
			count++;
		
		// Existem numberOfBlocks blocos livres
		if(count == numberOfBlocks)
			return 1;
	}

	return 0;

}

t2fs_file isNewFile(t2fs_record* fileRecord)
{
	int i;
	

	for(i = 0; i<MAX_OPEN_FILES; i++)
	{
		
		if(tdaaFreeEntry[i] != 0){

			if(!strcmp(tdaa[i].record.name,(fileRecord->name))){
				return i;
			}	
		}
			
	}
	return -1;
}

t2fs_file addProcessTAAP(t2fs_file TDAAEntry)
{
	t2fs_taap* lastFile;
	t2fs_taap* newFile;
	t2fs_file currentFreeHandle;


	lastFile = taap;

	while(lastFile->nextFile != NULL)
		lastFile = (t2fs_taap*) lastFile->nextFile;

	newFile = (t2fs_taap*) malloc (sizeof(t2fs_taap));

	newFile->nextFile = NULL;
	newFile->currentPointer = 0;
	newFile->TDAAEntry = TDAAEntry;

	lastFile->nextFile = newFile;

	if (freeTAAPHandles  != NULL)
	{
		currentFreeHandle = freeTAAPHandles->handle;
		if (freeTAAPHandles->next == NULL)
		{
			free(freeTAAPHandles);
			freeTAAPHandles = NULL;
		}
		else
		{
			freeTAAPHandles = freeTAAPHandles->next;
			freeTAAPHandles->previous = NULL;
		}
			
		
		newFile->handle = currentFreeHandle;
	}
	else{

		currentFreeHandle = TAAPHandle;
		newFile->handle = TAAPHandle;
		TAAPHandle++;
	}
		
	return currentFreeHandle;

}


int removeTAAPFile(t2fs_file handle){
	
	t2fs_taap* fileToBeRemoved;
	t2fs_taap* fileInTAAP;

	
	unsigned int freeEntry;

	if (taap->nextFile == NULL)
	{
		freeEntry = taap->TDAAEntry;
		taap = NULL;

	}
	else
	if (taap->handle == handle)
	{
		 freeEntry = taap->TDAAEntry;
		 taap = (t2fs_taap*) taap->nextFile;
	}
	else
	{

		fileToBeRemoved = taap;
		fileInTAAP = (t2fs_taap*) fileToBeRemoved->nextFile;

		while(fileInTAAP->handle != handle){
			fileToBeRemoved = (t2fs_taap*) fileToBeRemoved->nextFile;
			fileInTAAP = (t2fs_taap*) fileToBeRemoved->nextFile;
		}
			

	
		freeEntry = fileInTAAP->TDAAEntry;

		if ( fileInTAAP->nextFile == NULL)	
			fileToBeRemoved = NULL;
		else
			fileToBeRemoved->nextFile = fileInTAAP->nextFile;

			

	}

	return freeEntry;

}

void insertNewFreeHandle(t2fs_file handle)
{
	freeHandles* newHandle;
        freeHandles* nextHandle;

	if (freeTAAPHandles == NULL)
	{
		freeTAAPHandles = (freeHandles*) malloc (sizeof(freeHandles));
		freeTAAPHandles->handle = handle;
		freeTAAPHandles->next = NULL;
		freeTAAPHandles->previous = NULL;

	}
	else
	{
		newHandle = (freeHandles*) malloc (sizeof(freeHandles));
		
		if (freeTAAPHandles->next == NULL)
		{
			if (freeTAAPHandles->handle > handle)
			{
				freeTAAPHandles->previous = newHandle;
				freeTAAPHandles->previous->handle = handle;
				freeTAAPHandles->previous->previous = NULL;
				freeTAAPHandles->previous->next = freeTAAPHandles;
				freeTAAPHandles = newHandle;
			
			}
			else
			{

				freeTAAPHandles->next = newHandle;
				freeTAAPHandles->next->handle = handle;
				freeTAAPHandles->next->previous = freeTAAPHandles;
				freeTAAPHandles->next = newHandle;

			}
		}
		else
		if (freeTAAPHandles->handle > handle)
		{
			freeTAAPHandles->previous = newHandle;
			freeTAAPHandles->previous->handle = handle;
			freeTAAPHandles->previous->previous = NULL;
			freeTAAPHandles->previous->next = freeTAAPHandles;
			freeTAAPHandles = newHandle;

		}
		else 
		{

			nextHandle = freeTAAPHandles;

			while(nextHandle ->next != NULL && nextHandle ->handle < handle)
				nextHandle  = nextHandle ->next;

			if (nextHandle ->next == NULL)
			{
				nextHandle ->next = newHandle;
				nextHandle ->next->previous = nextHandle;
				nextHandle ->next->next = NULL;
			}
			else
		
			{
				nextHandle->previous->next = newHandle;
				nextHandle->previous->next->previous = nextHandle->previous;
				nextHandle->previous = newHandle;
				nextHandle->previous->next = nextHandle ;
			
				/*nextHandle->next->previous = newHandle;
				nextHandle->next->previous->next = nextHandle->next;
				nextHandle->next = newHandle;
				nextHandle->next->previous = nextHandle;*/

			}
		

		}

	}

}

void setCurDir(char* addr){//Set do Diretyório corrente
 curaddr = addr; 
}
char* getCurDir(){
 char* dir = curaddr;
 return dir;
}

void setCurUpDir(char* addr){//Set do Pai do diretório corrente
 curupaddr = addr; 
}
char* getCurUpDir(){
 char* dir = curupaddr;
 return dir;
}
