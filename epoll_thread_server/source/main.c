#include<epoll_thread_server.h>

int epfd;
std::map<int,time_t> heart_time_map;

void* scan_heart(void* arg){
	std::map<int,time_t>::iterator ite;
	while(1){
		sleep(4);
		time_t tp=time(NULL);
		for(ite=heart_time_map.begin();ite!=heart_time_map.end();ite++){
			if(tp-ite->second>MAXTIME){
				printf("%doffline\n",ite->first);
				SEND(ite->first,"offline\n",17,0);
				close(ite->first);
				heart_time_map.erase(ite);
			}
		}
	}
}

int main(){
	//网络初始化
	int sock;
	sock=net_initializer();
	epoll_initializer(sock);
	pool_t* pt;
	pt=thread_pool_create(100,10,1000);

	pthread_t tid;
	pthread_create(&tid,NULL,scan_heart,NULL);

	//启动监听
	epoll_listen(sock,pt);
	return 0;
}
