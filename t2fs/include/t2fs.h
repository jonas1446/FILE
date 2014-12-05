

#ifndef __LIBT2FS___
#define __LIBT2FS___

#define TYPEVAL_REGULAR     0x01
#define TYPEVAL_DIRETORIO   0x02
#define TYPEVAL_INVALIDO    0xFF

typedef int FILE2;
typedef int DIR2;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;


/** Registro de diretório (entrada de diretório) */
struct t2fs_record {

    BYTE    TypeVal;        
    char    name[31];      
    DWORD   blocksFileSize; 
    DWORD   bytesFileSize; 
    DWORD   dataPtr[4];   
    DWORD   singleIndPtr; 
    DWORD   doubleIndPtr;  
} __attribute__((packed));

/** Superbloco */
struct t2fs_superbloco {
  
    char    Id[4]; 
    WORD    Version;  
    WORD    SuperBlockSize;
    DWORD   DiskSize; 
    DWORD   NofBlocks;  
    DWORD   BlockSize;
    char    Reserved[108]; 
    struct t2fs_record BitMapReg; 
    struct t2fs_record RootDirReg; 
} __attribute__((packed));

#define MAX_FILE_NAME_SIZE 255
typedef struct {
    char name[MAX_FILE_NAME_SIZE+1];
    int fileType;   // ==1, is directory; ==0 is file
    unsigned long fileSize;
} DIRENT2;

typedef struct t2fs_superbloco t2fs_superblock;
typedef struct t2fs_record t2fs_record;



int identify2 (char *name, int size);
FILE2 create2 (char *filename);
int delete2 (char *filename);
FILE2 open2 (char *filename);
int close2 (FILE2 handle);
int read2 (FILE2 handle, char *buffer, int size);
int write2 (FILE2 handle, char *buffer, int size);
int seek2 (FILE2 handle, unsigned int offset);

int mkdir2 (char *pathname);
int rmdir2 (char *pathname);
DIR2 opendir2 (char *pathname);
int readdir2 (DIR2 handle, DIRENT2* dentry);
int closedir2 (DIR2 handle);
int chdir2 (char* pathname);
int getcwd2 (char* pathname, int size);



int getNameAddress(char * nome, char ** fileName, char ** address);

void setName(char ** nome);

t2fs_record* EmptyRecordDoubleIndPtr(unsigned int block, unsigned int* recordBlock, char * fileName, int* isTheSameFile);

t2fs_record* EmptyRecordSingleIndPtr(unsigned int block, unsigned int* recordBlock, char * fileName, int* isTheSameFile);

void removeBlocksFromFile(t2fs_record * fileRecord);

t2fs_record* findEmptyRecord(unsigned int block,  char * fileName, int* isTheSameFile);

t2fs_record * recFile(char * name, t2fs_record * recFile);

void wrrec (unsigned int recordBlock, t2fs_record* fileRecord, char* nome);

void writeRecord  (unsigned int recordBlock, t2fs_record* fileRecord);

int chdir2 (char *pathname);
int getcwd2 (char *pathname, int size);

void printRecordBlock(unsigned int block);
void printDataBlock(unsigned int block);
void printIndexBlock(unsigned int block);

void readd(char* nome);
void readdDataPtr(unsigned int block);
void readdSingleIndPtr(unsigned int block);
void readdDoubleIndPtr(unsigned int block);

t2fs_record * newdirrec(char * name, t2fs_record * newdirrec);
void writeNewdirrec (unsigned int recordBlock, t2fs_record* fileRecord, char* nome);
FILE2 t2fs_createDirectory (char * nome);
int t2fs_deleteDirectory (char *name);

int calcNumberOfBlocks(unsigned int begin, unsigned int end);

int calcFistBlock(unsigned int begin);

int calcFirstBlockOffset(unsigned int begin);

int calcLastBlock(unsigned int end);

int calcLastBlockOffset(unsigned int end);

DWORD getRealBlock(t2fs_record * fileRecord, DWORD block);

int numberOfBlocksToBeAllocated(DWORD lastBlock, DWORD firstBlock, unsigned int handle);

int allocateNewBlock (int handle, int block, t2fs_record* record);

void setBar(char ** pathname);

void setUpAddress(char * nome);

#endif
