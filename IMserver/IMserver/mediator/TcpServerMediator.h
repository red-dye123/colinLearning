#pragma once
#include "mediator.h"

class TcpServerMediator :public mediator {
public:
	TcpServerMediator();
	~TcpServerMediator();

	//������
	bool openNet();

	//��������
	bool sendData(char* data, int len, long to);

	//ת��
	void transmitData(char* data, int len, long from);

	//�ر�����
	void closeNet();
};