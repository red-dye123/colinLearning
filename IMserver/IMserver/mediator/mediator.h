#pragma once

//��������ʹ��
class net;

class mediator {
public:
	mediator(){}
	virtual ~mediator(){}

	//������
	virtual bool openNet() = 0;

	//��������
	virtual bool sendData(char* data, int len, long to) =0;

	//ת������(�ѽ��յ�������ת����kernel����)
	virtual void transmitData(char* data, int len, long from) = 0;

	//�ر�����
	virtual void closeNet() = 0;
protected:
	net* m_pNet;
};