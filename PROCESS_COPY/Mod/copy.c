#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

int main(int argc,char* argv[]){
	int fd_src,fd_dst;
	int pos=atoi(argv[3]);
	int blocksize=atoi(argv[4]);
	char buf[102400]="";

	fd_src=open(argv[1],O_RDONLY);
	if(fd_src==-1){
		printf("copy %d read failed\n",getpid());
		return 1;
	}
	//umask(0000);
	fd_dst=open(argv[2],O_WRONLY|O_CREAT,0666);
	if(fd_dst==-1){
		printf("copy %d open failed\n",getpid());
		close(fd_src);
		return 1;
	}

	int s1=lseek(fd_src,pos,SEEK_SET);
	int s2=lseek(fd_dst,pos,SEEK_SET);
	
	int len=read(fd_src,buf,blocksize);
	if(len>0)	
		write(fd_dst,buf,blocksize);
	else printf("write error\n");

	//printf("child pid %d, pos %d, blocksize %d\n",getpid(),pos,blocksize);
	
	close(fd_src);
	close(fd_dst);	

	return 0;
}

