#pragma once
#include "mediator.h"

class UdpMediator :public mediator {
public:
	UdpMediator();
	~UdpMediator();

	//打开网络
	bool openNet();

	//发送数据
	bool sendData(char* data, int len, long to);

	//转发数据(把接收到的数据转发给kernel处理)
	void transmitData(char* data, int len, long from);

	//关闭网络
	void closeNet();
};