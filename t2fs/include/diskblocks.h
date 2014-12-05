#include <t2fs.h>

#define SECTOR_SIZE 256
#define SUPERBLOCK 256
#define BLOCK_SIZE getBlocksize()
#define NOF_BLOCKS getNofblocks()
#define RECORD_SIZE 64
#define BITS_PER_WORD 8
#define MAX_FILE_NAME_SIZE 31
#define NOT_ERROR 0

/** Carrega o superbloco **/
void initSuperblock(void);
char * getSuperblock_id();
WORD getSuperblockVersion();
WORD getSuperblocksize();
DWORD getDisksize();
DWORD getNofblocks();
DWORD getBlocksize();
char * getReserved();
t2fs_record * getBitmapreg();
t2fs_record * getRootdirreg();
void printSuperblock();

char * loadBlock(unsigned int block);
t2fs_record * loadRecordsBlock(unsigned int block);

/// Procura por record
t2fs_record * findRecord(char * name, BYTE TypeVal, unsigned int* recordBlock);
t2fs_record* loadDataPtr(unsigned int block, BYTE TypeVal, char* token);
t2fs_record* loadSingleIndPtr(unsigned int block, BYTE TypeVal, char* token, unsigned int* recordBlock);
t2fs_record* loadDoubleIndPtr(unsigned int block, BYTE TypeVal, char* token, unsigned int* recordBlock);
DWORD* loadIndexBlock(unsigned int block);

int allocateDataBlock(unsigned int block);
int allocateIndexBlock(unsigned int block);
int allocateRecordsBlock(unsigned int block);
int deallocateDataBlock(unsigned int block);
int deallocateRecordBlock(unsigned int block);
int writeIndexBlock(unsigned int block, DWORD * indexBlock);
int writeRecordsBlock(unsigned int block, t2fs_record * record);
int writeBlock(DWORD block, char * buffer);

void error_read(unsigned int sector);
void error_write(unsigned int sector);

void writeSuperblock(void);
void setSuperblockDoubleIndPtr(DWORD block);
void setSuperblockSingleIndPtr(DWORD block);
void setSuperblockDataPtr0(DWORD block);
void setSuperblockDataPtr1(DWORD block);
void setSuperblockFileSize(DWORD bytes, DWORD blocks);


#endif
