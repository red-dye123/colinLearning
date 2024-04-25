#include<process_copy.h>

int BLOCK_CUR(char* SRC,int pronum){
	int fd_src=open(SRC,O_RDONLY);
	if(fd_src==-1){
		perror("open failed");
		exit(0);
	}
	int src_file_size=lseek(fd_src,0,SEEK_END);
	return src_file_size%pronum?src_file_size/pronum+1:src_file_size/pronum;
}

