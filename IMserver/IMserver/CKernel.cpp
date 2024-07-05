#include <iostream>
using namespace std;

#include "CKernel.h"
#include "mediator/TcpServerMediator.h"

CKernel* CKernel::pKernel = nullptr;

CKernel::CKernel() {
	pKernel = this;
	m_pMediator = new TcpServerMediator;
	setProrocolMap();
}
CKernel::~CKernel() {
	if (m_pMediator) {
		m_pMediator->closeNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}
}
//打开服务器
bool CKernel::startServer() {
	//1.打开网络
	if (!m_pMediator->openNet()) {
		cout << "openNet error" << endl;
		return 0;
	}

	//2.连接数据库
	if (!m_mysql.ConnectMySql((char*)"127.0.0.1", (char*)"root", (char*)"123456", (char*)"kelin_ism")) {
		cout << "连接数据库失败" << endl;
		return 0;
	}

	return 1;
}
//关闭服务器
void CKernel::closeServer() {
	//关闭网络
	if (m_pMediator) {
		m_pMediator->closeNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}

	//TODO:关闭数据库
}

//处理所有接收到的数据
void CKernel::dealData(char* data, int len, long from) {
	cout << __func__ << endl;
	//取出协议头
	PackType type = *(PackType*)data;

	//计算出数组下标
	int index = type - _DEF_PROTOCOL_BASE - 1;

	//判断数组下标是否在有效范围内
	if (index >= 0 && index < _DEF_PROEOCOL_COUNT) {
		//取出函数指针
		PFUN pf = m_mapProrocol[index];
		if (pf) {
			(this->*pf)(data, len, from);
		}
		else {
			//1.没有绑定协议头和数组 2.结构体协议头赋值错误
			cout << "type2:" << type << endl;
		}
	}
	else {
		//1.结构体协议头赋值错误 2.接收数据有问题，可能offset没有清零
		cout << "type1:" << type << endl;
	}

}

//绑定协议头数组
void CKernel::setProrocolMap() {
	cout << __func__ << endl;
	//初始化数组
	memset(m_mapProrocol, 0, sizeof(m_mapProrocol));
	//给数组赋值
	m_mapProrocol[_DEF_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;
	m_mapProrocol[_DEF_LOGIN_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_mapProrocol[_DEF_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_mapProrocol[_DEF_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_mapProrocol[_DEF_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_mapProrocol[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;

	m_mapProrocol[_DEF_CREATE_GROUP_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealCreateGroupRq;
	m_mapProrocol[_DEF_ADD_GROUP_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddGroupRq;
	m_mapProrocol[_DEF_GROUP_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealGroupChatRq;
}

//处理注册请求
void CKernel::dealRegisterRq(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.拆包
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;

	//2.校验数据合法性（规则和客户端一致）

	//3.根据电话号码从数据库查询电话号
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select tel from t_user where tel = '%s';", rq->tel);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}

	//4.判断结果查询结果是否为空
	_STRU_REGISTER_RS rs;
	if (lstRes.size() != 0) {
		//查询到了，说明电话号已经被注册过了，注册失败
		rs.result = _register_tel_exists;
	}
	else {
		//5.根据昵称查询昵称
		sprintf(szSql, "select name from t_user where name = '%s';", rq->name);
		if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
			cout << "查询数据库失败：" << szSql << endl;
			return;
		}
		//6.判断结果查询是否为空
		if (lstRes.size() != 0) {
			//查询到了，说明昵称已经被注册过了，注册失败
			rs.result = _register_name_exists;
		}
		else {
			//7.没有查询到，把注册信息写入数据库
			sprintf(szSql, "insert into t_user (name,tel,password,iconid,feeling) values ('%s','%s','%s',6,'这个人太懒了，什么也没有留下');", rq->name, rq->tel, rq->password);
			if (!m_mysql.UpdateMySql(szSql)) {
				cout << "更新数据库失败：" << szSql << endl;
				return;
			}
			//8.注册成功
			rs.result = _register_success;
		}
	}
	//9.不论是否成功，都要把结果发回给客户端
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//处理登录请求
void CKernel::dealLoginRq(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.拆包
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	
	//2.根据电话号码从数据库查询密码
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select password, id from t_user where tel = '%s';", rq->tel);
	if (!m_mysql.SelectMySql(szSql, 2, lstRes)) {	//写入两列信息
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}
	//3.判断结果是否为空
	_STRU_LOGIN_RS rs;
	if (lstRes.size() == 0) {
		//如果结果为空，说明电话号码没有注册，登录失败
		rs.result = _login_tel_not_exists;
	}
	else {
		//取出查询的密码（取出的数据要从列表中删除）
		string pass = lstRes.front();
		lstRes.pop_front();
		//取出用户id
		int userID = stoi(lstRes.front());	//string to int
		lstRes.pop_front();

		//4.结果不为空，比较查询到的密码和用户输入的密码
		if (strcmp(rq->password, pass.c_str()) != 0) {
			//密码不相等，登录失败
			rs.result = _login_password_error;
		}
		else {
			//5.密码相等，登录成功
			rs.result = _login_success;
			rs.userID = userID;

			//保存当前登录用户的socket
			m_mapIdToSocket[userID] = from;

			//6.无论登录是否成功，都要给客户端回复登录结果
			m_pMediator->sendData((char*)&rs, sizeof(rs), from);

			//获取当前好友列表
			getUserInfo(userID);

			//获取当前群组列表
			getGroupInfo(userID);

			return;
		}
	}
	//6.无论登录是否成功，都要给客户端回复登录结果
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//获取用户的好友信息（包括自己）
void CKernel::getUserInfo(int userID) {
	cout << __func__ << endl;
	//根据自己的id从数据库查询自己的信息
	_STRU_FRIEND_INFO userinfo;
	getInfoById(userID, &userinfo);

	//把自己的信息发给客户端。采取查一个发一个，解决数据庞大的问题，查找和发送效率都高一些
	if (m_mapIdToSocket.find(userID) != m_mapIdToSocket.end()) {
		m_pMediator->sendData((char*)&userinfo, sizeof(userinfo), m_mapIdToSocket[userID]);
	}

	//从好友关系表中查询好友的id列表
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select idB from t_friend where idA= '%d';",userID);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}

	//遍历好友id列表
	int friendId = 0;
	_STRU_FRIEND_INFO friendinfo;
	while (lstRes.size() > 0) {
		//取出好友id
		friendId = stoi(lstRes.front());
		lstRes.pop_front();

		//根据好友id查询好友信息
		getInfoById(friendId, &friendinfo);

		//把好友的信息发给客户端
		if (m_mapIdToSocket.find(userID) != m_mapIdToSocket.end()) {
			m_pMediator->sendData((char*)&friendinfo, sizeof(friendinfo), m_mapIdToSocket[userID]);
		}

		//通知所有在线好友，自己上线了
		if (m_mapIdToSocket.count(friendId) > 0) {
			m_pMediator->sendData((char*)&userinfo, sizeof(userinfo), m_mapIdToSocket[friendId]);
		}

		//获取好友给自己发的离线消息
		list<string> lstResMess;
		sprintf(szSql, "select message from t_fmessage where sender_id= '%d';", friendId);
		if (!m_mysql.SelectMySql(szSql, 1, lstResMess)) {
			cout << "查询数据库失败：" << szSql << endl;
			return;
		}
		
		_STRU_CHAT_RQ data;
		data.userID = friendId;
		data.friendID = userID;
		while (lstResMess.size() > 0) {
			strcpy(data.content, lstResMess.front().c_str());
			lstResMess.pop_front();
			m_pMediator->sendData((char*)&data, sizeof(data), m_mapIdToSocket[userID]);
		}
	}
}

//根据用户id查询用户信息
void CKernel::getInfoById(int id, _STRU_FRIEND_INFO* info) {
	cout << __func__ << endl;
	info->userID = id;
	if (m_mapIdToSocket.count(id) > 0) {
		//用户在线
		info->status = _status_online;
	}
	else {
		//用户不在线
		info->status = _status_offline;
	}

	//根据id从数据库查询用户的昵称、签名、头像id
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select name, feeling,iconId from t_user where id= '%d';", id);
	if (!m_mysql.SelectMySql(szSql, 3, lstRes)) {
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}
	if (3 == lstRes.size()) {
		//取出昵称
		strcpy(info->name, lstRes.front().c_str());
		lstRes.pop_front();
		//取出签名
		strcpy(info->feeling, lstRes.front().c_str());
		lstRes.pop_front();
		//取出头像id
		info->iconID = stoi(lstRes.front());
		lstRes.pop_front();
	}
}

//处理聊天请求
void CKernel::dealChatRq(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.拆包
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;

	//2.判断好友是否在线
	if (m_mapIdToSocket.count(rq->friendID) > 0) {
		m_pMediator->sendData(data, len, m_mapIdToSocket[rq->friendID]);
	}
	else {
		//好友不在线，把聊天内容存在数据库里，等好友上线再发给他
		list<string> lstRes;
		char szSql[1024] = "";
		sprintf(szSql, "insert into t_fmessage (sender_id,receiver_id,message) values('%d','%d','%s');", rq->userID, rq->friendID,rq->content);
		if (!m_mysql.UpdateMySql(szSql)) {
			cout << "更新数据库失败：" << szSql << endl;
		}
		//好友不在线，发送失败
		_STRU_CHAT_RS rs;
		rs.result = _send_fail;
		rs.friendId = rq->friendID;
		m_pMediator->sendData((char*)&rs,sizeof(rs),from);

	}
}

//处理添加好友请求
void CKernel::dealAddFriendRq(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

	//2.根据昵称查询用户id
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select id from t_user where name='%s';", rq->friendName);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {	//受影响的行数
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}

	_STRU_ADD_FRIEND_RS rs;
	strcpy_s(rs.friendName, rq->friendName);
	//3.判断结果是否为空
	if (lstRes.size() == 0) {
		//4.结果为空，说明用户不存在，添加失败
		rs.result = _add_friend_not_exists;
	}
	else {
		//取出用户id
		int friendId = stoi(lstRes.front());
		lstRes.pop_front();
		//5.结果不为空，判断用户是否在线
		if (m_mapIdToSocket.count(friendId) > 0) {
			//6.用户在线，给用户转发添加好友请求
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
			return;
		}
		else {
			//7.用户不在线，添加好友失败
			rs.result = _add_friend_offline;
		}
	}
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//处理添加好友回复
void CKernel::dealAddFriendRs(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;

	//2.判断添加结果是否同意
	if (rs->result == _add_friend_success) {
		//3.把好友关系写入数据库
		char szSql[1024] = "";
		sprintf(szSql, "insert into t_friend values('%d','%d');", rs->aId, rs->bId);	//实现两条插入都成功，可以用事务或者触发器
		if (!m_mysql.UpdateMySql(szSql)) {
			cout << "插入数据库失败："  << endl;
			return;
		}
		sprintf(szSql, "insert into t_friend values('%d','%d');", rs->bId, rs->aId);
		if (!m_mysql.UpdateMySql(szSql)) {
			cout << "插入数据库失败：" << szSql << endl;
			return;
		}
		
		//更新
		getUserInfo(rs->aId);

	}

	if (m_mapIdToSocket.count(rs->aId) > 0) {
		m_pMediator->sendData(data, len, m_mapIdToSocket[rs->aId]);
	}

}

void CKernel::dealCreateGroupRq(char* data, int len, long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_CREATE_GROUP_RQ* rq = (_STRU_CREATE_GROUP_RQ*)data;

	//2.根据群名查询群id
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select group_id from t_group where group_name='%s';", rq->groupName);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {	//受影响的行数
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}

	_STRU_CREATE_GROUP_RS rs;
	strcpy_s(rs.groupName, rq->groupName);
	rs.userID = rq->userID;
	strcpy_s(rs.userName, rq->userName);

	
	//3.判断结果是否为空
	if (lstRes.size() != 0) {
		//4.查到了，不能创建
		rs.result = _create_group_fail;
	}
	else {
		sprintf(szSql, "insert into t_group (group_name,user_id,user_name,iconid) values('%s','%d','%s','9');", rq->groupName, rq->userID, rq->userName);
		if (!m_mysql.UpdateMySql(szSql)) {
			cout << "更新数据库1失败：" << szSql << endl;
		}
		else {
			sprintf(szSql, "select group_id from t_group where group_name='%s';", rq->groupName);
			if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
				cout << "查询数据库失败：" << szSql << endl;
			}
			else if (lstRes.size()>0) {
				rs.groupId = stoi(lstRes.front());
				lstRes.pop_front();
				sprintf(szSql, "insert into t_groupmember (group_id,user_id) values('%d','%d');", rs.groupId, rs.userID);
				if (!m_mysql.UpdateMySql(szSql)) {
					cout << "更新数据库2失败：" << szSql << endl;
				}
				else
					rs.result = _create_group_success;
			}
		}
		
	}
	cout << send << endl;
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	//更新
	getGroupInfo(rs.userID);
}

void CKernel::dealAddGroupRq(char* data, int len, long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_GROUP_RQ* rq = (_STRU_ADD_GROUP_RQ*)data;

	//2.根据群名查询群id
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select group_id from t_group where group_name='%s';", rq->groupName);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {	//受影响的行数
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}

	_STRU_ADD_GROUP_RS rs;
	strcpy_s(rs.groupName, rq->groupName);

	//3.判断结果是否为空
	if (lstRes.size() == 0) {
		//4.没查到
		rs.result = _add_group_not_exists;
	}
	else {
		rs.groupId = stoi(lstRes.front());
		lstRes.pop_front();

		sprintf(szSql, "select user_id from t_groupmember where group_id = '%d';", rs.groupId);
		if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
			cout << "查询数据库失败：" << szSql << endl;
		}
		else {
			sprintf(szSql, "insert into t_groupmember (group_id,user_id) values('%d','%d');", rs.groupId, rq->userID);
			if (!m_mysql.UpdateMySql(szSql)) {
				cout << "更新数据库2失败：" << szSql << endl;
			}
			else {
				//遍历好友列表
				int friendId = 0;
				while (lstRes.size() > 0) {
					//取出组id
					friendId = stoi(lstRes.front());
					lstRes.pop_front();

					if (m_mapIdToSocket.count(friendId) > 0) {
						getGroupInfo(friendId);
					}

				}
			}
		}
	}
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	//更新
	getGroupInfo(rq->userID);
}

void CKernel::getGroupInfo(int userID)
{
	cout << __func__ << endl;

	//从组员关系表中查询群的id列表
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select group_id from t_groupmember where user_id = '%d';", userID);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}

	//遍历组id列表
	int groupId = 0;
	_STRU_GROUP_INFO groupinfo;
	while (lstRes.size() > 0) {
		//取出组id
		groupId = stoi(lstRes.front());
		lstRes.pop_front();

		//根据组id查询组信息
		getGroupInfoById(groupId, &groupinfo);

		//把群组的信息发给客户端
		if (m_mapIdToSocket.find(userID) != m_mapIdToSocket.end()) {
			m_pMediator->sendData((char*)&groupinfo, sizeof(groupinfo), m_mapIdToSocket[userID]);
		}

		//获取缓存的群消息
		list<string> lstResMess;
		sprintf(szSql, "select message,sender_id from t_gmessage where group_id= '%d';", groupId);
		if (!m_mysql.SelectMySql(szSql, 2, lstResMess)) {
			cout << "查询数据库失败：" << szSql << endl;
			return;
		}

		_STRU_GROUP_CHAT_RQ data;
		data.groupID = groupId;
		list<string> lstResFN;
		while (lstResMess.size() > 0) {
			cout << "gm" << endl;
			strcpy(data.content, lstResMess.front().c_str());
			lstResMess.pop_front();
			//取出好友id
			data.userID = stoi(lstResMess.front());
			lstResMess.pop_front();
			sprintf(szSql, "select name from t_user where id= '%d';", data.userID);
			if (!m_mysql.SelectMySql(szSql, 1, lstResFN)) {
				cout << "查询数据库失败：" << szSql << endl;
				return;
			}
			if (1 == lstResFN.size()) {
				//取出昵称
				strcpy(data.friendName, lstResFN.front().c_str());
				lstResFN.pop_front();
			}


			m_pMediator->sendData((char*)&data, sizeof(data), m_mapIdToSocket[userID]);
		}
	}
}

void CKernel::getGroupInfoById(int id, _STRU_GROUP_INFO* info)
{
	cout << __func__ << endl;
	info->groupID = id;

	//根据id从数据库查询群名、头像id
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select group_name, iconId from t_group where group_id= '%d';", id);
	if (!m_mysql.SelectMySql(szSql, 2, lstRes)) {
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}
	if (2 == lstRes.size()) {
		//取出昵称
		strcpy(info->groupName, lstRes.front().c_str());
		lstRes.pop_front();
		//取出头像id
		info->iconID = stoi(lstRes.front());
		lstRes.pop_front();
	}
}

void CKernel::dealGroupChatRq(char* data, int len, long from)
{
	cout << __func__  << endl;
	//1.拆包
	_STRU_GROUP_CHAT_RQ* rq = (_STRU_GROUP_CHAT_RQ*)data;
	//2.找群友
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select user_id from t_groupmember where group_id = '%d';", rq->groupID);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
		cout << "查询数据库失败：" << szSql << endl;
		return;
	}
	int friendID;
	while (lstRes.size() > 0) {
		//取出好友id
		friendID = stoi(lstRes.front());
		lstRes.pop_front();
		//3.判断好友是否在线
		if (m_mapIdToSocket.count(friendID) > 0 && friendID!=rq->userID) {
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendID]);
		}
		else {
			//好友离线，信息缓存
			list<string> lstRes;
			char szSql[1024] = "";
			sprintf(szSql, "insert into t_gmessage (sender_id,group_id,message) values('%d','%d','%s');", rq->userID, rq->groupID, rq->content);
			if (!m_mysql.UpdateMySql(szSql)) {
				cout << "更新数据库失败：" << szSql << endl;
			}
		}
	}
}

//处理下线请求
void CKernel::dealOfflineRq(char* data, int len, long from) {
	//拆包
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;
	//找好友id
	list<string> lst;
	char szSql[1024] = "";
	sprintf(szSql, "select idB from t_friend where idA = '%d';", rq->userID);
	if (!m_mysql.SelectMySql(szSql, 1, lst)) {
		cout << "test maysql offline faile" << endl;
		return;
	}
	//给在线的发下线请求
	int fId;
	while (lst.size()) {
		fId = stoi(lst.front());
		lst.pop_front();
		if (m_mapIdToSocket.count(fId) > 0) {
			m_pMediator->sendData(data, len, m_mapIdToSocket[fId]);
		}
	}
	//下线
	if (m_mapIdToSocket.count(rq->userID)) {
		closesocket(m_mapIdToSocket[rq->userID]);
		m_mapIdToSocket.erase(rq->userID);
	}
	
}