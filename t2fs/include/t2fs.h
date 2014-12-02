#ifndef __LIBT2FS___
#define __LIBT2FS___

#define TYPEVAL_REGULAR     0x01
#define TYPEVAL_DIRETORIO   0x02
#define TYPEVAL_INVALIDO    0xFF

typedef int FILE2;
typedef int DIR2;
typedef int BOOL;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;

#define FALSE 0
#define TRUE 1

#define TYPEVAL_REGULAR     0x01
#define TYPEVAL_DIRETORIO   0x02
#define TYPEVAL_INVALIDO    0xFF    // qualquer outro valor tamb�m � invalido

/** Registro de diret�rio (entrada de diret�rio) */
struct t2fs_record {
    /* Tipo da entrada. Indica se o registro � v�lido e, se for, o tipo do arquivo (regular ou diret�rio).
    �	0xFF, registro inv�lido (n�o associado a nenhum arquivos);
    �	0x01 arquivo regular;
    �	0x02, arquivo de diret�rio.
    */
    BYTE    TypeVal;        //   0:  1 byte

    /* Nome do arquivo. : string com caracteres ASCII (0x21 at� 0x7A), case sensitive.
    O string deve terminar com o caractere especial �\0� (0x00). */
    char    name[31];       //   1: 31 bytes

    /* Tamanho do arquivo. Expresso, apenas, em n�mero de blocos de dados
    (n�o est�o inclusos eventuais blocos de �ndice). */
    DWORD   blocksFileSize; //  32:  4 bytes

    /* Tamanho do arquivo. Expresso em n�mero de bytes.
    Notar que o tamanho de um arquivo n�o � um m�ltiplo do tamanho dos blocos de dados.
    Portanto, o �ltimo bloco de dados pode n�o estar totalmente utilizado.
    Assim, a detec��o do t�rmino do arquivo depender� da observa��o desse campo do registro. */
    DWORD   bytesFileSize;  //  36:  4 bytes

    /* Dois ponteiros diretos, para blocos de dados do arquivo */
    DWORD   dataPtr[4];     //  40:  16 bytes

    /* Ponteiro de indire��o simples,
    que aponta para um bloco de �ndices onde est�o ponteiros para blocos de dados do arquivo. */
    DWORD   singleIndPtr;   //  56:  4 bytes

    /* Ponteiro de indire��o dupla,
    que aponta para um bloco de �ndices onde est�o outros ponteiros para blocos de �ndice.
    Esses �ltimos ponteiros apontam para blocos de dados do arquivo. */
    DWORD   doubleIndPtr;   //  60:  4 bytes

} __attribute__((packed));

/** Superbloco */
struct t2fs_superbloco {
    /* Identifica��o do sistema de arquivo. � formado pelas letras �T2FS�. */
    char    Id[4];          // :   4 bytes

    /* Vers�o atual desse sistema de arquivos: (valor fixo 7DE=2014; 1=1 semestre). */
    WORD    Version;        // :   2 bytes

    /* Quantidade de setores  que formam o superbloco. (fixo em 1 setor) */
    WORD    SuperBlockSize; // :   2 bytes

    /* Tamanho total da parti��o T2FS, incluindo o tamanho do superblock. (1.048.832 bytes) */
    DWORD   DiskSize;       // :   4 bytes

    /* Quantidade total de blocos de dados na parti��o T2FS (1024 blocos). */
    DWORD   NofBlocks;      // :   4 bytes

    /* Tamanho de um bloco. (1024 bytes) */
    DWORD   BlockSize;      // :   4 bytes

    /* N�o usados */
    char    Reserved[108];  // : 108 bytes

    /* Registro que descreve o arquivo que mant�m o bitmap de blocos livres e ocupados */
    struct t2fs_record BitMapReg;  // :  64 bytes

    /* Registro que descreve o arquivo que mant�m as entradas do diret�rio raiz */
    struct t2fs_record RootDirReg; // :  64 bytes

} __attribute__((packed));

// Declara��o dos tipos superbloco e bloco
typedef struct t2fs_superbloco t2fs_superblock;
typedef struct t2fs_record t2fs_record;

#define MAX_FILE_NAME_SIZE 255
typedef struct {
    char name[MAX_FILE_NAME_SIZE+1];
    int fileType;   // ==1, is directory; ==0 is file
    unsigned long fileSize;
} DIRENT2;

/** Retorna a identifica��o dos implementadores do T2FS. */
int identify2 (char *name, int size);

/** Fun��o usada para criar um novo arquivo no disco. */
FILE2 create2 (char *filename);

/** Fun��o usada para remover (apagar) um arquivo do disco. */
int delete2 (char *filename);

/** Fun��o que abre um arquivo existente no disco. */
FILE2 open2 (char *filename);

/** Fun��o usada para fechar um arquivo. */
int close2 (FILE2 handle);

/** Fun��o usada para realizar a leitura em um arquivo. */
int read2 (FILE2 handle, char *buffer, int size);

/** Fun��o usada para realizar a escrita em um arquivo. */
int write2 (FILE2 handle, char *buffer, int size);

/** Altera o contador de posi��o (current pointer) do arquivo. */
int seek2 (FILE2 handle, unsigned int offset);

/** Fun��o usada para criar um novo diret�rio. */
int mkdir2 (char *pathname);

/** Fun��o usada para remover (apagar) um diret�rio do disco. */
int rmdir2 (char *pathname);

/** Fun��o que abre um diret�rio existente no disco. */
DIR2 opendir2 (char *pathname);

/** Fun��o usada para ler as entradas de um diret�rio. */
int readdir2 (DIR2 handle, DIRENT2 *dentry);

/** Fun��o usada para fechar um arquivo. */
int closedir2 (DIR2 handle);

/** Fun��o usada para alterar o diret�rio corrente. */
int chdir2 (char *pathname);

/** Fun��o usada para obter o caminho do diret�rio corrente. */
int getcwd2 (char *pathname, int size);

int getNameAddress(char * nome, char ** fileName, char ** address);

t2fs_record* EmptyRecordDoubleIndPtr(unsigned int block, unsigned int* recordBlock, char * fileName, BOOL* isTheSameFile);

t2fs_record* EmptyRecordSingleIndPtr(unsigned int block, unsigned int* recordBlock, char * fileName, BOOL* isTheSameFile);

void removeBlocksFromFile(t2fs_record * fileRecord);

void removeRecordBlockFromDirectory(DWORD recordBlock, t2fs_record * directoryRecord);

t2fs_record* findEmptyRecord(unsigned int block,  char * fileName, BOOL* isTheSameFile);

t2fs_record * newFileRecord(char * name, t2fs_record * newFileRecord);

void writeNewFileRecord (unsigned int recordBlock, t2fs_record* fileRecord, char* nome);

void writeRecord  (unsigned int recordBlock, t2fs_record* fileRecord);

int removeRecord(unsigned int recordBlock, t2fs_record* fileRecord);

void printRecordBlock(unsigned int block);
void printDataBlock(unsigned int block);
void printIndexBlock(unsigned int block);

int numberOfBlocksToBeAllocated(DWORD lastBlock, DWORD firstBlock, unsigned int handle);
int allocateNewBlock (int handle, int block, t2fs_record* record);

/** READ WRITE **/
int calcNumberOfBlocks(unsigned int begin, unsigned int end);
DWORD getRealBlock(t2fs_record * fileRecord, DWORD block);
int calcFistBlock(unsigned int begin);
int calcLastBlock(unsigned int end);
int calcFirstBlockOffset(unsigned int begin);
int calcLastBlockOffset(unsigned int end);

void dirt2(char* nome);
void dirt2DataPtr(unsigned int block);
void dirt2SingleIndPtr(unsigned int block);
void dirt2DoubleIndPtr(unsigned int block);

t2fs_record * newDirectoryRecord(char * name, t2fs_record * newDirectoryRecord);
void writeNewDirectoryRecord (unsigned int recordBlock, t2fs_record* fileRecord, char* nome);
int getNameAddress(char * nome, char ** fileName, char ** address);

/** Guarda o nome dos autores, quando "setada" */
char *authors;

#endif
