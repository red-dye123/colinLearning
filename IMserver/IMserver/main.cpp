#include <iostream>
#include <Windows.h>
#include "CKernel.h"

using namespace std;

int main() {

	//测试：先开启服务端，再开启客户端
	CKernel kernel;
	if (!kernel.startServer()) {
		cout << "打开服务端失败" << endl;
		return 1;
	}

	/*
	* mediator* pServer = new TcpServerMediator;
	mediator* pClient = new TcpClientMediator;
	if (!pClient->openNet()) {
		cout << "打开客户端失败" << endl;
		return 1;
	}
	//客户端给服务端发消息
	char buf[] = "hello world";
	pClient->sendData(buf, sizeof(buf), 0);
	*/

	//让程序一直运行
	while (1) {
		Sleep(5000);
		cout << "server is running" << endl;
	}

	return 0;
}

/*
项目分析:
IM聊天系统
通信协议:TCP协议
模型:C/S模型

功能:
注册
登录
获取好友列表
聊天
添加好友
下线


面相对象编程:类
客户端QT:
net类(收发数据、初始化网络、关闭网络)
中介者类(为了以后扩展使用)
kernel类(处理接收到的数据、组织要发送的数据
ui类(注册&登录界面、好友列表界面、聊天窗口)

服务端VS:
net类(收发数据、初始化网络、关闭网络)
中介者类(为了以后扩展使用)
kernel类(处理接收到的数据、组织要发送的数据)
MySq1类


net类(继承和多态)
父类(收发数据、初始化网络、关闭网络)
TCP子类(TCP服务端、TCP客户端)(收发数据、初始化网络、关闭网络)
UDP子类(收发数据、初始化网络、关闭网络)

*/

/*
总结
一、存在的问题
1、可以使用单例模式来保证只new一个类，只产生一个窗口
2、心跳机制保持长连接。（传输层、应用层）
3、代码支持广域网通信，但是实际操作需要公网ip——租借公网服务器，把服务端部署在公网服务器上
4、使用的是windows下的多线程阻塞模型。操作系统会给每个进程分配4G的内存空间，0-2G是用户空间，2G-4G是内核空间。每个线程默认1M堆栈，最多就能创建2k个左右的线程，只能能服务2k个客户端，

二、扩展
1、发文件、自定义&系统自带的表情包
2、群聊
3、游戏对战
4、网络爬虫（新闻推送）
*/