#pragma once
#include "net.h"

class Udp :public net {
public:
	Udp();
	~Udp();

	//初始化网络
	bool initNet();

	//发送数据（UDP里由IP地址决定数据发给谁，TCP由SOCKET决定数据发送给谁）
	//IP是ulong类型，SCOKET是uint
	bool sendData(char* data, int len, long to);

	//接收数据
	void recvData();

	//关闭网络
	void unInitNet();

};