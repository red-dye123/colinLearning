#include "UdpMediator.h"

UdpMediator::UdpMediator() {}
UdpMediator::~UdpMediator() {}

//������
bool UdpMediator::openNet() {
	return 1;
}

//��������
bool UdpMediator::sendData(char* data, int len, long to) {
	return 1;
}

//ת������(�ѽ��յ�������ת����kernel����)
void UdpMediator::transmitData(char* data, int len, long from) {

}

//�ر�����
void UdpMediator::closeNet() {

}