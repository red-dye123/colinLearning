#include "TcpServerMediator.h"
#include "../net/TcpServer.h"

TcpServerMediator::TcpServerMediator(){
	m_pNet = new TcpServer(this/*��main���ⲿָ�봫���ڲ�*/);
}
TcpServerMediator::~TcpServerMediator(){
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

//������
bool TcpServerMediator::openNet() {
	//����TcpServer��initNet
	return m_pNet->initNet();
}

//��������
bool TcpServerMediator::sendData(char* data, int len, long to) {
	return m_pNet->sendData(data,len,to);
}

//ת������(�ѽ��յ�������ת����kernel����)
void TcpServerMediator::transmitData(char* data, int len, long from) {
	//����
	cout << "TcpServerMediator::transmitData:" << data << endl;
	char buf[] = "12312dasdad";
	sendData(buf, sizeof(buf), from);
}

//�ر�����
void TcpServerMediator::closeNet() {
	m_pNet->unInitNet();
}