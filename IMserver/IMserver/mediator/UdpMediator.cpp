#include "UdpMediator.h"

UdpMediator::UdpMediator() {}
UdpMediator::~UdpMediator() {}

//打开网络
bool UdpMediator::openNet() {
	return 1;
}

//发送数据
bool UdpMediator::sendData(char* data, int len, long to) {
	return 1;
}

//转发数据(把接收到的数据转发给kernel处理)
void UdpMediator::transmitData(char* data, int len, long from) {

}

//关闭网络
void UdpMediator::closeNet() {

}