#include "TcpServer.h"
#include "def.h"
#include <process.h>
#include "../mediator/TcpServerMediator.h"


TcpServer::TcpServer(mediator* pMediator):m_bRunning(1) {
	//m_pMediator = new TcpServerMediator;	//ERROR:stack overflow
	//在main中已经有TcpMediator的对象了，让内部指针指向外部的对象
	m_pMediator = pMediator;
}

TcpServer::~TcpServer() {
}

//初始化网络：加载库、创建套接字、绑定、监听、创建接受连接的线程
bool TcpServer::initNet() {
	//1.加载库
	WORD versions = MAKEWORD(2, 2);
	WSADATA data;
	int err = WSAStartup(versions, &data);
	if (err != 0) {
		cout << "加载库失败" << endl;
		return 0;
	}
	if (HIBYTE(data.wVersion) != 2 || LOBYTE(data.wVersion) != 2) {
		cout << "版本号错误" << endl;
		return 0;
	}
	else {
		cout << "加载库成功" << endl;
	}

	//2.创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		cout << "创建套接字失败:" << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "创建套接字成功" << endl;
	}

	//3.绑定
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
	if (err == SOCKET_ERROR) {
		cout << "绑定失败：" << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "绑定成功" << endl;
	}

	//4.监听
	err = listen(m_sock, 10);
	if (err == SOCKET_ERROR) {
		cout << "监听失败:" << WSAGetLastError() << endl;
		return 0;
	}
	else {
		cout << "监听成功" << endl;
	}

	//5.创建接收连接的线程
	HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &acceptThread, this, 0, nullptr);
	if (handle) {
		m_listHandle.push_back(handle);
	}

	return 1;
}

//接收连接的线程
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
			//连接成功
			cout << "客户端ip:" << inet_ntoa(addrClient.sin_addr) << endl;
		
			//创建接收数据的线程
			HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &recvThread, pthis, 0, &threadId);
			if (handle) {
				pthis->m_listHandle.push_back(handle);
			}

			//保存socket
			pthis->m_mapThreadIdToSocket[threadId] = sockTalk;
		}
	}

	return 1;
}

//接收数据线程
unsigned __stdcall TcpServer::recvThread(void* lpVoid) {
	TcpServer* pthis = (TcpServer*)lpVoid;
	pthis->recvData();
	return 1;
}

//发送数据
bool TcpServer::sendData(char* data, int len, long to) {
	//1.校验参数
	if (!data || len <= 0) {
		return 0;
	}

	//2.先发包大小(避免粘包问题)
	if (send(to, (char*)&len, sizeof(int), 0) <= 0) {
		cout << "sendData error1:" << WSAGetLastError() << endl;
		return 0;
	}

	//3.再发包内容
	if (send(to, data, len, 0) <= 0) {
		cout << "sendData error2:" << WSAGetLastError() << endl;
		return 0;
	}

	return 1;
}

//接收数据
void TcpServer::recvData() {
	//休眠一会，以防map中还没有保存socket
	Sleep(10);

	//从map中取出当前线程对应的socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
	if (!sock || sock == INVALID_SOCKET) {
		cout << "recvData socket error" << endl;
		return;
	}

	int nRecvNum = 0;
	int packSize = 0;
	int nOffset = 0;	//偏移量，记录一个包累计接收到多少数据
	while (m_bRunning) {
		nRecvNum = recv(sock, (char*)&packSize, sizeof(int), 0);
		if (nRecvNum > 0) {
			//接收数据成功，知道包大小了，按照包大小准备空间
			char* recvBuf = new char[packSize];

			//接收包内容
			while (packSize > 0) {
				nRecvNum = recv(sock, recvBuf + nOffset/*开始写入的地址*/, packSize/*最大能写入的数据*/, 0);
				if (nRecvNum > 0) {
					nOffset += nRecvNum;
					packSize -= nRecvNum;
				}
				else {
					cout << "recvData error2：" << WSAGetLastError() << endl;
					break;
				}
			}
			//TODO:接收包内容成功，把数据传给中介者
			m_pMediator->transmitData(recvBuf, nOffset, sock);

			//偏移量清零
			nOffset = 0;
		}
		else {
			cout << "recvData error1：" << WSAGetLastError() << endl;
			break;
		}
	}
}

//关闭网络
void TcpServer::unInitNet() {
	//1.结束线程
	m_bRunning = 0;
	for (auto ite = m_listHandle.begin();ite!=m_listHandle.end(); ){
		HANDLE handle=*ite;
		if (WaitForSingleObject(handle, 1000) == WAIT_TIMEOUT) {
			TerminateThread(handle, -1);
		}
		CloseHandle(handle);
		handle = nullptr;
		//从list中移除无用的空间
		ite = m_listHandle.erase(ite);
	}
	

	//2.关闭套接字
	if (!m_sock && m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
	}
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
		SOCKET sock = ite->second;
		if (!sock && sock != INVALID_SOCKET) {
			closesocket(sock);
		}
		//从map中移除无效节点
		ite = m_mapThreadIdToSocket.erase(ite);
	}

	//3.卸载库
	WSACleanup();
}