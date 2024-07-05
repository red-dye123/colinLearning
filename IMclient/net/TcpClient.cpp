#include "TcpClient.h"
#include <process.h>
#include "def.h"
#include "../mediator/TcpClientMediator.h"

TcpClient::TcpClient(mediator* pMediator):m_handle(nullptr),m_bRunning(1) {
	m_pMediator = pMediator;
}

TcpClient::~TcpClient(){
}

//初始化网络：加载库、创建套接字、连接服务端、创建接收数据的线程（创建即运行）
bool TcpClient::initNet() {
	//1.加载库
	WORD versions = MAKEWORD(2, 2);
	WSADATA data;
	int err = WSAStartup(versions, &data);
	if (err != 0) {
        cout << "WSAStartup fail" << endl;
		return 0;
	}
	if (HIBYTE(data.wVersion) != 2 || LOBYTE(data.wVersion) != 2) {
        cout << "version error" << endl;
		WSACleanup();
		return 1;
	}
	else {
        cout << "WSAStartup success" << endl;
	}

	//2.创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
        cout << "socket eror:" << WSAGetLastError() << endl;
		return 0;
	}
	else {
        cout << "socket success" << endl;
	}

	//3.连接服务器
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	// 魔鬼数字
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_SERVER_IP);
	err = connect(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
	if (err == SOCKET_ERROR) {
        cout << "connect error:" << WSAGetLastError() << endl;
		return 0;
	}
	else {
        cout << "connect success" << endl;
	}

	//4.创建接收数据的线程（创建即运行）
	//CreateThread和ExitThread是一对，如果在线程使用了C++运行时库的函数（例如strcpy，申请了空间但没有释放）ExitThread也不会释放空间，这样子就会造成内存泄露
	//使用_beginthreadex和_endthreadex，_endthreadex会先回收空间，再调用ExitThread
	m_handle = (HANDLE)_beginthreadex(nullptr, 0/*使用线程默认堆栈大小1M*/, &recvThread/*需要有对象才能&，所以采用静态*/, this/*静态不能调用非静态，所以把this指针传进去*/, 0/*创建即运行*/, nullptr);

	return 1;
}

//线程函数（接收数据）
unsigned __stdcall TcpClient::recvThread(void* lpVoid){
	TcpClient* pthis = (TcpClient*)lpVoid;/*注意把void*强转*/
	pthis->recvData();
	return 1;
}

//发送数据
bool TcpClient::sendData(char* data, int len, long to) {
	//1.校验参数
	if (!data || len <= 0) {
		return 0;
	}

	//2.先发包大小(避免粘包问题)
	if (send(m_sock,(char*)&len,sizeof(int),0) <= 0) {
		cout << "sendData error1" << WSAGetLastError() << endl;
		return 0;
	}

	//3.再发包内容
	if (send(m_sock,data,len,0) <= 0) {
		cout << "sendData error1" << WSAGetLastError() << endl;
		return 0;
	}

	return 1;
}

//接收数据：循环接收数据
void TcpClient::recvData() {
	int nRecvNum = 0;
	int packSize = 0;
	int nOffset = 0;	//偏移量，记录一个包累计接收到多少数据
	while (m_bRunning) {
		nRecvNum = recv(m_sock, (char*)&packSize, sizeof(int), 0);
		if (nRecvNum > 0) {
			//接收数据成功，知道包大小了，按照包大小准备空间
			char* recvBuf = new char[packSize];
			
			//接收包内容
			while (packSize>0) {
				nRecvNum = recv(m_sock, recvBuf + nOffset/*开始写入的地址*/, packSize/*最大能写入的数据*/, 0);
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
			m_pMediator->transmitData(recvBuf, nOffset, m_sock);

			//偏移量清零
			nOffset = 0;
		}
		else {
			cout << "recvData error1：" << WSAGetLastError() << endl;
			break;
		}
	}
}

//关闭网络：结束线程、关闭套接字、卸载库
void TcpClient::unInitNet() {
	/*
	创建线程时，操作系统会给每个线程分配：内核对象、句柄（操作线程）、线程id（标志），引用计数+2
	结束进程就是让引用技术变成0：结束线程工作，引用计数-1；关闭句柄，引用计数-1
	不允许强制杀死线程，会导致死锁等问题
	*/
	//1.结束线程
	m_bRunning = 0;
	if (WaitForSingleObject(m_handle, 1000)==WAIT_TIMEOUT) {
		TerminateThread(m_handle,-1/*退出码*/);
	}
	CloseHandle(m_handle);
	m_handle = nullptr;

	//2.关闭套接字
	if (!m_sock && m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
	}

	//3.卸载库
	WSACleanup();
}


