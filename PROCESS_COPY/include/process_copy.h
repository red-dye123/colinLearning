#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int PRAM_CHECK(int argc,int pronum,char* SRC);
int BLOCK_CUR(char* SRC,int pronum);
int PROCESS_CREATE(char* SRC,char* DEST,int pronum,int blocksize);
void PROCESS_WAIT(void);

