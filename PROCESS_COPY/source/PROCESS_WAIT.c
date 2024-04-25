#include <process_copy.h>

void PROCESS_WAIT(){
	pid_t zpid;
	while((zpid=wait(NULL))>0){
		printf("parent %d waiting sucess, %d\n",getpid(),zpid);
	}
}
