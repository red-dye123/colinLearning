#pragma once
#include "mediator.h"

class TcpServerMediator :public mediator {
public:
	TcpServerMediator();
	~TcpServerMediator();

	//打开网络
	bool openNet();

	//发送数据
	bool sendData(char* data, int len, long to);

	//转发
	void transmitData(char* data, int len, long from);

	//关闭网络
	void closeNet();
};