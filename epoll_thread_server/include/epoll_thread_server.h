#pragma once
#include<mysock.h>
#include<sys/epoll.h>
#include<map>

//红黑树
extern int epfd;

//任务类型
typedef struct{
	void* (*business)(void*);//任务地址
	void* arg;//任务参数
}task_t;

//线程池类型
typedef struct{
	int thread_shutdown;//线程池开关
	int thread_max;//线程最大值
	int thread_min;//线程最小值
	int thread_alive;//有效数量
	int thread_busy;//繁忙线程
	int thread_exitcode;//缩减码
	task_t* list;//任务队列
	int list_front;//队列头索引
	int list_rear;//队列尾索引
	int list_max;//队列最大值
	int list_cur;//当前任务数
	pthread_mutex_t lock;//互斥锁
	pthread_cond_t not_full;//生产者条件变量
	pthread_cond_t not_empty;//消费者条件变量
	pthread_t* ctids;//存储消费者tid
	pthread_t mid;//存储管理者id
}pool_t;

void* data_business(void*);//数据处理业务(client_sock就绪)
void* accept_business(void*);//连接业务(server_sock就绪)
pool_t* thread_pool_create(int tmax,int tmin,int list_max);//线程池创建初始化，对线程进行预创建(消费者管理者)
int producer_add_task(pool_t*,task_t);//生产者调用此模块，添加一次任务
int epoll_initializer(int sock);//创建初始化epoll模型,并且设置第一次监听server_scok
int epoll_listen(int sock,pool_t*);//监听模块，负责监听sock是否就绪并按规则添加任务
void* customer_thread(void*);//消费者线程工作，等待于任务队列，持续获取任务并执行
void* manager_thread(void*);//管理者线程工作，等待于阈值，根据条件判断扩容缩减 管理线程数量
int if_thread_alive(pthread_t tid);//线程失效返回0,存活返回1

extern std::map<int,time_t> heart_time_map;
#define MAXTIME 5

