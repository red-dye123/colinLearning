#include "../net/TcpServer.h"
#include "../CKernel.h"
#include "TcpServerMediator.h"

TcpServerMediator::TcpServerMediator() {
	m_pNet = new TcpServer(this);
}

TcpServerMediator::~TcpServerMediator(){
	if (m_pNet) {
		m_pNet = nullptr;
	}
}

//打开网络
bool TcpServerMediator::openNet() {
	//调用TcpServer的initNet
	return m_pNet->initNet();
}

//发送数据
bool TcpServerMediator::sendData(char* data, int len, long to) {
	return m_pNet->sendData(data,len,to);
}

//转发
void TcpServerMediator::transmitData(char* data, int len, long from) {
	CKernel::pKernel->dealData(data, len, from);
}

//关闭网络
void TcpServerMediator::closeNet() {
	m_pNet->unInitNet();
}