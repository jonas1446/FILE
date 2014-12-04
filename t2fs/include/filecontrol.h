

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
	t2fs_file handle;
	unsigned int TDAAEntry;
	unsigned int currentPointer; // posição corrente do ponteiro de acesso
	struct t2fs_taap * nextFile;
}t2fs_taap;


typedef struct freeHandles {
	t2fs_file handle;
	struct freeHandles* next;
	struct freeHandles* previous;
}freeHandles;



void setCurDir(char* addr);
char* getCurDir(void);
void setCurUpDir(char* addr);
char* getCurUpDir(void);



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




#endif
