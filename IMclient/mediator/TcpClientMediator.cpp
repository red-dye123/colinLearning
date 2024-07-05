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

//打开网络
bool TcpClientMediator::openNet() {
	//调用TcpClient的initNet
	return m_pNet->initNet();
}

//发送数据
bool TcpClientMediator::sendData(char* data, int len, long to) {
	return m_pNet->sendData(data,len,to);
}

//转发数据(把接收到的数据转发给kernel处理)
void TcpClientMediator::transmitData(char* data, int len, long from) {
    Q_EMIT sig_transmitData(data,len,from);
}

//关闭网络
void TcpClientMediator::closeNet() {
	m_pNet->unInitNet();
}


/*
QT的信号和槽的使用(两个之间通知和数据传递)
1.两个都要继承自QObject，并且要有Q_OBJECT的宏
2.发送通知或者数据的类中，在头文件使用signals声明信号(返回值是void，参数需要传输的数据)，信号不是函数不需要实现
  在需要发通知或者数据的地方使用Q_EMIT 信号名(参数列表)发送信号
3.在接收数据的类中，使用slots声明一个槽函数(返回值和参数要和信号保持一致)，在cpp中实现槽函数
4.绑定信号和槽函数，在接收数据的类中，发送信号的对象new出来以后
 */
