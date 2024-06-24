#include <epoll_thread_server.h>

int if_thread_alive(pthread_t tid){
	int err;
	err=pthread_kill(tid,0);
	if(err==ESRCH)
		return 0;
	else
		return 1;
}
