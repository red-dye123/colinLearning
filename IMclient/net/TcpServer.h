#pragma once
#include "net.h"
#include <list>
#include <map>

class TcpServer :public net {
public:
	TcpServer(mediator* pMediator);
	~TcpServer();

	//��ʼ������
	bool initNet();

	//�������ݣ�UDP����IP��ַ�������ݷ���˭��TCP��SOCKET�������ݷ��͸�˭��
	//IP��ulong���ͣ�SCOKET��uint
	bool sendData(char* data, int len, long to);

	//��������
	void recvData();

	//�ر�����
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