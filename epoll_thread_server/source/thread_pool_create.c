#include <epoll_thread_server.h>

pool_t* thread_pool_create(int tmax,int tmin,int list_max){
	pool_t* pool=NULL;
	if((pool=(pool_t*)malloc(sizeof(pool_t)))==NULL){
		perror("thread_poll_create pool malloc failed");
		return NULL;
	}
	pool->thread_shutdown=1;
	pool->thread_max=tmax;
	pool->thread_min=tmin;
	pool->thread_alive=0;
	pool->thread_busy=0;
	pool->thread_exitcode=0;
	if((pool->list=(task_t*)malloc(sizeof(task_t)*list_max))==NULL){
		perror("thread_poll_create list malloc failed");
		return NULL;
	}
	pool->list_front=0;
	pool->list_rear=0;
	pool->list_cur=0;
	pool->list_max=list_max;
	if(pthread_mutex_init(&pool->lock,NULL)!=0 || pthread_cond_init(&pool->not_empty,NULL)!=0 || pthread_cond_init(&pool->not_full,NULL)!=0){
		printf("thread_pool_create init lock or cond error\n");
		return NULL;
	}
	if((pool->ctids=(pthread_t*)malloc(sizeof(pthread_t)*tmax))==NULL){
		perror("thread_pool_create init ctids failed");
		return NULL;
	}
	//初始化ctids
	bzero(pool->ctids,sizeof(pthread_t)*tmax);
	//预创建线程
	int err;
	for(int i=0;i<tmin;i++){
		if((err=pthread_create(&pool->ctids[i],NULL,customer_thread,(void*)pool))>0){
			printf("thread_pool_create,create_customer failed,%s\n",strerror(err));
			return NULL;
		}
		//pthread_mutex_lock(&pool->lock);
		++pool->thread_alive;
		//pthread_mutex_unlock(&pool->lock);
	}
	//创建管理者
	if((err=pthread_create(&pool->mid,NULL,manager_thread,(void*)pool))>0){
		printf("thread_pool_create,create manager failed,%s\n",strerror(err));
		return NULL;
	}
	return pool;
}
