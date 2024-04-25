#include<process_copy.h>

int PROCESS_CREATE(char* SRC,char* DEST,int pronum,int blocksize){
	pid_t pid;
	int i;
	for(i=0;i<pronum;i++){
		pid=fork();
		if(pid==0)
			break;
	}
	if(pid!=0){
		PROCESS_WAIT();
	}
	else{
		
		int pos=i*blocksize;
		char STR_POS[100]="";
		char STR_BLK[100]="";
		bzero(STR_POS,sizeof(STR_POS));
		bzero(STR_BLK,sizeof(STR_BLK));
		sprintf(STR_POS,"%d",pos);
		sprintf(STR_BLK,"%d",blocksize);
		printf("child execl..\n");
		execl("/home/colin/colin/PROCESS_COPY/Mod/copy","copy",SRC,DEST,STR_POS,STR_BLK,NULL);
	}
	return 0;
}


