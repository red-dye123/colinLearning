#include <epoll_thread_server.h>

void* data_business(void* arg){
	int client_sock=*(int*)arg;
	char buffer[1024];
	bzero(buffer,sizeof(buffer));
	int len;

	time_t tp;
	char time_buf[1024];
	bzero(time_buf,sizeof(time_buf));

	//读取请求数据，非阻塞读
	while((len=RECV(client_sock,buffer,sizeof(buffer),MSG_DONTWAIT))>0){
		heart_time_map[client_sock]=time(NULL);
		//判断客户端请求
		if(strcmp(buffer,"time\n")==0){
			tp=time(NULL);
			ctime_r(&tp,time_buf);
			SEND(client_sock,time_buf,strlen(time_buf),0);
		}
		else if(strcmp(buffer,"0")==0){
			printf("heart\n");
		}
		else{
			SEND(client_sock,"PLease try again\n",17,0);
		}
		//epolloneshot
		struct epoll_event node;
		node.data.fd=client_sock;
		node.events=EPOLLIN|EPOLLONESHOT;
		epoll_ctl(epfd,EPOLL_CTL_MOD,client_sock,&node);
	}
	
	//协议解析器
	return NULL;
}
