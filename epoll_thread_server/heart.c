#include<mysock.h>

void* heart_beat(void* arg){
	int server_sock=*(int*)arg;
	char recvBuf[1024]="";
	while(1){
		
		char data[]="0";
		printf("0\n");
		int result=SEND(server_sock,data,sizeof(data),MSG_DONTWAIT);
		if(result<0){
			printf("server close\n");
			break;
		}
		sleep(10);
	}
	return NULL;
}

int main(){
	//创建套接字
	int server_sock=SOCKET(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//服务器套接字信息
	struct sockaddr_in addrServer;
	bzero(&addrServer,sizeof(addrServer));
	addrServer.sin_family=AF_INET;
	addrServer.sin_port=htons(8080);
	addrServer.sin_addr.s_addr=inet_addr("127.0.0.1");

	CONNECT(server_sock,(struct sockaddr*)&addrServer,sizeof(addrServer));
	
	pthread_t tid;
	pthread_create(&tid,NULL,heart_beat,(void*)&server_sock);

	int nRecvNum=0;
	int nSendNum=0;
	char recvBuf[1024]="";
	char sendBuf[1024]="";
	bzero(sendBuf,sizeof(sendBuf));
	bzero(recvBuf,sizeof(recvBuf));

	nRecvNum=RECV(server_sock,recvBuf,sizeof(recvBuf),0);//first recv
	printf("server:%s\n",recvBuf);
	bzero(recvBuf,sizeof(recvBuf));

	while((fgets(sendBuf,sizeof(sendBuf),stdin))!=NULL){
		nSendNum=SEND(server_sock,sendBuf,sizeof(sendBuf),0);	
		nRecvNum=RECV(server_sock,recvBuf,sizeof(recvBuf),0);
		printf("server:%s\n",recvBuf);
		bzero(sendBuf,sizeof(sendBuf));
		bzero(recvBuf,sizeof(recvBuf));
	}
	close(server_sock);
	return 0;
}

