#include "TcpServer.h"
#include "def.h"
#include <process.h>
#include "../mediator/TcpServerMediator.h"


TcpServer::TcpServer(mediator* pMediator):m_bRunning(1) {
	//m_pMediator = new TcpServerMediator;	//ERROR:stack overflow
	//��main���Ѿ���TcpMediator�Ķ����ˣ����ڲ�ָ��ָ���ⲿ�Ķ���
	m_pMediator = pMediator;
}

TcpServer::~TcpServer() {
}

//��ʼ�����磺���ؿ⡢�����׽��֡��󶨡������������������ӵ��߳�
bool TcpServer::initNet() {
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
		return 0;
	}
	else {
		cout << "���ؿ�ɹ�" << endl;
	}

	//2.�����׽���
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		cout << "�����׽���ʧ��:" << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "�����׽��ֳɹ�" << endl;
	}

	//3.��
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
	if (err == SOCKET_ERROR) {
		cout << "��ʧ�ܣ�" << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "�󶨳ɹ�" << endl;
	}

	//4.����
	err = listen(m_sock, 10);
	if (err == SOCKET_ERROR) {
		cout << "����ʧ��:" << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "�����ɹ�" << endl;
	}

	//5.�����������ӵ��߳�
	HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &acceptThread, this, 0, nullptr);
	if (handle) {
		m_listHandle.push_back(handle);
	}

	return 1;
}

//�������ӵ��߳�
unsigned __stdcall TcpServer::acceptThread(void* lpVoid) {
	TcpServer* pthis = (TcpServer*)lpVoid;
	sockaddr_in addrClient;
	int addrClientSize = sizeof(addrClient);
	unsigned int threadId = 0;
	while (pthis->m_bRunning) {
		SOCKET sockTalk = accept(pthis->m_sock, (sockaddr*)&addrClient, &addrClientSize);
		if (sockTalk == INVALID_SOCKET) {
			cout << "accept error:" << WSAGetLastError() << endl;
		}
		else {
			//���ӳɹ�
			cout << "�ͻ���ip:" << inet_ntoa(addrClient.sin_addr) << endl;
		
			//�����������ݵ��߳�
			HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &recvThread, pthis, 0, &threadId);
			if (handle) {
				pthis->m_listHandle.push_back(handle);
			}

			//����socket
			pthis->m_mapThreadIdToSocket[threadId] = sockTalk;
		}
	}

	return 1;
}

//���������߳�
unsigned __stdcall TcpServer::recvThread(void* lpVoid) {
	TcpServer* pthis = (TcpServer*)lpVoid;
	pthis->recvData();
	return 1;
}

//��������
bool TcpServer::sendData(char* data, int len, long to) {
	//1.У�����
	if (!data || len <= 0) {
		return 0;
	}

	//2.�ȷ�����С(����ճ������)
	if (send(to, (char*)&len, sizeof(int), 0) <= 0) {
		cout << "sendData error1:" << WSAGetLastError() << endl;
		return 0;
	}

	//3.�ٷ�������
	if (send(to, data, len, 0) <= 0) {
		cout << "sendData error2:" << WSAGetLastError() << endl;
		return 0;
	}

	return 1;
}

//��������
void TcpServer::recvData() {
	//����һ�ᣬ�Է�map�л�û�б���socket
	Sleep(10);

	//��map��ȡ����ǰ�̶߳�Ӧ��socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
	if (!sock || sock == INVALID_SOCKET) {
		cout << "recvData socket error" << endl;
		return;
	}

	int nRecvNum = 0;
	int packSize = 0;
	int nOffset = 0;	//ƫ��������¼һ�����ۼƽ��յ���������
	while (m_bRunning) {
		nRecvNum = recv(sock, (char*)&packSize, sizeof(int), 0);
		if (nRecvNum > 0) {
			//�������ݳɹ���֪������С�ˣ����հ���С׼���ռ�
			char* recvBuf = new char[packSize];

			//���հ�����
			while (packSize > 0) {
				nRecvNum = recv(sock, recvBuf + nOffset/*��ʼд��ĵ�ַ*/, packSize/*�����д�������*/, 0);
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
			m_pMediator->transmitData(recvBuf, nOffset, sock);

			//ƫ��������
			nOffset = 0;
		}
		else {
			cout << "recvData error1��" << WSAGetLastError() << endl;
			break;
		}
	}
}

//�ر�����
void TcpServer::unInitNet() {
	//1.�����߳�
	m_bRunning = 0;
	for (auto ite = m_listHandle.begin();ite!=m_listHandle.end(); ){
		HANDLE handle=*ite;
		if (WaitForSingleObject(handle, 1000) == WAIT_TIMEOUT) {
			TerminateThread(handle, -1);
		}
		CloseHandle(handle);
		handle = nullptr;
		//��list���Ƴ����õĿռ�
		ite = m_listHandle.erase(ite);
	}
	

	//2.�ر��׽���
	if (!m_sock && m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
	}
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
		SOCKET sock = ite->second;
		if (!sock && sock != INVALID_SOCKET) {
			closesocket(sock);
		}
		//��map���Ƴ���Ч�ڵ�
		ite = m_mapThreadIdToSocket.erase(ite);
	}

	//3.ж�ؿ�
	WSACleanup();
}