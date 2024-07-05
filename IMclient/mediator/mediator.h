#pragma once
#include <QObject>

//先声明后使用
class net;

class mediator:public QObject {
    Q_OBJECT
public:
    mediator();
    virtual ~mediator();

	//打开网络
	virtual bool openNet() = 0;

	//发送数据
	virtual bool sendData(char* data, int len, long to) = 0;

	//转发数据(把接收到的数据转发给kernel处理)
	virtual void transmitData(char* data, int len, long from) = 0;

	//关闭网络
	virtual void closeNet() = 0;

signals:
    //把接收到的数据传给kernel
    void sig_transmitData(char* data,int len,long from);

protected:
	net* m_pNet;
};
