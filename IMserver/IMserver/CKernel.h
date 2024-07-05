#pragma once
#include "mediator/mediator.h"
#include "MySQL/CMySql.h"
#include "net/def.h"
#include <map>

//声明函数指针
//先声明，后定义
class CKernel;
typedef void(CKernel::* PFUN)(char* data, int len, long from);

class CKernel
{
public:
	//静态指针
	static CKernel* pKernel;

	CKernel();
	~CKernel();
	//打开服务器
	bool startServer();
	//关闭服务器
	void closeServer();

	//处理所有接收到的数据
	void dealData(char* data, int len, long from);
	//绑定协议头数组
	void setProrocolMap();
	//处理注册请求
	void dealRegisterRq(char* data, int len, long from);
	//处理登录请求
	void dealLoginRq(char* data, int len, long from);
	//处理聊天请求
	void dealChatRq(char* data, int len, long from);

	//获取用户的好友信息（包括自己）
	void getUserInfo(int userID);
	//根据用户id查询用户信息
	void getInfoById(int id, _STRU_FRIEND_INFO* info);

	//处理下线请求
	void dealOfflineRq(char* data, int len, long from);
	//处理添加好友请求
	void dealAddFriendRq(char* data, int len, long from);
	//处理加好友回复
	void dealAddFriendRs(char* data, int len, long from);

	//处理添加好友请求
	void dealCreateGroupRq(char* data, int len, long from);
	//处理加好友回复
	void dealAddGroupRq(char* data, int len, long from);
	//获取群组信息
	void getGroupInfo(int userID);
	//根据组id查询群友信息
	void getGroupInfoById(int id, _STRU_GROUP_INFO* info);
	//处理群聊请求
	void dealGroupChatRq(char* data, int len, long from);

private:
	mediator* m_pMediator;
	
	PFUN m_mapProrocol[_DEF_PROEOCOL_COUNT];

	//数据库对象
	CMySql m_mysql;

	std::map<int,SOCKET> m_mapIdToSocket;
};

