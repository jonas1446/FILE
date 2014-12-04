#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <t2fs.h>
#include <apidisk.h>
#include <stdint.h>
#include <diskblocks.h>
#include <filecontrol.h>

char *authors;

/** Informa a identificação dos desenvolvedores do T2FS. */
int identify2 (char *name, int size)
{
	int i;
	for(i=0;i<size;i++) {
		if(name[i] < 32 || name[i] > 122) return -1;
	}	
	//printf("%s", authors); 
	authors = malloc(size);
	memcpy(authors, name, size);
	return 0;
}

FILE2 create2 (char *filename)
{
	t2fs_record * directoryRecord = NULL;
	// Bloco onde existe entrada vazia
	t2fs_record * fileRecord = NULL;
	t2fs_record * sameNameFileRecord = NULL;
	char * address = NULL;
	char * fileName = NULL;
	int invalidAddress = 0;
	unsigned int directoryBlock;
	unsigned int recordBlock;
	unsigned int freeBlock;
	BOOL isTheSameFile = FALSE;
	BOOL isThereSameNameFile = FALSE;
	
	setName(&filename);
 	
	invalidAddress = getNameAddress(filename, &fileName, &address);
 
	// Impossível criar arquivo
	if(invalidAddress == -1)	
		return -1;
	// fileName => nome do arquivo
	// address => endereço do arquivo
	// Procura pelo diretório pai do arquivo que se quer incluir
	directoryRecord = findRecord(address, TYPEVAL_DIRETORIO, &directoryBlock);
	
	// Diretório pai encontrado
	if(directoryRecord == NULL)
		return -1;	
	

	sameNameFileRecord = findRecord(filename, TYPEVAL_REGULAR, &freeBlock);

		if (sameNameFileRecord != NULL)
		isThereSameNameFile = TRUE;

	if(directoryRecord->dataPtr[0] != -1 || 
		directoryRecord->dataPtr[1] != -1  ||
		directoryRecord->dataPtr[2] != -1  ||
		directoryRecord->dataPtr[3] != -1  ||
		directoryRecord->singleIndPtr != -1 || 
		directoryRecord->doubleIndPtr != -1 || 
		isThereSameNameFile == TRUE)
	{
		// Carrega bloco para achar próxima posição disponível (lista de records)
		fileRecord = findEmptyRecord(directoryRecord->dataPtr[0], fileName, &isTheSameFile);
	
		// Offset dentro do bloco para posição disponível
		if(fileRecord != NULL) {
			recordBlock = directoryRecord->dataPtr[0];
		}
		else
		{
			//free(recordsBlock);

			// dataPtr[1] não está vazio
			if(directoryRecord->dataPtr[1] != -1  || 
				directoryRecord->dataPtr[2] != -1  || 
				directoryRecord->dataPtr[3] != -1  || 
				directoryRecord->singleIndPtr != -1 || 
				directoryRecord->doubleIndPtr != -1)
			{
				// Carrega bloco do dataPtr[1]
				fileRecord = findEmptyRecord(directoryRecord->dataPtr[1], fileName, &isTheSameFile);

				// Offset dentro do bloco para posição disponível
				if(fileRecord != NULL)
				{
					recordBlock = directoryRecord->dataPtr[1];
				}
				else
				{

					if(directoryRecord->dataPtr[2] != -1  || 
						directoryRecord->dataPtr[3] != -1  || 
						directoryRecord->singleIndPtr != -1 || 
						directoryRecord->doubleIndPtr != -1)
					{
						// Carrega bloco do dataPtr[2]
						fileRecord = findEmptyRecord(directoryRecord->dataPtr[2], fileName, &isTheSameFile);

						// Offset dentro do bloco para posição disponível
						if(fileRecord != NULL)
						{
							recordBlock = directoryRecord->dataPtr[1];
						}
						else
						{
							if(directoryRecord->dataPtr[3] != -1  || 
								directoryRecord->singleIndPtr != -1 || 
								directoryRecord->doubleIndPtr != -1)
							{
								// Carrega bloco do dataPtr[3]
								fileRecord = findEmptyRecord(directoryRecord->dataPtr[3], fileName, &isTheSameFile);

								// Offset dentro do bloco para posição disponível
								if(fileRecord != NULL)
								{
									recordBlock = directoryRecord->dataPtr[1];
								}
								else
								{
									if(directoryRecord->singleIndPtr != -1 || 
										directoryRecord->doubleIndPtr != -1)
									{

										fileRecord = EmptyRecordSingleIndPtr(directoryRecord->singleIndPtr, &recordBlock, fileName, &isTheSameFile);

										if (fileRecord == NULL)
										{

											if (directoryRecord->doubleIndPtr != -1)
											{

												fileRecord = EmptyRecordDoubleIndPtr(directoryRecord->doubleIndPtr, &recordBlock, fileName, &isTheSameFile);
												if (fileRecord == NULL)
													return -1;
											}
											else
											{
												 if(!areThereFreeBlocks(4))
													return -1;
	
					  							 freeBlock = findFreeBlock();
					   							 allocateIndexBlock(freeBlock);
					   							 directoryRecord->doubleIndPtr = freeBlock;

												if (directoryBlock != -1)
					   								writeRecord(directoryBlock, directoryRecord);
												else
													setSuperblockDoubleIndPtr(freeBlock);
					   							// writeRecordsBlock(directoryBlock, directoryRecord);

					   					 		fileRecord = EmptyRecordDoubleIndPtr(directoryRecord->doubleIndPtr, &recordBlock, fileName, &isTheSameFile);
											}
					
										}
							
					
									}
									else
									{			
										 if(!areThereFreeBlocks(3))
											return -1;
	
					  					 freeBlock = findFreeBlock();
					   					 allocateIndexBlock(freeBlock);
					   					 directoryRecord->singleIndPtr = freeBlock;

										if (directoryBlock != -1)
					   						writeRecord(directoryBlock, directoryRecord);
										else
											setSuperblockSingleIndPtr(freeBlock);
					   				        // writeRecordsBlock(directoryBlock, directoryRecord);

					   					fileRecord = EmptyRecordSingleIndPtr(directoryRecord->singleIndPtr, &recordBlock, fileName, &isTheSameFile);

									}
								}
							}
							else
							{	
								// Se não possui 
								if(!areThereFreeBlocks(2))
									return -1;
			
								// Aloca bloco para records
								freeBlock = findFreeBlock();
								allocateRecordsBlock(freeBlock);
								directoryRecord->dataPtr[3] = freeBlock;

								if (directoryBlock != -1)
					   				writeRecord(directoryBlock, directoryRecord);
								else
									setSuperblockDataPtr1(freeBlock);

								fileRecord = findEmptyRecord(directoryRecord->dataPtr[3], fileName, &isTheSameFile);
								recordBlock = directoryRecord->dataPtr[3];
								//writeRecordsBlock(directoryBlock, directoryRecord);
							}
						}				
					}
					else
					{	
						// Se não possui 
						if(!areThereFreeBlocks(2))
							return -1;
	
						// Aloca bloco para records
						freeBlock = findFreeBlock();
						allocateRecordsBlock(freeBlock);
						directoryRecord->dataPtr[2] = freeBlock;

						if (directoryBlock != -1)
			   				writeRecord(directoryBlock, directoryRecord);
						else
							setSuperblockDataPtr1(freeBlock);

						fileRecord = findEmptyRecord(directoryRecord->dataPtr[2], fileName, &isTheSameFile);
						recordBlock = directoryRecord->dataPtr[2];
						//writeRecordsBlock(directoryBlock, directoryRecord);
					}

				}

			}
			else
			{	
				// Se não possui 
				if(!areThereFreeBlocks(2))
					return -1;
				
				// Aloca bloco para records
				freeBlock = findFreeBlock();
				allocateRecordsBlock(freeBlock);
				directoryRecord->dataPtr[1] = freeBlock;

				if (directoryBlock != -1)
	   				writeRecord(directoryBlock, directoryRecord);
				else
					setSuperblockDataPtr1(freeBlock);

				fileRecord = findEmptyRecord(directoryRecord->dataPtr[1], fileName, &isTheSameFile);
				recordBlock = directoryRecord->dataPtr[1];
				//writeRecordsBlock(directoryBlock, directoryRecord);
				

					

			}
			

		}
			
		
	}
	else
	{
	
		if(!areThereFreeBlocks(2))
			return -1;
	
		// Aloca bloco para records
		freeBlock = findFreeBlock();
	
		allocateRecordsBlock(freeBlock);
		directoryRecord->dataPtr[0] = freeBlock;


		if (directoryBlock != -1)
	   		writeRecord(directoryBlock, directoryRecord);
		else
			setSuperblockDataPtr0(freeBlock);

		fileRecord = findEmptyRecord(directoryRecord->dataPtr[0], fileName, &isTheSameFile);
		recordBlock = directoryRecord->dataPtr[0];
		//writeRecordsBlock(directoryBlock, directoryRecord);
		
	}

	if(isTheSameFile == FALSE)
	{
		directoryRecord->bytesFileSize += RECORD_SIZE;
		directoryRecord->blocksFileSize = (directoryRecord->bytesFileSize - RECORD_SIZE)/BLOCK_SIZE +1;
	
		if(directoryBlock != -1)
			writeRecord(directoryBlock, directoryRecord);	
		else
			setSuperblockFileSize(directoryRecord->bytesFileSize, directoryRecord->blocksFileSize);
	}

	//emptyRecordBlock = findEmptyRecordsBlock(directoryRecord,&recordOffset, &recordBlock, fileName, directoryBlock);

	newFileRecord(fileName, fileRecord);
	
	/*if(fileRecord != NULL)
		return -1;*/

	writeNewFileRecord(recordBlock, fileRecord, filename);

	//printRecordBlock(emptyRecordBlock);
	
	return open2(filename);

}

void writeNewFileRecord (unsigned int recordBlock, t2fs_record* fileRecord, char* nome)
{
	t2fs_record* loadedBlock;
	unsigned int block;

	int recordOffset = 0;

	if (findRecord(nome, TYPEVAL_REGULAR, &block) != NULL)
	{
		loadedBlock = loadRecordsBlock(block);
		while((strcmp(fileRecord->name, loadedBlock[recordOffset].name)) || (loadedBlock[recordOffset].TypeVal != fileRecord->TypeVal))
			recordOffset++;
	}
	else
	{
		loadedBlock = loadRecordsBlock(recordBlock);
		while((loadedBlock[recordOffset].TypeVal == TYPEVAL_REGULAR) || (loadedBlock[recordOffset].TypeVal == TYPEVAL_DIRETORIO))
			recordOffset++;
	}

	memcpy(&loadedBlock[recordOffset].TypeVal, &(fileRecord->TypeVal), 1);
	memcpy(&loadedBlock[recordOffset].name, &(fileRecord->name), 31);
	memcpy(&loadedBlock[recordOffset].blocksFileSize, &(fileRecord->blocksFileSize),4);
	memcpy(&loadedBlock[recordOffset].bytesFileSize, &(fileRecord->bytesFileSize),4);
	memcpy(&loadedBlock[recordOffset].dataPtr, &(fileRecord->dataPtr),16);
	memcpy(&loadedBlock[recordOffset].singleIndPtr, &(fileRecord->singleIndPtr),4);
	memcpy(&loadedBlock[recordOffset].doubleIndPtr, &(fileRecord->doubleIndPtr),4);

	writeRecordsBlock(recordBlock, loadedBlock);
			
}

void writeRecord (unsigned int recordBlock, t2fs_record* fileRecord)
{
	t2fs_record* loadedBlock;

	loadedBlock = loadRecordsBlock(recordBlock);

	int recordOffset = 0;

	while((strcmp(fileRecord->name, loadedBlock[recordOffset].name)) || (loadedBlock[recordOffset].TypeVal != fileRecord->TypeVal))
	{
		recordOffset++;
	}

	//printf("Offset: %d", recordOffset);

	memcpy(&loadedBlock[recordOffset].TypeVal, &(fileRecord->TypeVal), 1);
	memcpy(&loadedBlock[recordOffset].name, &(fileRecord->name), 31);
	memcpy(&loadedBlock[recordOffset].blocksFileSize, &(fileRecord->blocksFileSize),4);
	memcpy(&loadedBlock[recordOffset].bytesFileSize, &(fileRecord->bytesFileSize),4);
	memcpy(&loadedBlock[recordOffset].dataPtr, &(fileRecord->dataPtr),16);
	memcpy(&loadedBlock[recordOffset].singleIndPtr, &(fileRecord->singleIndPtr),4);
	memcpy(&loadedBlock[recordOffset].doubleIndPtr, &(fileRecord->doubleIndPtr),4);

	writeRecordsBlock(recordBlock, loadedBlock);
			
}


// Remove record de um bloco de records
// Retorna -1 se o record é o último do bloco
int removeRecord(unsigned int recordBlock, t2fs_record * fileRecord)
{
	t2fs_record* loadedBlock;
	int i;

	loadedBlock = loadRecordsBlock(recordBlock);

	int recordOffset = 0;

	while((strcmp(fileRecord->name, loadedBlock[recordOffset].name)) || (loadedBlock[recordOffset].TypeVal != fileRecord->TypeVal))
	{
		recordOffset++;
	}

	loadedBlock[recordOffset].TypeVal = TYPEVAL_INVALIDO;

	writeRecordsBlock(recordBlock, loadedBlock);

	// Verifico se é o último record no bloco
	for(i=0;i<BLOCK_SIZE/RECORD_SIZE;i++)
	{
		if(loadedBlock[i].TypeVal == TYPEVAL_REGULAR || loadedBlock[i].TypeVal == TYPEVAL_DIRETORIO)
			return 0;
	}

	return -1;
			
}




// Aloca área do disco para dados e cria record para arquivo
t2fs_record * newFileRecord(char * name, t2fs_record * newFileRecord)
{
	unsigned int freeBlockNumber;

	// Procura por bloco livre para armazenar os dados
	freeBlockNumber = findFreeBlock();

	// Nenhum bloco livre (disco cheio)
	if(freeBlockNumber == -1)
		return NULL;

	newFileRecord->TypeVal = TYPEVAL_REGULAR;
	memcpy(&(newFileRecord->name),name,31);
	newFileRecord->blocksFileSize = 1;
	newFileRecord->bytesFileSize = 0;
	newFileRecord->dataPtr[0] = freeBlockNumber;
	newFileRecord->dataPtr[1] = -1;
	newFileRecord->dataPtr[2] = -1;
	newFileRecord->dataPtr[3] = -1;
	newFileRecord->singleIndPtr = -1;
	newFileRecord->doubleIndPtr = -1;

	// Aloca bloco de dados
	if(allocateDataBlock(freeBlockNumber) == -1)
	{
		free(newFileRecord);
		return NULL;
	}

	return newFileRecord;

}



// Procura pelo bloco onde será salvo o descritor do arquivo
///// VERIFICAR SE JÁ EXISTE O NOME
t2fs_record* findEmptyRecord(unsigned int block,  char * fileName , BOOL* isTheSameFile){

	t2fs_record * loadedBlock;
	int i = 0;
	
	loadedBlock = loadRecordsBlock(block);


	while(i < BLOCK_SIZE / RECORD_SIZE )
	{

		if( ! strcmp(loadedBlock[i].name,fileName))
		{
			*isTheSameFile = TRUE;
			removeBlocksFromFile(&loadedBlock[i]);
			loadedBlock[i].TypeVal = TYPEVAL_INVALIDO;
			writeRecordsBlock(block, loadedBlock);

		}



		// Se os nomes são iguais e tipo do descritor é igual (diretório ou arquivo), existe a pasta ou arquivo
		if(!((loadedBlock[i].TypeVal == TYPEVAL_REGULAR) || (loadedBlock[i].TypeVal == TYPEVAL_DIRETORIO))) 
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

// Desaloca todos os blocos que o arquivo possui
void removeBlocksFromFile(t2fs_record * fileRecord)
{
	int i, j;
	DWORD * indexBlock;
	DWORD * indexBlock2;
	
	// Desaloca blocos ponteiro direto
	if(fileRecord->dataPtr[0] != -1)
	{
		deallocateDataBlock(fileRecord->dataPtr[0]);
		fileRecord->dataPtr[0] = -1;
	}

	if(fileRecord->dataPtr[1] != -1)
	{
		deallocateDataBlock(fileRecord->dataPtr[1]);
		fileRecord->dataPtr[1] = -1;
	}

	if(fileRecord->dataPtr[2] != -1)
	{
		deallocateDataBlock(fileRecord->dataPtr[2]);
		fileRecord->dataPtr[2] = -1;
	}

	if(fileRecord->dataPtr[3] != -1)
	{
		deallocateDataBlock(fileRecord->dataPtr[3]);
		fileRecord->dataPtr[3] = -1;
	}
	
	// Desaloca blocos ponteiro indireto simples
	if(fileRecord->singleIndPtr != -1)
	{
		indexBlock = loadIndexBlock(fileRecord->singleIndPtr);
		i = 0;
		while(indexBlock[i] != -1 && i < BLOCK_SIZE/sizeof(DWORD))
		{
			deallocateDataBlock(indexBlock[i]);
			i++;
		}

		deallocateDataBlock(fileRecord->singleIndPtr);
	}


	// Desaloca blocos ponteiro indireto duplo
	if(fileRecord->doubleIndPtr != -1)
	{
		indexBlock = loadIndexBlock(fileRecord->doubleIndPtr);
		i = 0;
		j = 0;
		while(indexBlock[i] != -1 && i < BLOCK_SIZE/sizeof(DWORD))
		{
			indexBlock2 = loadIndexBlock(indexBlock[i]);
			
			while(indexBlock2[j] != -1 && j < BLOCK_SIZE/sizeof(DWORD))
			{
				deallocateDataBlock(indexBlock2[j]);
				j++;
			}

			deallocateDataBlock(indexBlock[i]);
			i++;
		}

		deallocateDataBlock(fileRecord->doubleIndPtr);

	}
		

}


// Remove bloco de record do diretório informado
void removeRecordBlockFromDirectory(DWORD recordBlock, t2fs_record * directoryRecord)
{
	int i, j;
	DWORD * indexBlock;
	DWORD * indexBlock2;
	
	// Desaloca bloco ponteiro direto
	if(directoryRecord->dataPtr[0] == recordBlock)
	{
		deallocateDataBlock(directoryRecord->dataPtr[0]);
		directoryRecord->dataPtr[0] = -1;
		return;
	}

	if(directoryRecord->dataPtr[1] != recordBlock)
	{
		deallocateDataBlock(directoryRecord->dataPtr[1]);
		directoryRecord->dataPtr[1] = -1;
		return;
	}

	if(directoryRecord->dataPtr[2] != recordBlock)
	{
		deallocateDataBlock(directoryRecord->dataPtr[2]);
		directoryRecord->dataPtr[2] = -1;
		return;
	}

	if(directoryRecord->dataPtr[3] != recordBlock)
	{
		deallocateDataBlock(directoryRecord->dataPtr[3]);
		directoryRecord->dataPtr[3] = -1;
		return;
	}

	// Desaloca blocos ponteiro indireto simples
	if(directoryRecord->singleIndPtr != recordBlock)
	{
		indexBlock = loadIndexBlock(directoryRecord->singleIndPtr);
		i = 0;
		while(indexBlock[i] != -1 && i < BLOCK_SIZE/sizeof(DWORD))
		{
			deallocateDataBlock(indexBlock[i]);
			i++;
		}

		deallocateDataBlock(directoryRecord->singleIndPtr);

		return;
	}


	// Desaloca blocos ponteiro indireto duplo
	if(directoryRecord->doubleIndPtr != recordBlock)
	{
		indexBlock = loadIndexBlock(directoryRecord->doubleIndPtr);
		i = 0;
		j = 0;
		while(indexBlock[i] != -1 && i < BLOCK_SIZE/sizeof(DWORD))
		{
			indexBlock2 = loadIndexBlock(indexBlock[i]);
			
			while(indexBlock2[j] != -1 && j < BLOCK_SIZE/sizeof(DWORD))
			{
				deallocateDataBlock(indexBlock2[j]);
				j++;
			}

			deallocateDataBlock(indexBlock[i]);
			i++;
		}

		deallocateDataBlock(directoryRecord->doubleIndPtr);

		return;

	}
		

}

t2fs_record* EmptyRecordSingleIndPtr(unsigned int block, unsigned int* recordBlock, char * fileName, BOOL* isTheSameFile){

	DWORD * loadedBlock;
	int i = 0;
	t2fs_record* record;
	int freeBlock;

	loadedBlock = loadIndexBlock(block);

	while (i != sizeof(loadedBlock) && loadedBlock[i] != -1){

		if ( (record = findEmptyRecord(loadedBlock[i], fileName, isTheSameFile)) != NULL){
			*recordBlock = loadedBlock[i];
			return record;	
		}
			
		else
			i++;
	}
	if (i != sizeof(loadedBlock))
	{
	  if(!areThereFreeBlocks(2))
		return NULL;
	  
	  freeBlock = findFreeBlock();
	  allocateRecordsBlock(freeBlock);
	  loadedBlock[i] = freeBlock;
          writeIndexBlock(block, loadedBlock);
	  
          record = findEmptyRecord(loadedBlock[i], fileName, isTheSameFile);
	  *recordBlock = loadedBlock[i];

	  return record;		
		
	}

	return NULL;
 
}

t2fs_record* EmptyRecordDoubleIndPtr(unsigned int block, unsigned int* recordBlock, char * fileName, BOOL* isTheSameFile){

	DWORD * loadedBlock;
	int i = 0;
	t2fs_record* record;
	int freeBlock;

	loadedBlock = loadIndexBlock(block);

	while (i != sizeof(loadedBlock) && loadedBlock[i] != -1){

		if ( (record = EmptyRecordSingleIndPtr(loadedBlock[i], recordBlock, fileName, isTheSameFile)) != NULL)
			return record;
		else
			i++;
	}
	
	if (i != sizeof(loadedBlock))
	{
	   if(!areThereFreeBlocks(3))
		return NULL;
		
	   freeBlock = findFreeBlock();
	   allocateIndexBlock(freeBlock);
	   loadedBlock[i] = freeBlock;
	   writeIndexBlock(block, loadedBlock);

	   record =  EmptyRecordSingleIndPtr(loadedBlock[i], recordBlock, fileName, isTheSameFile);

	   return record;
		
	}


	return NULL;
	
}

int getNameAddress(char * nome, char ** fileName, char ** address)
{
	int barPosition = 0;
	int i = 0;

	// Encontra a posição do último elemento '/'
	// i = tamanho vetor
	while(nome[i] != '\0')
	{
		if(nome[i] == '/')
			barPosition = i;
		i++;
	}

	*address = (char*) malloc(barPosition+1);
	*fileName = (char*) malloc(i-barPosition+1);
	
	// Copia 'nome' até a última '/' (caminho)

	// Arquivo está na raíz	
	if(barPosition == 0)
	{
		strncpy(*address,nome,1);
		*(*address+1) = '\0';

	}
	else	// Arquivo não está na raíz
	{
		strncpy(*address,nome,barPosition);
		*(*address+barPosition) = '\0';
	}
	
	// Copia 'nome' após o último '/'
	strncpy(*fileName,nome+barPosition+1,i-barPosition);
	*(*fileName+i-barPosition) = '\0';

	// Caminho inválido: /dir1/teste4-dir1/ (com barra no final)
	if(*fileName[0] == '\0')
		return -1;

	return 0;

}

void printRecordBlock(unsigned int block)
{
 int i;
	t2fs_record* loadedBlock;

	loadedBlock = loadRecordsBlock(block);

	for(i=0; i< BLOCK_SIZE / RECORD_SIZE ; i++)
	{
		printf("name: %s  type: %d\n", loadedBlock[i].name, loadedBlock[i].TypeVal);
		printf("sons: %d %d %d %d %d %d\n", loadedBlock[i].dataPtr[0], loadedBlock[i].dataPtr[1], loadedBlock[i].dataPtr[2], loadedBlock[i].dataPtr[3], loadedBlock[i].singleIndPtr, loadedBlock[i].doubleIndPtr);
/** 
		printf("\n%d\n\n",i);
		printf("TypeVal: %d\n",loadedBlock[i].TypeVal);
		printf("name: %s\n", loadedBlock[i].name);
		printf("blocksFileSize: %d\n",loadedBlock[i].blocksFileSize);
		printf("bytesFileSize: %d\n",loadedBlock[i].bytesFileSize);
		printf("dataPtr[0]: %d\n",loadedBlock[i].dataPtr[0]);
		printf("dataPtr[1]: %d\n",loadedBlock[i].dataPtr[1]);
	 	printf("dataPtr[2]: %d\n",loadedBlock[i].dataPtr[2]);
		printf("dataPtr[3]: %d\n",loadedBlock[i].dataPtr[3]);
		printf("singleIndPtr: %d\n",loadedBlock[i].singleIndPtr);
		printf("doubleIndPtr: %d\n",loadedBlock[i].doubleIndPtr);
*/	

	}

	free(loadedBlock);

}

void printDataBlock(unsigned int block)
{
 int i;
	char* loadedBlock;

	loadedBlock = loadBlock(block);

 for(i=0; i< BLOCK_SIZE; i++)
 {
 	  	printf("\n%c",loadedBlock[i]);
 }

	free(loadedBlock);

}

void printIndexBlock(unsigned int block)
{
	 int i;
	DWORD * loadedBlock;

	loadedBlock = loadIndexBlock(block);

	for(i=0; i< BLOCK_SIZE/sizeof(DWORD); i++)
	{
		printf("\n%d",loadedBlock[i]);
	}
	free(loadedBlock);
}


/******* T2FS_OPEN  *******/
FILE2 open2(char * filename)
{

	unsigned int fileBlock;
	FILE2 handle;

	t2fs_record * fileRecord = NULL;
    
	setName(&filename);
	 
	fileRecord = findRecord(filename, TYPEVAL_REGULAR, &fileBlock);

	// Não achou o arquivo
	if(fileRecord == NULL)
		return -1;

	handle = insertFileTAAP(fileRecord,fileBlock);
	
	return handle;
}


/****** T2FS_CLOSE ******/
int close2(FILE2 handle)
{
	if(handle == -1) return -1;	
	return removeFileTAAP(handle);
}


/**** T2FS READ ****/
int read2(FILE2 handle, char * buffer, int size)
{
	if(handle == -1) return -1;

	// Tamanho inválido
	if(size <= 0)
		return -1;

	int i;
	int offset;
	DWORD realBlock;
	char * loadedBuffer;

	// Retorna o record do arquivo

	unsigned int * currentPointer = getTAAPcurrentPointer(handle);

	t2fs_record * fileRecord = getTDAARecord(handle);


	int firstBlock = calcFistBlock(*currentPointer);
	int lastBlock = calcLastBlock(*currentPointer+size);

	int originalSize = size;

	// Vai passar do fim do arquivo
	if(*currentPointer+size > fileRecord->bytesFileSize)
	{
		size = fileRecord->bytesFileSize - *currentPointer;
	}

	// Calcula em qual bloco deve estar
	// currentPointer + size > BLOCK_SIZE
	// Vai carregar mais de um bloco
	//      |                            |
	// currentPointer          currentPointer + size
	// Primeiro bloco: currentPointer/BLOCK_SIZE
	// Último bloco: (currentPointer+size)/BLOCK_SIZE
	// Número de blocos lidos:
	// (currentPointer+size)/BLOCK_SIZE - currentPointer/BLOCK_SIZE + 1
	// Número de bytes do primeiro bloco:
	// currentPointer%BLOCK_SIZE

	offset = 0;

	for( i = firstBlock ; i <= lastBlock ; i++ )
	{
		// Retorna bloco real
		realBlock = getRealBlock(fileRecord, i);

		// Carrega bloco para memória
		loadedBuffer = loadBlock(realBlock);

		// Apenas um bloco é carregado
		if(i == firstBlock && i == lastBlock)
		{
			// Carrega desde currentPointer até currentPointer + size
			memcpy(buffer+offset,loadedBuffer+*currentPointer%BLOCK_SIZE,(*currentPointer+size)%BLOCK_SIZE);
			offset += (*currentPointer+size)%BLOCK_SIZE;

		}
		else if(i == firstBlock)
		{
			// No primeiro bloco carrega apenas o que está a partir do currentPointer
			memcpy(buffer+offset,loadedBuffer+*currentPointer%BLOCK_SIZE,BLOCK_SIZE-*currentPointer%BLOCK_SIZE);
			offset += BLOCK_SIZE-(*currentPointer)%BLOCK_SIZE;		

		}
		else if (i == lastBlock)
		{
			// Carrega apenas o que está antes ao currentPointer + size
			memcpy(buffer+offset,loadedBuffer+(*currentPointer+size)%BLOCK_SIZE,BLOCK_SIZE-(*currentPointer+size)%BLOCK_SIZE);
			offset += BLOCK_SIZE-(*currentPointer+size)%BLOCK_SIZE;
			
		}
		else
		{	// Lê o bloco todo

			memcpy(buffer+offset,loadedBuffer,BLOCK_SIZE);
			offset += BLOCK_SIZE;
				
		}


	}

	*currentPointer += size;

	if(originalSize == size)
		return size;
	else
		return 0;


}

int calcNumberOfBlocks(unsigned int begin, unsigned int end)
{
	return end - begin + 1;
}

int calcFistBlock(unsigned int begin)
{
	return begin/BLOCK_SIZE;
}

int calcFirstBlockOffset(unsigned int begin)
{
	return begin%BLOCK_SIZE;
}

int calcLastBlock(unsigned int end)
{
	return end/BLOCK_SIZE;
}

int calcLastBlockOffset(unsigned int end)
{
	return end%BLOCK_SIZE;
}

DWORD getRealBlock(t2fs_record * fileRecord, DWORD block)
{

	int numberOfPointers;
	int numberOfDoublePointers;
	int realBlock;
	DWORD * indexBlock;
	DWORD * indexBlock2;

	if(block == 0)
	{
		return fileRecord->dataPtr[0];
	}
	else if(block == 1)
	{
		return fileRecord->dataPtr[1];
	}
	else if(block == 2)
	{
		return fileRecord->dataPtr[2];
	}
	else if(block == 3)
	{
		return fileRecord->dataPtr[3];
	}
	
	// Número de ponteiros no bloco de índice
	// Ou seja, número de bloco que podem ser endereçados
	numberOfPointers = BLOCK_SIZE/sizeof(DWORD);

	// Se bloco for menor que endereçamento indireto simples + 2 (dataPtr[0] e dataPtr[1])
	// Significa o bloco procurado está no bloco de indices de indireção dupla
	if(block < numberOfPointers+4)
	{
		// Carrega bloco de índice e retorna a posição
		indexBlock = loadIndexBlock(fileRecord->singleIndPtr);

		realBlock = indexBlock[(block-4)];

		free(indexBlock);

		return realBlock;

	}

	numberOfDoublePointers = BLOCK_SIZE/sizeof(DWORD) * BLOCK_SIZE/sizeof(DWORD);
	if(block < 4+numberOfPointers+numberOfDoublePointers)
	{
		// Carrega bloco de índice 1
		indexBlock = loadIndexBlock(fileRecord->doubleIndPtr);

		// Carrega bloco de índice 2
		indexBlock2 = loadIndexBlock(indexBlock[(block-4-numberOfPointers)/numberOfPointers]);

		realBlock = indexBlock2[ (block-4-numberOfPointers) % numberOfPointers ];

		free(indexBlock);
		free(indexBlock2);

		return realBlock;

	}

	// Bloco maior do que o tamanho do arquivo
	return -1;


}

/** Atualiza ponteiro **/
int seek2 (FILE2 handle, unsigned int offset)
{
	if(handle == -1) return -1;
	
	// Retorna o record do arquivo
	t2fs_record * fileRecord = getTDAARecord(handle);

	unsigned int * currentPointer = getTAAPcurrentPointer(handle);

	// Posiciona no final do arquivo 
	if(offset == -1)
	{
		*currentPointer = fileRecord->bytesFileSize;		
		return 0;
	}


	// Passou do fim do arquivo

	if((*currentPointer + offset) > fileRecord->bytesFileSize)
		return -1;

	*currentPointer = offset;

	return 0;

}

/** Deleta arquivo **/
int delete2 (char *filename)
{
	t2fs_record * fileRecord;
	t2fs_record * directoryRecord;
	// Bloco onde o record do arquivo se encontra
	DWORD fileRecordBlock;
	DWORD directoryBlock;

	char * fileName;
	char * address;

	setName(&filename);
	
	// Arquivo não encontrado
	if(getNameAddress(filename, &fileName, &address) == -1)
		return -1;

	// Acha o arquivo
	fileRecord = findRecord(filename,TYPEVAL_REGULAR,&fileRecordBlock);

	// Não encontrou o arquivo
	if(fileRecord == NULL)
	{
		return -1;
	}
	// Procura pelo diretório
	directoryRecord = findRecord(address,TYPEVAL_DIRETORIO,&directoryBlock);

	// Desaloca todos os blocos referentes a esse arquivo
	removeBlocksFromFile(fileRecord);

	// Deve-se apagar o record do bloco
	// Caso retorne -1,  o record removido era o último do bloco, portanto deve-se retirar o ponteiro do bloco pai para esse bloco
	if(removeRecord(fileRecordBlock,fileRecord) == -1)
	{
		deallocateRecordBlock(fileRecordBlock);

		// Verifica se é raiz
		if(directoryBlock == -1)
		{
			// Arquivo removido está na raiz, deve varrer todos os ponteiros dataPtr[0], dataPtr[1], singleIndPtr e doubleIndPtr
			removeRecordBlockFromDirectory(fileRecordBlock,directoryRecord);
			writeSuperblock();
			directoryRecord->bytesFileSize -= RECORD_SIZE;
			directoryRecord->blocksFileSize -= 1;

		}
		else
		{
			// Arquivo removido não está na raiz, deve varrer todos os ponteiros dataPtr[0], dataPtr[1], singleIndPtr e doubleIndPtr
			removeRecordBlockFromDirectory(fileRecordBlock,directoryRecord);
			// Diretório perde 64 bytes
			directoryRecord->bytesFileSize -= RECORD_SIZE;
			directoryRecord->blocksFileSize -= 1;
			// Salva diretório pai novamente
			writeRecord(directoryBlock, directoryRecord);
		}

	}


	return 0;

}

/** Função write **/
int write2(FILE2 handle, char *buffer, int size)
{
	if(handle == -1) return -1;

	// Tamanho inválido
	if(size <= 0)
		return -1;

	int i;
	DWORD realBlock;
	char * loadedBuffer;
	unsigned int blocksToBeAllocated = 0;
	unsigned int offset;

	// Retorna o record do arquivo
	t2fs_record * fileRecord = getTDAARecord(handle);

	unsigned int * currentPointer = getTAAPcurrentPointer(handle);

	//int numberOfBlocks = calcNumberOfBlocks(*currentPointer,*currentPointer+size);
	int firstBlock = calcFistBlock(*currentPointer);
	int lastBlock = calcLastBlock(*currentPointer+size);


	/*if (firstBlock +1 >  fileRecord->blocksFileSize) 
		return -1;*/
		
	if (lastBlock+1 >= fileRecord->blocksFileSize)
	{
		if((blocksToBeAllocated = numberOfBlocksToBeAllocated(lastBlock, firstBlock, handle)) == -1)
			return -1;
			
	}

	// Calcula em qual bloco deve estar
	// currentPointer + size > BLOCK_SIZE
	// Vai carregar mais de um bloco
	//      |                            |
	// currentPointer          currentPointer + size
	// Primeiro bloco: currentPointer/BLOCK_SIZE
	// Último bloco: (currentPointer+size)/BLOCK_SIZE
	// Número de blocos lidos:
	// (currentPointer+size)/BLOCK_SIZE - currentPointer/BLOCK_SIZE + 1
	// Número de bytes do primeiro bloco:
	// currentPointer%BLOCK_SIZE

	offset = 0;
	BOOL isNewBlock;

	for( i = firstBlock ; i <= lastBlock ; i++ )
	{
		isNewBlock = FALSE;

		if (blocksToBeAllocated != 0)
		{
			if (i+1 <= fileRecord->blocksFileSize)
				realBlock = getRealBlock(fileRecord, i);
			else
			{
				isNewBlock = TRUE;
				realBlock = allocateNewBlock(handle, i, fileRecord);
			}
		}
		else
			realBlock = getRealBlock(fileRecord, i);

		loadedBuffer = loadBlock(realBlock);

		// Apenas um bloco é carregado
		if(i == firstBlock && i == lastBlock)
		{
			// Carrega desde currentPointer até currentPointer + size
			memcpy(loadedBuffer+*currentPointer%BLOCK_SIZE,buffer+offset, size);
			offset += size;
			writeBlock(realBlock, loadedBuffer);

			// Altera o tamanho do arquivo (bytesFileSize)
			if(*currentPointer == fileRecord->bytesFileSize)
				fileRecord->bytesFileSize += size;
			else if(*currentPointer + size > fileRecord->bytesFileSize)
				fileRecord->bytesFileSize += *currentPointer + size - fileRecord->bytesFileSize;

			// else -> tamanho do arquivo continua o mesmo
							
		}
		else if(i == firstBlock)
		{
			// No primeiro bloco carrega apenas o que está a partir do currentPointer
			memcpy(loadedBuffer+*currentPointer%BLOCK_SIZE,buffer+offset,BLOCK_SIZE-*currentPointer%BLOCK_SIZE);
			writeBlock(realBlock, loadedBuffer);

			if(*currentPointer == fileRecord->bytesFileSize)
				fileRecord->bytesFileSize += BLOCK_SIZE-(*currentPointer%BLOCK_SIZE);
			else if(*currentPointer + size > fileRecord->bytesFileSize)
				fileRecord->bytesFileSize += *currentPointer + BLOCK_SIZE-*currentPointer%BLOCK_SIZE - fileRecord->bytesFileSize;

			// else -> tamanho do arquivo continua o mesmo


			offset += BLOCK_SIZE-(*currentPointer)%BLOCK_SIZE;

		}
		else if (i == lastBlock)
		{
			// Carrega apenas o que está antes ao currentPointer + size
			memcpy(loadedBuffer+(*currentPointer+size)%BLOCK_SIZE,buffer+offset,(*currentPointer+size)%BLOCK_SIZE);
			
			writeBlock(realBlock, loadedBuffer);

			if(*currentPointer + offset == fileRecord->bytesFileSize)
				fileRecord->bytesFileSize += BLOCK_SIZE-(*currentPointer%BLOCK_SIZE);
			else if(*currentPointer + size > fileRecord->bytesFileSize)
				fileRecord->bytesFileSize += *currentPointer + BLOCK_SIZE-*currentPointer%BLOCK_SIZE - fileRecord->bytesFileSize;


			offset += (*currentPointer+size)%BLOCK_SIZE;

			if (isNewBlock)
				 fileRecord->blocksFileSize += 1;
			
		}
		else
		{	// Lê o bloco todo

			memcpy(loadedBuffer,buffer+offset,BLOCK_SIZE);
			
			writeBlock(realBlock, loadedBuffer);

			if(*currentPointer+offset == fileRecord->bytesFileSize)
				fileRecord->bytesFileSize += BLOCK_SIZE;

			offset += BLOCK_SIZE;


			if (isNewBlock)
				 fileRecord->blocksFileSize += 1;
				
		}
	
	}

	writeRecord(getTDAABlock(handle),  fileRecord);

	*currentPointer += size;

	return size;
}


int numberOfBlocksToBeAllocated(DWORD lastBlock, DWORD firstBlock, unsigned int handle)
{

	unsigned int blocksToBeAllocated = 0;
	DWORD currentBlock = firstBlock + 1;
	unsigned int numberOfPointers;

	numberOfPointers = BLOCK_SIZE/sizeof(DWORD);

	while(currentBlock <= lastBlock)
	{
		
		if (currentBlock >= getTDAARecord(handle)->blocksFileSize)
		{
				if (currentBlock == 1)
					blocksToBeAllocated++;
				else
				{
					if(numberOfPointers + 1 >= currentBlock) ///// block => currentBlock ????
					{
						if (currentBlock == 2)
							blocksToBeAllocated+= 2;
						else
							blocksToBeAllocated++;
					}
					else
					{
						if((currentBlock%numberOfPointers == 2) && ((currentBlock-3)/numberOfPointers < 1)) 
							blocksToBeAllocated+= 3;
						else
							if (currentBlock%numberOfPointers == 2)
								blocksToBeAllocated+= 2;
							else
								blocksToBeAllocated++;
					}
				}
		}

		currentBlock++;
	}

	if (areThereFreeBlocks(blocksToBeAllocated))
		return blocksToBeAllocated;
	else
		return -1;
}




int allocateNewBlock (int handle, int block, t2fs_record* record)
{

	/*1 --> aloca o bloco de dados
	2 --> aloca bloco de índice e de dados
	3 - 257 --> aloca bloco de dados
	258 --> aloca dois blocos de índices e um de dados
	258-513 --> aloca bloco de dados
	514 --> aloca bloco de índice e de dados*/
	DWORD freeBlock;
	int indexBlock0;
	DWORD * indexBlock;

	int numberOfPointers = BLOCK_SIZE/sizeof(DWORD);


		if (block == 1)
		{
			freeBlock = findFreeBlock();
			allocateDataBlock(freeBlock);
			record->dataPtr[1] = freeBlock;
			writeRecord(getTDAABlock(handle), record);

		}
		else
			if(numberOfPointers + 1 >= block)
			{
				if (block == 2)
				{
					freeBlock = findFreeBlock();
					allocateIndexBlock(freeBlock);
					record->singleIndPtr = freeBlock;
					writeRecord(getTDAABlock(handle), record);

					freeBlock = findFreeBlock();
					allocateDataBlock(freeBlock);
					indexBlock = loadIndexBlock(record->singleIndPtr);
					indexBlock[0] = freeBlock;
					writeIndexBlock(record->singleIndPtr, indexBlock);

				}						
				else
				{
					freeBlock = findFreeBlock();
					allocateDataBlock(freeBlock);
					indexBlock = loadIndexBlock(record->singleIndPtr);
					indexBlock[block-2] = freeBlock;
					writeIndexBlock(record->singleIndPtr, indexBlock);
				}
				free(indexBlock);
			}		
		else
		{
			if((block%numberOfPointers == 2) && ((block-3)/numberOfPointers < 1)) 
			{

				freeBlock = findFreeBlock();
				allocateIndexBlock(freeBlock);
				record->doubleIndPtr = freeBlock;
				writeRecord(getTDAABlock(handle), record);


				freeBlock = findFreeBlock();
				allocateIndexBlock(freeBlock);
				indexBlock = loadIndexBlock(record->doubleIndPtr);
				indexBlock[0] = freeBlock;
				indexBlock0 = freeBlock;
				writeIndexBlock(record->doubleIndPtr, indexBlock);

				freeBlock = findFreeBlock();
				allocateDataBlock(freeBlock);
				indexBlock = loadIndexBlock(indexBlock0);
				indexBlock[0] = freeBlock;
				writeIndexBlock(indexBlock0, indexBlock);

			}
			else
			{
				if (block%numberOfPointers == 2)
				{

					freeBlock = findFreeBlock();
					allocateIndexBlock(freeBlock);
					indexBlock = loadIndexBlock(record->doubleIndPtr);
					indexBlock[(block-2-numberOfPointers)/numberOfPointers] = freeBlock;
					indexBlock0 = freeBlock;
					writeIndexBlock(record->doubleIndPtr, indexBlock);

					freeBlock = findFreeBlock();
					allocateDataBlock(freeBlock);
					indexBlock = loadIndexBlock(indexBlock0);
					indexBlock[0] = freeBlock;
					writeIndexBlock(indexBlock0, indexBlock);


				}		
				else
				{
					indexBlock = loadIndexBlock(record->doubleIndPtr);
					indexBlock0 = indexBlock[(block-2-numberOfPointers)/numberOfPointers];
					freeBlock = findFreeBlock();
					allocateDataBlock(freeBlock);
					indexBlock = loadIndexBlock(indexBlock0);
					indexBlock[(block-2-numberOfPointers) % numberOfPointers] = freeBlock;
					writeIndexBlock(indexBlock0, indexBlock);

				}
				free(indexBlock);				
			}
		}


		return freeBlock;

}

int mkdir2 (char *pathname)
{
	// Entrada:
	// Ex.: /dir1/arquivo
	// Separa string, para pegar apenas o /dir (deixa o último de fora)

	t2fs_record * directoryRecord = NULL;
	// Bloco onde existe entrada vazia
	t2fs_record * fileRecord = NULL;
	t2fs_record * sameNameFileRecord = NULL;
	char * address = NULL;
	char * fileName = NULL;
	int invalidAddress = 0;
	unsigned int directoryBlock;
	unsigned int recordBlock;
	unsigned int freeBlock;
	BOOL isTheSameFile = FALSE;
	BOOL isThereSameNameFile = FALSE;
	
	setName(&pathname);
	
	invalidAddress = getNameAddress(pathname, &fileName, &address);

	printf("%s", "->");

	// Impossível criar arquivo
	if(invalidAddress == -1) {
		printf("%s", "2! - ");
		return -1;
	}


	// fileName => nome do arquivo
	// address => endereço do arquivo
	
	// Procura pelo diretório pai do arquivo que se quer incluir
	directoryRecord = findRecord(address, TYPEVAL_DIRETORIO, &directoryBlock);
	
	// Diretório pai encontrado
	if(directoryRecord == NULL) {
		printf("%s", "3! - ");
		return -1;	
	}

	sameNameFileRecord = findRecord(pathname, TYPEVAL_DIRETORIO, &freeBlock);

	if (sameNameFileRecord != NULL)
		isThereSameNameFile = TRUE;

	if (isThereSameNameFile)
	{
		if(sameNameFileRecord->dataPtr[0] != -1 || 
			sameNameFileRecord->dataPtr[1] != -1 || 
			sameNameFileRecord->dataPtr[2] != -1 || 
			sameNameFileRecord->dataPtr[3] != -1 || 
			sameNameFileRecord->singleIndPtr != -1 || 
			sameNameFileRecord->doubleIndPtr != -1) {
/**
			printf("sameNameFileRecord->dataPtr[0]: %d\n", sameNameFileRecord->dataPtr[0]);		
			printf("sameNameFileRecord->dataPtr[1]: %d\n", sameNameFileRecord->dataPtr[1]);		
			printf("sameNameFileRecord->dataPtr[2]: %d\n", sameNameFileRecord->dataPtr[2]);		
			printf("sameNameFileRecord->dataPtr[3]: %d\n", sameNameFileRecord->dataPtr[3]);		
			printf("sameNameFileRecord->singleIndPtr: %d\n", sameNameFileRecord->singleIndPtr);		
			printf("sameNameFileRecord->doubleIndPtr: %d\n", sameNameFileRecord->doubleIndPtr);		

			printRecordBlock(sameNameFileRecord->dataPtr[0]);
**/
			printf("%s", "4! - ");
			return -1;

		}

	}

	if(directoryRecord->dataPtr[0] != -1 ||
		directoryRecord->dataPtr[1] != -1  ||
		directoryRecord->dataPtr[2] != -1  ||
		directoryRecord->dataPtr[3] != -1  ||
		directoryRecord->singleIndPtr != -1 ||
		directoryRecord->doubleIndPtr != -1 ||
		isThereSameNameFile == TRUE) {
		// Carrega bloco para achar próxima posição disponível (lista de records)
		fileRecord = findEmptyRecord(directoryRecord->dataPtr[0], fileName, &isTheSameFile);
	
		// Offset dentro do bloco para posição disponível
		if(fileRecord != NULL)
		{
			recordBlock = directoryRecord->dataPtr[0];
		}
		else
		{
			//free(recordsBlock);

			// dataPtr[1] não está vazio
			if(directoryRecord->dataPtr[1] != -1  || 
				directoryRecord->dataPtr[2] != -1  ||
				directoryRecord->dataPtr[3] != -1  ||
				directoryRecord->singleIndPtr != -1 || 
				directoryRecord->doubleIndPtr != -1)
			{
				// Carrega bloco do dataPtr[2]
				fileRecord = findEmptyRecord(directoryRecord->dataPtr[1], fileName, &isTheSameFile);

				// Offset dentro do bloco para posição disponível
				if(fileRecord != NULL)
				{
					recordBlock = directoryRecord->dataPtr[1];
				}
				else
				{
					//free(recordsBlock);

					// dataPtr[1] não está vazio
					if(directoryRecord->dataPtr[2] != -1  ||
						directoryRecord->dataPtr[3] != -1  ||
						directoryRecord->singleIndPtr != -1 || 
						directoryRecord->doubleIndPtr != -1)
					{
						// Carrega bloco do dataPtr[2]
						fileRecord = findEmptyRecord(directoryRecord->dataPtr[1], fileName, &isTheSameFile);

						// Offset dentro do bloco para posição disponível
						if(fileRecord != NULL)
						{
							recordBlock = directoryRecord->dataPtr[1];
						}
						else
						{
							//free(recordsBlock);

							// dataPtr[1] não está vazio
							if(directoryRecord->dataPtr[3] != -1  || 
								directoryRecord->singleIndPtr != -1 || 
								directoryRecord->doubleIndPtr != -1)
							{
								// Carrega bloco do dataPtr[2]
								fileRecord = findEmptyRecord(directoryRecord->dataPtr[3], fileName, &isTheSameFile);

								// Offset dentro do bloco para posição disponível
								if(fileRecord != NULL)
								{
									recordBlock = directoryRecord->dataPtr[3];
								}
								else
								{

									if(directoryRecord->singleIndPtr != -1 || 
									directoryRecord->doubleIndPtr != -1)
									{

										fileRecord = EmptyRecordSingleIndPtr(directoryRecord->singleIndPtr, &recordBlock, fileName, &isTheSameFile);

										if (fileRecord == NULL)
										{

											if (directoryRecord->doubleIndPtr != -1)
											{

												fileRecord = EmptyRecordDoubleIndPtr(directoryRecord->doubleIndPtr, &recordBlock, fileName, &isTheSameFile);
												if (fileRecord == NULL) {
													printf("%s", "5! - ");
													return -1;
												}
											}
											else
											{
												if(!areThereFreeBlocks(4)) {
													printf("%s", "6! - ");
													return -1;
												}
		
					  							 freeBlock = findFreeBlock();
					   							 allocateIndexBlock(freeBlock);
					   							 directoryRecord->doubleIndPtr = freeBlock;

												if (directoryBlock != -1)
					   								writeRecord(directoryBlock, directoryRecord);
												else
													setSuperblockDoubleIndPtr(freeBlock);
					   							// writeRecordsBlock(directoryBlock, directoryRecord);

					   					 		fileRecord = EmptyRecordDoubleIndPtr(directoryRecord->doubleIndPtr, &recordBlock, fileName, &isTheSameFile);

											}
										}
									}
									else
									{			
										 if(!areThereFreeBlocks(3)){
											printf("%s", "7! - ");
											return -1;
										}
		
					  					 freeBlock = findFreeBlock();
					   					 allocateIndexBlock(freeBlock);
					   					 directoryRecord->singleIndPtr = freeBlock;

										if (directoryBlock != -1)
					   						writeRecord(directoryBlock, directoryRecord);
										else
											setSuperblockSingleIndPtr(freeBlock);
					   				        // writeRecordsBlock(directoryBlock, directoryRecord);

					   					fileRecord = EmptyRecordSingleIndPtr(directoryRecord->singleIndPtr, &recordBlock, fileName, &isTheSameFile);
									}
								}
							}
							else
							{	
								// Se não possui 
								if(!areThereFreeBlocks(2)){
									printf("%s", "8! - ");
									return -1;
								}
			
								// Aloca bloco para records
								freeBlock = findFreeBlock();
								allocateRecordsBlock(freeBlock);
								directoryRecord->dataPtr[3] = freeBlock;

								if (directoryBlock != -1)
					   				writeRecord(directoryBlock, directoryRecord);
								else
									setSuperblockDataPtr1(freeBlock);

								fileRecord = findEmptyRecord(directoryRecord->dataPtr[3], fileName, &isTheSameFile);
								recordBlock = directoryRecord->dataPtr[3];
								//writeRecordsBlock(directoryBlock, directoryRecord);	
							}


						}
					}
					else
					{	
						// Se não possui 
						if(!areThereFreeBlocks(2)){
							printf("%s", "8! - ");
							return -1;
						}
	
						// Aloca bloco para records
						freeBlock = findFreeBlock();
						allocateRecordsBlock(freeBlock);
						directoryRecord->dataPtr[2] = freeBlock;

						if (directoryBlock != -1)
			   				writeRecord(directoryBlock, directoryRecord);
						else
							setSuperblockDataPtr1(freeBlock);

						fileRecord = findEmptyRecord(directoryRecord->dataPtr[2], fileName, &isTheSameFile);
						recordBlock = directoryRecord->dataPtr[2];
						//writeRecordsBlock(directoryBlock, directoryRecord);	
					}
				}
			}
			else
			{	
				// Se não possui 
				if(!areThereFreeBlocks(2)){
					printf("%s", "8! - ");
					return -1;
				}
				
				// Aloca bloco para records
				freeBlock = findFreeBlock();
				allocateRecordsBlock(freeBlock);
				directoryRecord->dataPtr[1] = freeBlock;

				if (directoryBlock != -1)
	   				writeRecord(directoryBlock, directoryRecord);
				else
					setSuperblockDataPtr1(freeBlock);

				fileRecord = findEmptyRecord(directoryRecord->dataPtr[1], fileName, &isTheSameFile);
				recordBlock = directoryRecord->dataPtr[1];
				//writeRecordsBlock(directoryBlock, directoryRecord);	
			}
		}
	}
	else
	{
		if(!areThereFreeBlocks(2)){
			printf("%s", "9! - ");
			return -1;
		}

		// Aloca bloco para records
		freeBlock = findFreeBlock();
		allocateRecordsBlock(freeBlock);
		directoryRecord->dataPtr[0] = freeBlock;


		if (directoryBlock != -1)
	   		writeRecord(directoryBlock, directoryRecord);
		else
			setSuperblockDataPtr0(freeBlock);

		fileRecord = findEmptyRecord(directoryRecord->dataPtr[0], fileName, &isTheSameFile);
		recordBlock = directoryRecord->dataPtr[0];
		//writeRecordsBlock(directoryBlock, directoryRecord);
	}

	if(isTheSameFile == FALSE)
	{
		directoryRecord->bytesFileSize += RECORD_SIZE;
		directoryRecord->blocksFileSize = (directoryRecord->bytesFileSize - RECORD_SIZE)/BLOCK_SIZE +1;
	
		if(directoryBlock != -1)
			writeRecord(directoryBlock, directoryRecord);	
		else
			setSuperblockFileSize(directoryRecord->bytesFileSize, directoryRecord->blocksFileSize);



	}

	//emptyRecordBlock = findEmptyRecordsBlock(directoryRecord,&recordOffset, &recordBlock, fileName, directoryBlock);

	newDirectoryRecord(fileName, fileRecord);
	
	/*if(fileRecord != NULL)
		return -1;*/

	writeNewDirectoryRecord(recordBlock, fileRecord, pathname);

	//printRecordBlock(emptyRecordBlock);

	//printf("\nRecord Offset: %d \n Record Block: %d\n",recordOffset, recordBlock);
	printf("%s", "0! - ");
	return 0;

}

void writeNewDirectoryRecord (unsigned int recordBlock, t2fs_record* fileRecord, char* nome)
{
	t2fs_record* loadedBlock;
	unsigned int block;

	int recordOffset = 0;

	if (findRecord(nome, TYPEVAL_DIRETORIO, &block) != NULL)
	{
		loadedBlock = loadRecordsBlock(block);
		while((strcmp(fileRecord->name, loadedBlock[recordOffset].name)) || (loadedBlock[recordOffset].TypeVal != fileRecord->TypeVal))
			recordOffset++;
	}
	else
	{
		loadedBlock = loadRecordsBlock(recordBlock);
		while((loadedBlock[recordOffset].TypeVal == TYPEVAL_REGULAR) || (loadedBlock[recordOffset].TypeVal == TYPEVAL_DIRETORIO))
			recordOffset++;
	}

	memcpy(&loadedBlock[recordOffset].TypeVal, &(fileRecord->TypeVal), 1);
	memcpy(&loadedBlock[recordOffset].name, &(fileRecord->name), 31);
	memcpy(&loadedBlock[recordOffset].blocksFileSize, &(fileRecord->blocksFileSize),4);
	memcpy(&loadedBlock[recordOffset].bytesFileSize, &(fileRecord->bytesFileSize),4);
	memcpy(&loadedBlock[recordOffset].dataPtr, &(fileRecord->dataPtr),16);
	memcpy(&loadedBlock[recordOffset].singleIndPtr, &(fileRecord->singleIndPtr),4);
	memcpy(&loadedBlock[recordOffset].doubleIndPtr, &(fileRecord->doubleIndPtr),4);

	writeRecordsBlock(recordBlock, loadedBlock);
			
}

t2fs_record * newDirectoryRecord(char * name, t2fs_record * newDirectoryRecord)
{

	newDirectoryRecord->TypeVal = TYPEVAL_DIRETORIO;
	memcpy(&(newDirectoryRecord->name),name,31);
	newDirectoryRecord->blocksFileSize = 0;
	newDirectoryRecord->bytesFileSize = 0;
	newDirectoryRecord->dataPtr[0] = -1;
	newDirectoryRecord->dataPtr[1] = -1;
	newDirectoryRecord->dataPtr[2] = -1;
	newDirectoryRecord->dataPtr[3] = -1;
	newDirectoryRecord->singleIndPtr = -1;
	newDirectoryRecord->doubleIndPtr = -1;

	return newDirectoryRecord;

}

int rmdir2 (char *pathname)
{

	t2fs_record * fileRecord;
	t2fs_record * directoryRecord;
	// Bloco onde o record do arquivo se encontra
	DWORD fileRecordBlock;
	DWORD directoryBlock;

	char * fileName;
	char * address;

	if(!strcmp(pathname, "/"))
		return -1;

	// Arquivo não encontrado
	if(getNameAddress(pathname, &fileName, &address) == -1)
		return -1;

	// Acha o arquivo
	fileRecord = findRecord(pathname,TYPEVAL_DIRETORIO,&fileRecordBlock);

	// Não encontrou o arquivo
	if(fileRecord == NULL)
	{
		return -1;
	}

	if (fileRecord->dataPtr[0] != -1 || fileRecord->dataPtr[1] != -1 || fileRecord->singleIndPtr != -1 || fileRecord->doubleIndPtr != -1)
		return -1;

	// Procura pelo diretório
	directoryRecord = findRecord(address,TYPEVAL_DIRETORIO,&directoryBlock);

	// Desaloca todos os blocos referentes a esse arquivo
	//removeBlocksFromFile(fileRecord);

	// Deve-se apagar o record do bloco
	// Caso retorne -1,  o record removido era o último do bloco, portanto deve-se retirar o ponteiro do bloco pai para esse bloco
	if(removeRecord(fileRecordBlock,fileRecord) == -1)
	{
		deallocateRecordBlock(fileRecordBlock);

		// Verifica se é raiz
		if(directoryBlock == -1)
		{
			// Arquivo removido está na raiz, deve varrer todos os ponteiros dataPtr[0], dataPtr[1], singleIndPtr e doubleIndPtr
			removeRecordBlockFromDirectory(fileRecordBlock,directoryRecord);
			writeSuperblock();
			directoryRecord->bytesFileSize -= RECORD_SIZE;
			directoryRecord->blocksFileSize -= 1;

		}
		else
		{
			// Arquivo removido não está na raiz, deve varrer todos os ponteiros dataPtr[0], dataPtr[1], singleIndPtr e doubleIndPtr
			removeRecordBlockFromDirectory(fileRecordBlock,directoryRecord);
			// Diretório perde 64 bytes
			directoryRecord->bytesFileSize -= RECORD_SIZE;
			directoryRecord->blocksFileSize -= 1;
			// Salva diretório pai novamente
			writeRecord(directoryBlock, directoryRecord);
		}

	}


	return 0;

}

int chdir2 (char *pathname){
    DWORD block;
    	
	printf("\n\npai<%s>curdir<%s> pathname<%s>",getCurUpDir(),getCurDir(),pathname);
   
	setName(&pathname);//Verifica se endereço é relativou ou não
	
	printf("\n\npai<%s>curdir<%s> pathname<%s>",getCurUpDir(),getCurDir(),pathname);
	if(findRecord(pathname,TYPEVAL_DIRETORIO,&block) == NULL)//Diretório não existe!
	{
		return -1;
	}

	setBar(&pathname);
	   printf("\n\nIIIIIIIIIII\n\n");	 
    setCurDir(pathname); //Seta diretório corrente
	   printf("\n\nIIIIIIIIIII\n\n");	  
	setUpAddress(pathname); //Seta pai do diretorio corrente
		printf("\n\nIIIIIIIIIII\n\n");	
	
	// printf("\n\npai<%s>curdir<%s> pathname<%s>",getCurUpDir(),getCurDir(),pathname);
    return 0;

}

int getcwd2 (char *pathname, int size){

	return 0;
}

int readdir2(DIR2 handle, DIRENT2 *dentry) {
	
	return 0;
}

void dirt2(char* nome){

	t2fs_record * sameNameFileRecord = NULL;
	unsigned int freeBlock;
	
	sameNameFileRecord = findRecord(nome, TYPEVAL_DIRETORIO, &freeBlock);

	if(sameNameFileRecord != NULL)
	{
		if (sameNameFileRecord->dataPtr[0] != -1)
			dirt2DataPtr(sameNameFileRecord->dataPtr[0]);

		if( sameNameFileRecord->dataPtr[1] != -1)
			dirt2DataPtr(sameNameFileRecord->dataPtr[1]);

		if (sameNameFileRecord->singleIndPtr != -1)
			dirt2SingleIndPtr(sameNameFileRecord->singleIndPtr);

		if( sameNameFileRecord->doubleIndPtr != -1)
			dirt2DoubleIndPtr(sameNameFileRecord->doubleIndPtr);
	}
}


void dirt2DataPtr(unsigned int block){

	t2fs_record * loadedBlock;
	int i = 0;
	
	loadedBlock = loadRecordsBlock(block);


	while(i < BLOCK_SIZE / RECORD_SIZE)
	{
		if (((loadedBlock[i].TypeVal == TYPEVAL_REGULAR) || (loadedBlock[i].TypeVal == TYPEVAL_DIRETORIO)))
		{
			printf("\n%-15s", loadedBlock[i].name);

			if (loadedBlock[i].TypeVal == TYPEVAL_REGULAR)
				printf("r");
			else
				printf("d");

			printf("    %3d   %5d bytes", loadedBlock[i].blocksFileSize, loadedBlock[i].bytesFileSize);

		}
		i++;	
		
	}

}

void dirt2SingleIndPtr(unsigned int block){

	DWORD * loadedBlock;
	int i = 0;

	loadedBlock = loadIndexBlock(block);

	while (i != sizeof(loadedBlock)){

		if (loadedBlock[i] != -1)
			dirt2DataPtr(loadedBlock[i]);
		i++;
	}
}


void dirt2DoubleIndPtr(unsigned int block){

	DWORD * loadedBlock;
	int i = 0;

	loadedBlock = loadIndexBlock(block);

	while (i != sizeof(loadedBlock)){

		if (loadedBlock[i] != -1)
			dirt2SingleIndPtr(loadedBlock[i]);
		i++;
	}
	
	
}


void setName(char ** nome){
    
	char* nome_ = *nome;
	
	//Aponta para o pai
	if (nome_[0] == '.' && nome_[1] == '.' && nome_[2] == '/') {
	 
		char* nomeaux = (char*) malloc(strlen(getCurUpDir()) + strlen(*nome) -3);//retira 3 elemesntos ../
		strcpy(nomeaux , getCurUpDir()); 
		int j;  
		for (j=3 ; j<strlen(*nome); j++){
			nomeaux[strlen(getCurUpDir()) + j - 3]= nome_[j];
		}	
		 
       *nome = (char*) malloc(strlen(getCurUpDir()) + strlen(*nome) -3);
       *nome = nomeaux;
		
		// printf("AAAAAAAAAAAAAAAA%sAAAAAAAAAAAA",nomeaux);   
  
	}
	//Aponta para si próprio
	else if (nome_[0] == '.' && nome_[1] == '/' ) {
        char* nomeaux = (char*) malloc(strlen(getCurDir()) + strlen(*nome) -2);//retira 2 elemesntos ../
		strcpy(nomeaux , getCurDir()); 
		int j;  
        for (j=2 ; j<strlen(*nome); j++){
		 
			nomeaux[strlen(getCurDir()) + j - 2]= nome_[j];
		 
		}	
	
		*nome = (char*) malloc(strlen(getCurDir()) + strlen(*nome) -2);
		*nome = nomeaux;
		//printf("BBBBBBBBBBBBBBB%sBBBBBBBBBBBBBBBB",nomeaux);
  }
  //Endereço relativo
  else if(nome_[0] != '/') {	
		char* nomeaux = (char*) malloc(strlen(getCurDir()) + strlen(*nome));
	
		strcpy(nomeaux , getCurDir()); 
		strcat(nomeaux,*nome);
		*nome = (char*) malloc(strlen(getCurDir()) + strlen(*nome));
		*nome = nomeaux;	
  }

}

//Coloca '/' no final da string
void setBar(char ** pathname){
	 
	char* pathname_ = *pathname;
  
    if(pathname_[strlen(pathname_)-1] != '/' && pathname_[strlen(pathname_)-1] != '\0') { 
	   
	   *pathname = (char*) malloc(strlen(pathname_) + 1);
	   strcpy(*pathname , pathname_); 
	   strcat(*pathname,"/");
	  
	}
	else if (pathname_[strlen(pathname_)-2] != '/'&& pathname_[strlen(pathname_)-1] == '\0') {	
	
	   *pathname = (char*) malloc(strlen(pathname_) - 1);
	   pathname_[strlen(pathname_)-1]='/';	
	   *pathname = pathname_;
	 
	}
	else if (pathname_[strlen(pathname_)-2] == '/'&& pathname_[strlen(pathname_)-1] == '\0') {
       *pathname = (char*) malloc(strlen(pathname_) - 1);
	   strncpy(*pathname , pathname_ , strlen(pathname_) - 1);      	
	}   
}

void setUpAddress(char * nome)
{  
	int barPosition1 = -1;//última barra
	int barPosition2 = -1;//penúltima barra
	int i = 0;

	
	while(nome[i] != '\0' && i < strlen(nome))
	{
		if(nome[i] == '/')
		{
		    if(barPosition1 == barPosition2)//Primeira vez
			{
			  barPosition1 = i;
			}else
			{
			barPosition2 = barPosition1;
			barPosition1 = i;
			}
		
			
		}
		i++;
	}

	
    if(barPosition1==0 && barPosition2==-1)	{
		printf("\n\nooo1ooooo\n\n");
    	setCurUpDir("/");
	}
	else {	 
		printf("\n\nooo%dooooo\n\n",barPosition2);
	    char* addrup  = (char*) malloc(barPosition2+1);
		int j;  
		
        for (j=0 ; j<=barPosition2; j++){
		 
		addrup[j]= nome[j];
		 
		}	
		 
		setCurUpDir(addrup);
	}
}

