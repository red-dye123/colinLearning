#pragma once
#include<WinSock2.h>
#include <iostream>


using namespace std;

#pragma comment(lib,"Ws2_32.lib")

//��������ʹ��
//���ͷ�ļ��������������
//��������mediator����ֱ࣬��ʹ�ã�һ��ͱ��뵽��
class mediator;

class net {
public:
	net():m_sock(INVALID_SOCKET) {}
	virtual ~net() {}

	virtual bool initNet() = 0;

	//�������ݣ�UDP����IP��ַ�������ݷ���˭��TCP��SOCKET�������ݷ��͸�˭��
	//IP��ulong���ͣ�SCOKET��uint
	virtual bool sendData(char* data,int len,long to) = 0;

	//��������
	virtual void recvData() = 0;

	//�ر�����
	virtual void unInitNet() = 0;

protected:
	SOCKET m_sock;
	mediator* m_pMediator;

};