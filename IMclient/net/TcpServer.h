#pragma once
#include "net.h"
#include <list>
#include <map>

class TcpServer :public net {
public:
	TcpServer(mediator* pMediator);
	~TcpServer();

	//初始化网络
	bool initNet();

	//发送数据（UDP里由IP地址决定数据发给谁，TCP由SOCKET决定数据发送给谁）
	//IP是ulong类型，SCOKET是uint
	bool sendData(char* data, int len, long to);

	//接收数据
	void recvData();

	//关闭网络
	void unInitNet();

protected:
	static unsigned __stdcall acceptThread(void* lpVoid);
	static unsigned __stdcall recvThread(void* lpVoid);
	
private:
	//HANDLE m_handle;
	bool m_bRunning;
	list<HANDLE> m_listHandle;
	map<unsigned int, SOCKET> m_mapThreadIdToSocket;
};