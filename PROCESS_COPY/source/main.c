#include <process_copy.h>

int main(int argc,char* argv[]){
	int pronum=0;
	if(argc<4)
		pronum=5;
	else pronum=atoi(argv[3]);
	if(PRAM_CHECK(argc,pronum,argv[1])==0){
		printf("input error\n");
		return 0;
	}
	int blocksize=BLOCK_CUR(argv[1],pronum);
	printf("blocksize= %d,pronum=%d\n",blocksize,pronum);
	PROCESS_CREATE(argv[1],argv[2],pronum,blocksize);
	return 0;
}

