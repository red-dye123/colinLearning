#include "TcpServerMediator.h"
#include "../net/TcpServer.h"

TcpServerMediator::TcpServerMediator(){
	m_pNet = new TcpServer(this/*把main的外部指针传到内部*/);
}
TcpServerMediator::~TcpServerMediator(){
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
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

//转发数据(把接收到的数据转发给kernel处理)
void TcpServerMediator::transmitData(char* data, int len, long from) {
	//测试
	cout << "TcpServerMediator::transmitData:" << data << endl;
	char buf[] = "12312dasdad";
	sendData(buf, sizeof(buf), from);
}

//关闭网络
void TcpServerMediator::closeNet() {
	m_pNet->unInitNet();
}