#include <epoll_thread_server.h>

int epoll_listen(int server_sock,pool_t* p){
	//监听接口，生产者进行持续的sock事件监听并完成任务分支
	int readynum;
	int flag;
	struct epoll_event readylist[100000];
	task_t temp_bs;
	//循环监听
	printf("epoll_listen\n");
	while(p->thread_shutdown){
		//边缘触发，忽略为处理 避免epoll_wait异常返回
		//水平模式+EPOLLONESHOT or 边缘模式
		//水平模式异常添加任务的问题，将就绪sock投递后，消费者为及时处理导致生产者多次调用epoll_wait，函数立即返回，多次添加任务（异常）
		if((readynum=epoll_wait(epfd,readylist,100000,-1))==-1){
			perror("epoll_listen,epoll_wait call failed");
			return -1;
		}
		//循环判断处理就绪
		flag=0;
		while(readynum){
			if(readylist[flag].data.fd==server_sock){
				//向任务容器中添加一次连接业务
				temp_bs.business=accept_business;
				temp_bs.arg=(void*)&server_sock;
				producer_add_task(p,temp_bs);
				printf("add accept_business %p sucess\n",accept_business);
			}
			else{
				//添加数据处理业务
				temp_bs.business=data_business;
				temp_bs.arg=(void*)&readylist[flag].data.fd;
				producer_add_task(p,temp_bs);
				printf("add data_business %p sucess\n",data_business);
			}
			--readynum;
			++flag;
		}
	}
	printf("epoll_listen exit\n");
	return 0;
}
