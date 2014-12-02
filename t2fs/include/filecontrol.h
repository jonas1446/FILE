

#ifndef FILECONTROL_H
#define FILECONTROL_H

#include <t2fs.h>

// Estrutura da "Tabela dos descritores de arquivos abertos"
typedef struct TDAA {
	t2fs_record record; //record do arquivo
	unsigned int block; // bloco onde o descritor do arquivo se encontra
	unsigned int numberOfInstances;
}t2fs_tdaa;

typedef struct TAAP {
	FILE2 handle;
	unsigned int TDAAEntry;
	unsigned int currentPointer; // posição corrente do ponteiro de acesso
	struct t2fs_taap * nextFile;
}t2fs_taap;


typedef struct freeHandles {
	FILE2 handle;
	struct freeHandles* next;
	struct freeHandles* previous;
}freeHandles;


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
FILE2 insertFileTDAA(t2fs_record * fileRecord, DWORD fileBlock);
int canOpenMoreFiles();
void printOpenedFiles();
void printTAAP();
t2fs_record * getTDAARecord(FILE2 handle);
DWORD getTDAABlock(FILE2 handle);
unsigned int * getTAAPcurrentPointer(FILE2 handle);
int removeFileTDAA(FILE2 handle);

FILE2 isNewFile(t2fs_record* fileRecord);

void insertNewFreeHandle(FILE2 handle);
int removeTAAPFile(FILE2 handle);
FILE2 addProcessTAAP(FILE2 TDAAEntry);
int removeFileTAAP(FILE2 handle);
FILE2 insertFileTAAP(t2fs_record * fileRecord, DWORD fileBlock);




#endif
