#include "TcpClient.h"
#include <process.h>
#include "def.h"
#include "../mediator/TcpClientMediator.h"

TcpClient::TcpClient(mediator* pMediator):m_handle(nullptr),m_bRunning(1) {
	m_pMediator = pMediator;
}

TcpClient::~TcpClient(){
}

//��ʼ�����磺���ؿ⡢�����׽��֡����ӷ���ˡ������������ݵ��̣߳����������У�
bool TcpClient::initNet() {
	//1.���ؿ�
	WORD versions = MAKEWORD(2, 2);
	WSADATA data;
	int err = WSAStartup(versions, &data);
	if (err != 0) {
		cout << "���ؿ�ʧ��" << endl;
		return 0;
	}
	if (HIBYTE(data.wVersion) != 2 || LOBYTE(data.wVersion) != 2) {
		cout << "�汾�Ŵ���" << endl;
		WSACleanup();
		return 1;
	}
	else {
		cout << "���ؿ�ɹ�" << endl;
	}

	//2.�����׽���
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		cout << "�����׽���ʧ�ܣ�" << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "�����׽��ֳɹ�" << endl;
	}

	//3.���ӷ�����
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	// ħ������
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_SERVER_IP);
	err = connect(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
	if (err == SOCKET_ERROR) {
		cout << "���Ӵ���" << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "���ӳɹ�" << endl;
	}

	//4.�����������ݵ��̣߳����������У�
	//CreateThread��ExitThread��һ�ԣ�������߳�ʹ����C++����ʱ��ĺ���������strcpy�������˿ռ䵫û���ͷţ�ExitThreadҲ�����ͷſռ䣬�����Ӿͻ�����ڴ�й¶
	//ʹ��_beginthreadex��_endthreadex��_endthreadex���Ȼ��տռ䣬�ٵ���ExitThread
	m_handle = (HANDLE)_beginthreadex(nullptr, 0/*ʹ���߳�Ĭ�϶�ջ��С1M*/, &recvThread/*��Ҫ�ж������&�����Բ��þ�̬*/, this/*��̬���ܵ��÷Ǿ�̬�����԰�thisָ�봫��ȥ*/, 0/*����������*/, nullptr);

	return 1;
}

//�̺߳������������ݣ�
unsigned __stdcall TcpClient::recvThread(void* lpVoid){
	TcpClient* pthis = (TcpClient*)lpVoid;/*ע���void*ǿת*/
	pthis->recvData();
	return 1;
}

//��������
bool TcpClient::sendData(char* data, int len, long to) {
	//1.У�����
	if (!data || len <= 0) {
		return 0;
	}

	//2.�ȷ�����С(����ճ������)
	if (send(m_sock,(char*)&len,sizeof(int),0) <= 0) {
		cout << "sendData error1" << WSAGetLastError() << endl;
		return 0;
	}

	//3.�ٷ�������
	if (send(m_sock,data,len,0) <= 0) {
		cout << "sendData error1" << WSAGetLastError() << endl;
		return 0;
	}

	return 1;
}

//�������ݣ�ѭ����������
void TcpClient::recvData() {
	int nRecvNum = 0;
	int packSize = 0;
	int nOffset = 0;	//ƫ��������¼һ�����ۼƽ��յ���������
	while (m_bRunning) {
		nRecvNum = recv(m_sock, (char*)&packSize, sizeof(int), 0);
		if (nRecvNum > 0) {
			//�������ݳɹ���֪������С�ˣ����հ���С׼���ռ�
			char* recvBuf = new char[packSize];
			
			//���հ�����
			while (packSize>0) {
				nRecvNum = recv(m_sock, recvBuf + nOffset/*��ʼд��ĵ�ַ*/, packSize/*�����д�������*/, 0);
				if (nRecvNum > 0) {
					nOffset += nRecvNum;
					packSize -= nRecvNum;
				}
				else {
					cout << "recvData error2��" << WSAGetLastError() << endl;
					break;
				}
			}
			//TODO:���հ����ݳɹ��������ݴ����н���
			m_pMediator->transmitData(recvBuf, nOffset, m_sock);

			//ƫ��������
			nOffset = 0;
		}
		else {
			cout << "recvData error1��" << WSAGetLastError() << endl;
			break;
		}
	}
}

//�ر����磺�����̡߳��ر��׽��֡�ж�ؿ�
void TcpClient::unInitNet() {
	/*
	�����߳�ʱ������ϵͳ���ÿ���̷߳��䣺�ں˶��󡢾���������̣߳����߳�id����־�������ü���+2
	�������̾��������ü������0�������̹߳��������ü���-1���رվ�������ü���-1
	������ǿ��ɱ���̣߳��ᵼ������������
	*/
	//1.�����߳�
	m_bRunning = 0;
	if (WaitForSingleObject(m_handle, 1000)==WAIT_TIMEOUT) {
		TerminateThread(m_handle,-1/*�˳���*/);
	}
	CloseHandle(m_handle);
	m_handle = nullptr;

	//2.�ر��׽���
	if (!m_sock && m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
	}

	//3.ж�ؿ�
	WSACleanup();
}


