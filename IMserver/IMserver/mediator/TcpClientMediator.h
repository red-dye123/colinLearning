#pragma once
#include "mediator.h"

class TcpClientMediator :public mediator {
public:
	TcpClientMediator();
	~TcpClientMediator();

	//������
	bool openNet();

	//��������
	bool sendData(char* data, int len, long to);

	//ת������(�ѽ��յ�������ת����kernel����)
	void transmitData(char* data, int len, long from);

	//�ر�����
	void closeNet();
};