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
//�򿪷�����
bool CKernel::startServer() {
	//1.������
	if (!m_pMediator->openNet()) {
		cout << "openNet error" << endl;
		return 0;
	}

	//2.�������ݿ�
	if (!m_mysql.ConnectMySql((char*)"127.0.0.1", (char*)"root", (char*)"123456", (char*)"kelin_ism")) {
		cout << "�������ݿ�ʧ��" << endl;
		return 0;
	}

	return 1;
}
//�رշ�����
void CKernel::closeServer() {
	//�ر�����
	if (m_pMediator) {
		m_pMediator->closeNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}

	//TODO:�ر����ݿ�
}

//�������н��յ�������
void CKernel::dealData(char* data, int len, long from) {
	cout << __func__ << endl;
	//ȡ��Э��ͷ
	PackType type = *(PackType*)data;

	//����������±�
	int index = type - _DEF_PROTOCOL_BASE - 1;

	//�ж������±��Ƿ�����Ч��Χ��
	if (index >= 0 && index < _DEF_PROEOCOL_COUNT) {
		//ȡ������ָ��
		PFUN pf = m_mapProrocol[index];
		if (pf) {
			(this->*pf)(data, len, from);
		}
		else {
			//1.û�а�Э��ͷ������ 2.�ṹ��Э��ͷ��ֵ����
			cout << "type2:" << type << endl;
		}
	}
	else {
		//1.�ṹ��Э��ͷ��ֵ���� 2.�������������⣬����offsetû������
		cout << "type1:" << type << endl;
	}

}

//��Э��ͷ����
void CKernel::setProrocolMap() {
	cout << __func__ << endl;
	//��ʼ������
	memset(m_mapProrocol, 0, sizeof(m_mapProrocol));
	//�����鸳ֵ
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

//����ע������
void CKernel::dealRegisterRq(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.���
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;

	//2.У�����ݺϷ��ԣ�����Ϳͻ���һ�£�

	//3.���ݵ绰��������ݿ��ѯ�绰��
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select tel from t_user where tel = '%s';", rq->tel);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}

	//4.�жϽ����ѯ����Ƿ�Ϊ��
	_STRU_REGISTER_RS rs;
	if (lstRes.size() != 0) {
		//��ѯ���ˣ�˵���绰���Ѿ���ע����ˣ�ע��ʧ��
		rs.result = _register_tel_exists;
	}
	else {
		//5.�����ǳƲ�ѯ�ǳ�
		sprintf(szSql, "select name from t_user where name = '%s';", rq->name);
		if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
			cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
			return;
		}
		//6.�жϽ����ѯ�Ƿ�Ϊ��
		if (lstRes.size() != 0) {
			//��ѯ���ˣ�˵���ǳ��Ѿ���ע����ˣ�ע��ʧ��
			rs.result = _register_name_exists;
		}
		else {
			//7.û�в�ѯ������ע����Ϣд�����ݿ�
			sprintf(szSql, "insert into t_user (name,tel,password,iconid,feeling) values ('%s','%s','%s',6,'�����̫���ˣ�ʲôҲû������');", rq->name, rq->tel, rq->password);
			if (!m_mysql.UpdateMySql(szSql)) {
				cout << "�������ݿ�ʧ�ܣ�" << szSql << endl;
				return;
			}
			//8.ע��ɹ�
			rs.result = _register_success;
		}
	}
	//9.�����Ƿ�ɹ�����Ҫ�ѽ�����ظ��ͻ���
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//�����¼����
void CKernel::dealLoginRq(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.���
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	
	//2.���ݵ绰��������ݿ��ѯ����
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select password, id from t_user where tel = '%s';", rq->tel);
	if (!m_mysql.SelectMySql(szSql, 2, lstRes)) {	//д��������Ϣ
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}
	//3.�жϽ���Ƿ�Ϊ��
	_STRU_LOGIN_RS rs;
	if (lstRes.size() == 0) {
		//������Ϊ�գ�˵���绰����û��ע�ᣬ��¼ʧ��
		rs.result = _login_tel_not_exists;
	}
	else {
		//ȡ����ѯ�����루ȡ��������Ҫ���б���ɾ����
		string pass = lstRes.front();
		lstRes.pop_front();
		//ȡ���û�id
		int userID = stoi(lstRes.front());	//string to int
		lstRes.pop_front();

		//4.�����Ϊ�գ��Ƚϲ�ѯ����������û����������
		if (strcmp(rq->password, pass.c_str()) != 0) {
			//���벻��ȣ���¼ʧ��
			rs.result = _login_password_error;
		}
		else {
			//5.������ȣ���¼�ɹ�
			rs.result = _login_success;
			rs.userID = userID;

			//���浱ǰ��¼�û���socket
			m_mapIdToSocket[userID] = from;

			//6.���۵�¼�Ƿ�ɹ�����Ҫ���ͻ��˻ظ���¼���
			m_pMediator->sendData((char*)&rs, sizeof(rs), from);

			//��ȡ��ǰ�����б�
			getUserInfo(userID);

			//��ȡ��ǰȺ���б�
			getGroupInfo(userID);

			return;
		}
	}
	//6.���۵�¼�Ƿ�ɹ�����Ҫ���ͻ��˻ظ���¼���
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//��ȡ�û��ĺ�����Ϣ�������Լ���
void CKernel::getUserInfo(int userID) {
	cout << __func__ << endl;
	//�����Լ���id�����ݿ��ѯ�Լ�����Ϣ
	_STRU_FRIEND_INFO userinfo;
	getInfoById(userID, &userinfo);

	//���Լ�����Ϣ�����ͻ��ˡ���ȡ��һ����һ������������Ӵ�����⣬���Һͷ���Ч�ʶ���һЩ
	if (m_mapIdToSocket.find(userID) != m_mapIdToSocket.end()) {
		m_pMediator->sendData((char*)&userinfo, sizeof(userinfo), m_mapIdToSocket[userID]);
	}

	//�Ӻ��ѹ�ϵ���в�ѯ���ѵ�id�б�
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select idB from t_friend where idA= '%d';",userID);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}

	//��������id�б�
	int friendId = 0;
	_STRU_FRIEND_INFO friendinfo;
	while (lstRes.size() > 0) {
		//ȡ������id
		friendId = stoi(lstRes.front());
		lstRes.pop_front();

		//���ݺ���id��ѯ������Ϣ
		getInfoById(friendId, &friendinfo);

		//�Ѻ��ѵ���Ϣ�����ͻ���
		if (m_mapIdToSocket.find(userID) != m_mapIdToSocket.end()) {
			m_pMediator->sendData((char*)&friendinfo, sizeof(friendinfo), m_mapIdToSocket[userID]);
		}

		//֪ͨ�������ߺ��ѣ��Լ�������
		if (m_mapIdToSocket.count(friendId) > 0) {
			m_pMediator->sendData((char*)&userinfo, sizeof(userinfo), m_mapIdToSocket[friendId]);
		}

		//��ȡ���Ѹ��Լ�����������Ϣ
		list<string> lstResMess;
		sprintf(szSql, "select message from t_fmessage where sender_id= '%d';", friendId);
		if (!m_mysql.SelectMySql(szSql, 1, lstResMess)) {
			cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
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

//�����û�id��ѯ�û���Ϣ
void CKernel::getInfoById(int id, _STRU_FRIEND_INFO* info) {
	cout << __func__ << endl;
	info->userID = id;
	if (m_mapIdToSocket.count(id) > 0) {
		//�û�����
		info->status = _status_online;
	}
	else {
		//�û�������
		info->status = _status_offline;
	}

	//����id�����ݿ��ѯ�û����ǳơ�ǩ����ͷ��id
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select name, feeling,iconId from t_user where id= '%d';", id);
	if (!m_mysql.SelectMySql(szSql, 3, lstRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}
	if (3 == lstRes.size()) {
		//ȡ���ǳ�
		strcpy(info->name, lstRes.front().c_str());
		lstRes.pop_front();
		//ȡ��ǩ��
		strcpy(info->feeling, lstRes.front().c_str());
		lstRes.pop_front();
		//ȡ��ͷ��id
		info->iconID = stoi(lstRes.front());
		lstRes.pop_front();
	}
}

//������������
void CKernel::dealChatRq(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.���
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;

	//2.�жϺ����Ƿ�����
	if (m_mapIdToSocket.count(rq->friendID) > 0) {
		m_pMediator->sendData(data, len, m_mapIdToSocket[rq->friendID]);
	}
	else {
		//���Ѳ����ߣ����������ݴ������ݿ���Ⱥ��������ٷ�����
		list<string> lstRes;
		char szSql[1024] = "";
		sprintf(szSql, "insert into t_fmessage (sender_id,receiver_id,message) values('%d','%d','%s');", rq->userID, rq->friendID,rq->content);
		if (!m_mysql.UpdateMySql(szSql)) {
			cout << "�������ݿ�ʧ�ܣ�" << szSql << endl;
		}
		//���Ѳ����ߣ�����ʧ��
		_STRU_CHAT_RS rs;
		rs.result = _send_fail;
		rs.friendId = rq->friendID;
		m_pMediator->sendData((char*)&rs,sizeof(rs),from);

	}
}

//������Ӻ�������
void CKernel::dealAddFriendRq(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.���
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

	//2.�����ǳƲ�ѯ�û�id
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select id from t_user where name='%s';", rq->friendName);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {	//��Ӱ�������
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}

	_STRU_ADD_FRIEND_RS rs;
	strcpy_s(rs.friendName, rq->friendName);
	//3.�жϽ���Ƿ�Ϊ��
	if (lstRes.size() == 0) {
		//4.���Ϊ�գ�˵���û������ڣ����ʧ��
		rs.result = _add_friend_not_exists;
	}
	else {
		//ȡ���û�id
		int friendId = stoi(lstRes.front());
		lstRes.pop_front();
		//5.�����Ϊ�գ��ж��û��Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0) {
			//6.�û����ߣ����û�ת����Ӻ�������
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
			return;
		}
		else {
			//7.�û������ߣ���Ӻ���ʧ��
			rs.result = _add_friend_offline;
		}
	}
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}

//������Ӻ��ѻظ�
void CKernel::dealAddFriendRs(char* data, int len, long from) {
	cout << __func__ << endl;
	//1.���
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;

	//2.�ж���ӽ���Ƿ�ͬ��
	if (rs->result == _add_friend_success) {
		//3.�Ѻ��ѹ�ϵд�����ݿ�
		char szSql[1024] = "";
		sprintf(szSql, "insert into t_friend values('%d','%d');", rs->aId, rs->bId);	//ʵ���������붼�ɹ���������������ߴ�����
		if (!m_mysql.UpdateMySql(szSql)) {
			cout << "�������ݿ�ʧ�ܣ�"  << endl;
			return;
		}
		sprintf(szSql, "insert into t_friend values('%d','%d');", rs->bId, rs->aId);
		if (!m_mysql.UpdateMySql(szSql)) {
			cout << "�������ݿ�ʧ�ܣ�" << szSql << endl;
			return;
		}
		
		//����
		getUserInfo(rs->aId);

	}

	if (m_mapIdToSocket.count(rs->aId) > 0) {
		m_pMediator->sendData(data, len, m_mapIdToSocket[rs->aId]);
	}

}

void CKernel::dealCreateGroupRq(char* data, int len, long from)
{
	cout << __func__ << endl;
	//1.���
	_STRU_CREATE_GROUP_RQ* rq = (_STRU_CREATE_GROUP_RQ*)data;

	//2.����Ⱥ����ѯȺid
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select group_id from t_group where group_name='%s';", rq->groupName);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {	//��Ӱ�������
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}

	_STRU_CREATE_GROUP_RS rs;
	strcpy_s(rs.groupName, rq->groupName);
	rs.userID = rq->userID;
	strcpy_s(rs.userName, rq->userName);

	
	//3.�жϽ���Ƿ�Ϊ��
	if (lstRes.size() != 0) {
		//4.�鵽�ˣ����ܴ���
		rs.result = _create_group_fail;
	}
	else {
		sprintf(szSql, "insert into t_group (group_name,user_id,user_name,iconid) values('%s','%d','%s','9');", rq->groupName, rq->userID, rq->userName);
		if (!m_mysql.UpdateMySql(szSql)) {
			cout << "�������ݿ�1ʧ�ܣ�" << szSql << endl;
		}
		else {
			sprintf(szSql, "select group_id from t_group where group_name='%s';", rq->groupName);
			if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
				cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
			}
			else if (lstRes.size()>0) {
				rs.groupId = stoi(lstRes.front());
				lstRes.pop_front();
				sprintf(szSql, "insert into t_groupmember (group_id,user_id) values('%d','%d');", rs.groupId, rs.userID);
				if (!m_mysql.UpdateMySql(szSql)) {
					cout << "�������ݿ�2ʧ�ܣ�" << szSql << endl;
				}
				else
					rs.result = _create_group_success;
			}
		}
		
	}
	cout << send << endl;
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	//����
	getGroupInfo(rs.userID);
}

void CKernel::dealAddGroupRq(char* data, int len, long from)
{
	cout << __func__ << endl;
	//1.���
	_STRU_ADD_GROUP_RQ* rq = (_STRU_ADD_GROUP_RQ*)data;

	//2.����Ⱥ����ѯȺid
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select group_id from t_group where group_name='%s';", rq->groupName);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {	//��Ӱ�������
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}

	_STRU_ADD_GROUP_RS rs;
	strcpy_s(rs.groupName, rq->groupName);

	//3.�жϽ���Ƿ�Ϊ��
	if (lstRes.size() == 0) {
		//4.û�鵽
		rs.result = _add_group_not_exists;
	}
	else {
		rs.groupId = stoi(lstRes.front());
		lstRes.pop_front();

		sprintf(szSql, "select user_id from t_groupmember where group_id = '%d';", rs.groupId);
		if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
			cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		}
		else {
			sprintf(szSql, "insert into t_groupmember (group_id,user_id) values('%d','%d');", rs.groupId, rq->userID);
			if (!m_mysql.UpdateMySql(szSql)) {
				cout << "�������ݿ�2ʧ�ܣ�" << szSql << endl;
			}
			else {
				//���������б�
				int friendId = 0;
				while (lstRes.size() > 0) {
					//ȡ����id
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
	//����
	getGroupInfo(rq->userID);
}

void CKernel::getGroupInfo(int userID)
{
	cout << __func__ << endl;

	//����Ա��ϵ���в�ѯȺ��id�б�
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select group_id from t_groupmember where user_id = '%d';", userID);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}

	//������id�б�
	int groupId = 0;
	_STRU_GROUP_INFO groupinfo;
	while (lstRes.size() > 0) {
		//ȡ����id
		groupId = stoi(lstRes.front());
		lstRes.pop_front();

		//������id��ѯ����Ϣ
		getGroupInfoById(groupId, &groupinfo);

		//��Ⱥ�����Ϣ�����ͻ���
		if (m_mapIdToSocket.find(userID) != m_mapIdToSocket.end()) {
			m_pMediator->sendData((char*)&groupinfo, sizeof(groupinfo), m_mapIdToSocket[userID]);
		}

		//��ȡ�����Ⱥ��Ϣ
		list<string> lstResMess;
		sprintf(szSql, "select message,sender_id from t_gmessage where group_id= '%d';", groupId);
		if (!m_mysql.SelectMySql(szSql, 2, lstResMess)) {
			cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
			return;
		}

		_STRU_GROUP_CHAT_RQ data;
		data.groupID = groupId;
		list<string> lstResFN;
		while (lstResMess.size() > 0) {
			cout << "gm" << endl;
			strcpy(data.content, lstResMess.front().c_str());
			lstResMess.pop_front();
			//ȡ������id
			data.userID = stoi(lstResMess.front());
			lstResMess.pop_front();
			sprintf(szSql, "select name from t_user where id= '%d';", data.userID);
			if (!m_mysql.SelectMySql(szSql, 1, lstResFN)) {
				cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
				return;
			}
			if (1 == lstResFN.size()) {
				//ȡ���ǳ�
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

	//����id�����ݿ��ѯȺ����ͷ��id
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select group_name, iconId from t_group where group_id= '%d';", id);
	if (!m_mysql.SelectMySql(szSql, 2, lstRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}
	if (2 == lstRes.size()) {
		//ȡ���ǳ�
		strcpy(info->groupName, lstRes.front().c_str());
		lstRes.pop_front();
		//ȡ��ͷ��id
		info->iconID = stoi(lstRes.front());
		lstRes.pop_front();
	}
}

void CKernel::dealGroupChatRq(char* data, int len, long from)
{
	cout << __func__  << endl;
	//1.���
	_STRU_GROUP_CHAT_RQ* rq = (_STRU_GROUP_CHAT_RQ*)data;
	//2.��Ⱥ��
	list<string> lstRes;
	char szSql[1024] = "";
	sprintf(szSql, "select user_id from t_groupmember where group_id = '%d';", rq->groupID);
	if (!m_mysql.SelectMySql(szSql, 1, lstRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << szSql << endl;
		return;
	}
	int friendID;
	while (lstRes.size() > 0) {
		//ȡ������id
		friendID = stoi(lstRes.front());
		lstRes.pop_front();
		//3.�жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendID) > 0 && friendID!=rq->userID) {
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendID]);
		}
		else {
			//�������ߣ���Ϣ����
			list<string> lstRes;
			char szSql[1024] = "";
			sprintf(szSql, "insert into t_gmessage (sender_id,group_id,message) values('%d','%d','%s');", rq->userID, rq->groupID, rq->content);
			if (!m_mysql.UpdateMySql(szSql)) {
				cout << "�������ݿ�ʧ�ܣ�" << szSql << endl;
			}
		}
	}
}

//������������
void CKernel::dealOfflineRq(char* data, int len, long from) {
	//���
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;
	//�Һ���id
	list<string> lst;
	char szSql[1024] = "";
	sprintf(szSql, "select idB from t_friend where idA = '%d';", rq->userID);
	if (!m_mysql.SelectMySql(szSql, 1, lst)) {
		cout << "test maysql offline faile" << endl;
		return;
	}
	//�����ߵķ���������
	int fId;
	while (lst.size()) {
		fId = stoi(lst.front());
		lst.pop_front();
		if (m_mapIdToSocket.count(fId) > 0) {
			m_pMediator->sendData(data, len, m_mapIdToSocket[fId]);
		}
	}
	//����
	if (m_mapIdToSocket.count(rq->userID)) {
		closesocket(m_mapIdToSocket[rq->userID]);
		m_mapIdToSocket.erase(rq->userID);
	}
	
}