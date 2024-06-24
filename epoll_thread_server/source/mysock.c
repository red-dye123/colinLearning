#include<mysock.h>

int client_sock_array[1024];

int SOCKET(int domain,int type,int protocol){
	int sock;
	if((sock=socket(domain,type,protocol))==-1){
		perror("socket create failed");
		return -1;
	}
	return sock;
}

int BIND(int sockfd,const struct sockaddr* addr,socklen_t addrlen){
	if((bind(sockfd,addr,addrlen))==-1){
		perror("bind call failed");
		return -1;
	}
	return 0;
}

int LISTEN(int sockfd,int backlog){
	if((listen(sockfd,backlog))==-1){
		perror("listen call failed");
		return -1;
	}
	return 0;
}

int CONNECT(int sockfd,const struct sockaddr* addr,socklen_t addrlen){
	if((connect(sockfd,addr,addrlen))==-1){
		perror("connect call failed");
		return -1;
	}
	return 0;
}

int ACCEPT(int sockfd,struct sockaddr* addr,socklen_t* addrlen){
	int sock;
	if((sock=accept(sockfd,addr,addrlen))==-1){
		if(errno==EAGAIN){
			return -1;
		}
		perror("accept call failed");
		return -1;
	}
	return sock;
}

ssize_t RECV(int sockfd,void* buf,size_t len,int flags){
	ssize_t size;
	if((size=recv(sockfd,buf,len,flags))==-1){
		if(errno==EAGAIN){
			return -1;
		}
		perror("recv call failed");
		return -1;
	}
	return size;
}

ssize_t SEND(int sockfd,const void* buf,size_t len,int flags){
	ssize_t size;
	if((size=send(sockfd,buf,len,flags))==-1){
		perror("send call failed");
		return -1;
	}
	return size;
}

int net_initializer(){
	//套接字信息
	struct sockaddr_in addrServer;
	bzero(&addrServer,sizeof(addrServer));
	addrServer.sin_family=AF_INET;
	addrServer.sin_port=htons(8080);
	addrServer.sin_addr.s_addr=htonl(INADDR_ANY);
	//创建套接字
	int server_sock;
	server_sock=SOCKET(AF_INET,SOCK_STREAM,0);
	BIND(server_sock,(struct sockaddr*)&addrServer,sizeof(addrServer));
	LISTEN(server_sock,128);
	return server_sock;
}

void first_response(int sock,char* cip){
	char response[1024];
	bzero(response,sizeof(response));
	sprintf(response,"hi, %s wellcome test TCP server.\n",cip);
	SEND(sock,response,strlen(response),0);
}

int business(int flags){
	ssize_t recv_size;
	char recv_buffer[1024];
	//循环尝试读取客户端请求
	for(int i=0;i<1024;i++){
		if(client_sock_array[i]!=-1){
			int client_sock=client_sock_array[i];
			
			bzero(recv_buffer,sizeof(recv_buffer));
			recv_size=RECV(client_sock,recv_buffer,sizeof(recv_buffer),flags);
			if(recv_size>0){
				printf("client:%s\n",recv_buffer);
				if((strcmp(recv_buffer,"time\n")==0) || (strcmp(recv_buffer,"time")==0)){
					//响应系统时间
					time_t tp;
					char tbuf[1024];
					bzero(tbuf,sizeof(tbuf));
					ctime_r(&tp,tbuf);
					SEND(client_sock,tbuf,strlen(tbuf),0);
					printf("server,response time success.\n");
				}
				else{
					//数据处理，大小写转换 toupper()
					int cnt=0;
					while(cnt<recv_size){
						recv_buffer[cnt]=toupper(recv_buffer[cnt]);
						cnt++;
					}
					SEND(client_sock,recv_buffer,recv_size,0);
					printf("server,response data sucess.\n");
				}
			}
			else if(recv_size==0){//sock断开
				printf("client exit, close sock %d\n",client_sock);
				close(client_sock);
				//从数组中删除
				client_sock_array[i]=-1;
			}
			else if(recv_size==-1){
			}
		}
	}
	return 0;	
}
