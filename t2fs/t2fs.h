

#ifndef t2fs_file



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
    char    Id[4];          /* Identificação do sistema de arquivo. É formado pelas letras T2FS. */
    WORD    Version;        /* Versão atual desse sistema de arquivos: (valor fixo 7DE=2014; 2=2 semestre). */
    WORD    SuperBlockSize; /* Quantidade de setores  que formam o superbloco. (fixo em 1 setor) */
    DWORD   DiskSize;       /* Tamanho total da partição T2FS, incluindo o tamanho do superblock. */
    DWORD   NofBlocks;      /* Quantidade total de blocos de dados na partição T2FS (1024 blocos). */
    DWORD   BlockSize;      /* Tamanho de um bloco.*/
    char    Reserved[108];  /* Não usados */
    //struct t2fs_record BitMapReg;  /* Registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados */
   // struct t2fs_record RootDirReg; /* Registro que descreve o arquivo que mantém as entradas do diretório raiz */
} __attribute__((packed));


#define MAX_FILE_NAME_SIZE 255
typedef struct {
    char name[MAX_FILE_NAME_SIZE+1];
    int fileType;   // ==1, is directory; ==0 is file
    unsigned long fileSize;
} DIRENT2;

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
int readdir2 (DIR2 handle, DIRENT2 *dentry);
int closedir2 (DIR2 handle);

int chdir2 (char *pathname);
int getcwd2 (char *pathname, int size);

void GetDiskInformation2(void);
#endif
