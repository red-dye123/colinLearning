#include <epoll_thread_server.h>

void* customer_thread(void* arg){
	pool_t* p=(pool_t*)arg;
	task_t temp;
	pthread_detach(pthread_self());
	printf("customer 0x%x waiting jobs\n",(unsigned int)pthread_self());
	while(p->thread_shutdown){
		//消费线程，持续获取任务
		pthread_mutex_lock(&p->lock);
		while(p->list_cur==0){
			pthread_cond_wait(&p->not_empty,&p->lock);
			if(!p->thread_shutdown){
				pthread_mutex_unlock(&p->lock);
				printf("customer 0x%x,shutdown its 0,closeint\n",(unsigned int)pthread_self());
				pthread_exit(NULL);
			}
			//缩减
			if(p->thread_exitcode){
				--p->thread_alive;
				--p->thread_exitcode;
				pthread_mutex_unlock(&p->lock);
				printf("customer 0x%x exit\n",(unsigned int)pthread_self());
				pthread_exit(NULL);
			}
		}
		temp.business=p->list[p->list_rear].business;
		temp.arg=p->list[p->list_rear].arg;
		--(p->list_cur);
		p->list_rear=(p->list_rear+1)%p->list_max;
		++(p->thread_busy);
		pthread_mutex_unlock(&p->lock);
		pthread_cond_signal(&p->not_full);
		//执行任务
		temp.business(temp.arg);
		pthread_mutex_lock(&p->lock);
		--(p->thread_busy);
		pthread_mutex_unlock(&p->lock);
	}
	printf("customer 0x%x,shutdown its 0,closeint\n",(unsigned int)pthread_self());
	return NULL;
}
