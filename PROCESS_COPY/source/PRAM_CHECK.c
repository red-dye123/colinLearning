#include<process_copy.h>

int PRAM_CHECK(int argc,int pronum,char* SRC){
	if(argc<3){
		printf("wrong argc.\n");
		return 0;
	}
	if(access(SRC,F_OK)!=0){
		printf("src no exists.\n");
		return 0;
	}
	if(pronum<5 || pronum>100){
		printf("over pronum.\n");
		return 0;
	}
	return 1;
}
