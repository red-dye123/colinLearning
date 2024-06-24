#include <epoll_thread_server.h>

int producer_add_task(pool_t* p,task_t bs){
	//调用一次添加一次任务
	//判断任务队列是否为满，满则挂起，否则继续添加任务
	pthread_mutex_lock(&p->lock);
	if(p->thread_shutdown){
		while(p->list_cur==p->list_max){
			pthread_cond_wait(&p->not_full,&p->lock);
			if(!p->thread_shutdown){
				pthread_mutex_unlock(&p->lock);
				return -1;
			}
		}
		//头牵引添加任务
		p->list[p->list_front].business=bs.business;
		p->list[p->list_front].arg=bs.arg;
		++(p->list_cur);
		p->list_front=(p->list_front+1)%p->list_max;
		pthread_mutex_unlock(&p->lock);
		pthread_cond_signal(&p->not_empty);
	}
	return 0;
}
