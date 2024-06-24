#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
#include<ctype.h>
#include<time.h>
#include<fcntl.h>

#include<signal.h>
#include<pthread.h>
#include<sys/wait.h>

int SOCKET(int domain,int type,int protocol);
int BIND(int sockfd,const struct sockaddr* addr,socklen_t addrlen);
int LISTEN(int sockfd,int backlog);
int CONNECT(int sockfd,const struct sockaddr* addr,socklen_t addrlen);
int ACCEPT(int sockfd,struct sockaddr* addr,socklen_t* addrlen);
ssize_t RECV(int sockfd,void* buf,size_t len,int flags);
ssize_t SEND(int sockfd,const void* buf,size_t len,int flags);

int net_initializer();
void first_response(int sock,char* cip);

//业务处理模块
int business(int flags);

extern int client_sock_array[1024];
