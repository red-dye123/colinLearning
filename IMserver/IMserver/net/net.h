#pragma once
#include<WinSock2.h>
#include <iostream>


using namespace std;

#pragma comment(lib,"Ws2_32.lib")

//先声明后使用
//解决头文件互相包含的问题
//先声明有mediator这个类，直接使用，一会就编译到了
class mediator;

class net {
public:
	net():m_sock(INVALID_SOCKET) {}
	virtual ~net() {}

	virtual bool initNet() = 0;

	//发送数据（UDP里由IP地址决定数据发给谁，TCP由SOCKET决定数据发送给谁）
	//IP是ulong类型，SCOKET是uint
	virtual bool sendData(char* data,int len,long to) = 0;

	//接收数据
	virtual void recvData() = 0;

	//关闭网络
	virtual void unInitNet() = 0;

protected:
	SOCKET m_sock;
	mediator* m_pMediator;

};