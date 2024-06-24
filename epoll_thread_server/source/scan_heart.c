#include<epoll_thread_server.h>

void scan_heart(){
	std::map<int,time_t>::iterator ite;
	while(1){
		sleep(4);
		time_t tp=time(NULL);
		for(ite=heart_time_map.begin();ite!=heart_time_map.end();ite++){
			if(tp-ite->second>MAXTIME){
				printf("offline\n");
				SEND(ite->first,"offline\n",17,0);
				close(ite->first);
			}
		}
	}
}
