#include <epoll_thread_server.h>

int epoll_initializer(int server_sock){
	//epoll创建初始化
	if((epfd=epoll_create(100000))==-1){
		perror("epoll_inintializer create epoll failed");
		return -1;
	}
	//初始化监听
	struct epoll_event node;
	node.data.fd=server_sock;
	node.events=EPOLLIN|EPOLLONESHOT/*|EPOLLET*/;//边缘
	if((epoll_ctl(epfd,EPOLL_CTL_ADD,server_sock,&node))==-1){
		perror("epoll_initializer listen server_sock failed");
		return -1;
	}
	printf("epoll_init success\n");
	return 0;
}
