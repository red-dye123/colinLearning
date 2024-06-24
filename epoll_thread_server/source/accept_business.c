#include<epoll_thread_server.h>

void* accept_business(void* arg){
	int server_sock,client_sock;
	struct sockaddr_in client_addr;
	socklen_t addrlen;
	server_sock=*(int*)arg;
	//处理就绪sock
	addrlen=sizeof(client_addr);
	client_sock=ACCEPT(server_sock,(struct sockaddr*)&client_addr,&addrlen);
	//设置监听
	struct epoll_event node;
	node.data.fd=client_sock;
	node.events=EPOLLIN|EPOLLONESHOT/*|EPOLLET*/;
	epoll_ctl(epfd,EPOLL_CTL_ADD,client_sock,&node);
	//首次响应
	char cip[16];
	bzero(cip,16);
	inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,cip,16);
	first_response(client_sock,cip);
	//epolloneshot
	struct epoll_event nodes;
	nodes.data.fd=server_sock;
	nodes.events=EPOLLIN|EPOLLONESHOT;
	epoll_ctl(epfd,EPOLL_CTL_MOD,server_sock,&nodes);
	return NULL;
}
