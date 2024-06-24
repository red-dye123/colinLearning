#include <epoll_thread_server.h>

void* manager_thread(void* arg){
	pool_t* p=(pool_t*)arg;
	//持续扫描阈值，完成扩容缩减
	int alive,busy,cur;
	int flag;//遍历下标 ctids
	int add;//计数器(创建线程)
	int err;
	pthread_detach(pthread_self());
	printf("manager thread 0x%x Running\n",(unsigned int)pthread_self());
	while(p->thread_shutdown){
		//取值
		pthread_mutex_lock(&p->lock);
		alive=p->thread_alive;
		busy=p->thread_busy;
		cur=p->list_cur;
		pthread_mutex_unlock(&p->lock);
		//判断扩容
		printf("server status:alive %d busy %d idel %d cur %d B/A %.2f%% A/M %.2f%%\n",alive,busy,alive-busy,cur,(double)busy/alive*100,(double)alive/p->thread_max*100);
		if((alive-busy<=cur || (double)busy/alive*100>=70) && alive+p->thread_min<=p->thread_max){
			for(flag=0,add=0;flag<p->thread_max && add<p->thread_min;flag++){
				if(p->ctids[flag]==0 || !if_thread_alive(p->ctids[flag])){
					if((err=pthread_create(&p->ctids[flag],NULL,customer_thread,(void*)p))>0){
						printf("manager_thread,create cusotomer failed:%s\n",strerror(err));
					}
					++add;
					pthread_mutex_lock(&p->lock);
					++(p->thread_alive);
					pthread_mutex_unlock(&p->lock);
				}
			}		
		}
		//缩减
		if(busy*2<=alive-busy && alive-p->thread_min>=p->thread_min){
			p->thread_exitcode=p->thread_min;
			for(int i=0;i<p->thread_min;i++){
				pthread_cond_signal(&p->not_empty);
			}
		}		
		sleep(1);
	}
	printf("shutdown close,manager exiting..\n");
	pthread_exit(NULL);
}

