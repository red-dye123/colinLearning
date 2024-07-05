#pragma once
#include "net.h"

class TcpClient:public net {
public:
	TcpClient(mediator* pMediator);
	~TcpClient();

	//��ʼ������
	bool initNet() ;

	//�������ݣ�UDP����IP��ַ�������ݷ���˭��TCP��SOCKET�������ݷ��͸�˭��
	//IP��ulong���ͣ�SCOKET��uint
	bool sendData(char* data, int len, long to);

	//��������
	void recvData();

	//�ر�����
	void unInitNet();

protected:
	static unsigned __stdcall recvThread(void* lpVoid);

private:
	HANDLE m_handle;
	bool m_bRunning;
};