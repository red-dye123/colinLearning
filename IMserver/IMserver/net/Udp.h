#pragma once
#include "net.h"

class Udp :public net {
public:
	Udp();
	~Udp();

	//��ʼ������
	bool initNet();

	//�������ݣ�UDP����IP��ַ�������ݷ���˭��TCP��SOCKET�������ݷ��͸�˭��
	//IP��ulong���ͣ�SCOKET��uint
	bool sendData(char* data, int len, long to);

	//��������
	void recvData();

	//�ر�����
	void unInitNet();

};