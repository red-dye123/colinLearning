#include "TcpClientMediator.h"
#include "../net/TcpClient.h"

TcpClientMediator::TcpClientMediator() {
	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator() {
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

//������
bool TcpClientMediator::openNet() {
	//����TcpClient��initNet
	return m_pNet->initNet();
}

//��������
bool TcpClientMediator::sendData(char* data, int len, long to) {
	return m_pNet->sendData(data,len,to);
}

//ת������(�ѽ��յ�������ת����kernel����)
void TcpClientMediator::transmitData(char* data, int len, long from) {
	cout << "TcpClientMediator::transmitData:" << data << endl;
}

//�ر�����
void TcpClientMediator::closeNet() {
	m_pNet->unInitNet();
}