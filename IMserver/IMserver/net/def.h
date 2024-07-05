#pragma once
#include <set>

//�����ŷ�ֹ����ʱ���ּ���˳������
#define _DEF_TCP_PORT (67890)
#define _DEF_TCP_SERVER_IP ("127.0.0.1")

//����Э��ͷ
#define _DEF_PROTOCOL_BASE	(1000)

#define _DEF_LEGNTH			(100)
#define _DEF_CONTENT_LENGTH (4096)
#define _DEF_PROEOCOL_COUNT	(16)

//ע������
#define _DEF_REGISTER_RQ	(_DEF_PROTOCOL_BASE + 1)
//ע��ظ�
#define _DEF_REGISTER_RS	(_DEF_PROTOCOL_BASE + 2)
//��¼����
#define _DEF_LOGIN_RQ		(_DEF_PROTOCOL_BASE + 3)
//��¼�ظ�
#define _DEF_LOGIN_RS		(_DEF_PROTOCOL_BASE + 4)
//��ȡ�����б�
#define _DEF_FRIEND_INFO	(_DEF_PROTOCOL_BASE + 5)
//��������
#define _DEF_CHAT_RQ		(_DEF_PROTOCOL_BASE + 6)
//����ظ�
#define _DEF_CHAT_RS		(_DEF_PROTOCOL_BASE + 7)
//��Ӻ�������
#define _DEF_ADD_FRIEND_RQ	(_DEF_PROTOCOL_BASE + 8)
//��Ӻ��ѻظ�
#define _DEF_ADD_FRIEND_RS	(_DEF_PROTOCOL_BASE + 9)
//��������
#define _DEF_OFFLINE_RQ		(_DEF_PROTOCOL_BASE + 10)

//����Ⱥ������
#define _DEF_CREATE_GROUP_RQ	(_DEF_PROTOCOL_BASE + 11)
//��Ⱥ�ظ�
#define _DEF_CREATE_GROUP_RS	(_DEF_PROTOCOL_BASE + 12)
//����Ⱥ������
#define _DEF_ADD_GROUP_RQ	(_DEF_PROTOCOL_BASE + 13)
//��Ⱥ�ظ�
#define _DEF_ADD_GROUP_RS	(_DEF_PROTOCOL_BASE + 14)
//��ȡȺ���б�
#define _DEF_GROUP_INFO		(_DEF_PROTOCOL_BASE + 15)
//Ⱥ������
#define _DEF_GROUP_CHAT_RQ	(_DEF_PROTOCOL_BASE + 16)



//�������
//ע����
#define _register_success		(0)
#define _register_tel_exists	(1)
#define _register_name_exists	(2)
//��¼���
#define _login_success			(0)
#define _login_tel_not_exists	(1)
#define _login_password_error	(2)
//�û�״̬
#define _status_online			(0)
#define _status_offline			(1)
//���ͽ��
#define _send_success			(0)
#define _send_fail				(1)
//��Ӻ��ѽ��
#define _add_friend_success		(0)
#define _add_friend_not_exists	(1)
#define _add_friend_offline		(2)
#define _add_friend_refuse		(3)

//��Ⱥ���
#define _create_group_success	(0)
#define _create_group_fail		(1)
//��Ⱥ���
#define _add_group_success		(0)
#define _add_group_not_exists	(1)
#define _add_group_refuse		(2)
//Ⱥ�����
#define _group_send_success			(0)
#define _group_send_fail			(1)


//����Э��ͷ����
typedef int PackType;

//����ṹ��
//ע������Э��ͷ���绰�š��ǳơ�����
typedef struct _STRU_REGISTER_RQ {
	_STRU_REGISTER_RQ() :type(_DEF_REGISTER_RQ) {
		memset(tel, 0, _DEF_LEGNTH);
		memset(name,1, _DEF_LEGNTH);
		memset(password, 0, _DEF_LEGNTH);
	}
	PackType type;
	char tel[_DEF_LEGNTH];
	char name[_DEF_LEGNTH];
	char password[_DEF_LEGNTH];
}_STRU_REGISTER_RQ;

//ע��ظ���Э��ͷ��ע������ע��ɹ����绰�ѱ�ע�ᡢ�ǳ��ѱ�ע�ᣩ
typedef struct _STRU_REGISTER_RS {
	_STRU_REGISTER_RS() :type(_DEF_REGISTER_RS), result(_register_name_exists) {}
	PackType type;
	int result;
}_STRU_REGISTER_RS;

//��¼����Э��ͷ���绰���롢����
typedef struct _STRU_LOGIN_RQ {
	_STRU_LOGIN_RQ() :type(_DEF_LOGIN_RQ) {
		memset(tel, 0, _DEF_LEGNTH);
		memset(password, 0, _DEF_LEGNTH);
	}
	PackType type;
	char tel[_DEF_LEGNTH];
	char password[_DEF_LEGNTH];
}_STRU_LOGIN_RQ;

//��¼�ظ���Э��ͷ����¼�������¼�ɹ���������󡢵绰�����ڣ�
typedef struct _STRU_LOGIN_RS {
	_STRU_LOGIN_RS() :type(_DEF_LOGIN_RS), result(_login_password_error) {}
	PackType type;
	int userID;
	int result;
}_STRU_LOGIN_RS;

//��ȡ�����б�Э��ͷ���û�id���ǳơ�ǩ����ͷ��id��״̬
typedef struct _STRU_FRIEND_INFO {
	_STRU_FRIEND_INFO() :type(_DEF_FRIEND_INFO), status(_status_offline), iconID(0), userID(0) {
		memset(name, 0, _DEF_LEGNTH);
		memset(feeling, 0, _DEF_LEGNTH);
	}
	PackType type;
	int userID;
	char name[_DEF_LEGNTH];
	char feeling[_DEF_LEGNTH];
	int iconID;
	int status;
}_STRU_FRIEND_INFO;



//����ظ���Э��ͷ�����ͽ�����ɹ����û������ߣ�
typedef struct _STRU_CHAT_RS {
	_STRU_CHAT_RS() :type(_DEF_CHAT_RS), result(_send_fail),friendId(0) {}
	PackType type;
	int friendId;
	int result;
}_STRU_CHAT_RS;

//��Ӻ������󣨸����ǳ���Ӻ��ѣ���Э��ͷ�����ѵ��ǳơ��Լ���id���Լ����ǳ�
typedef struct _STRU_ADD_FRIEND_RQ {
	_STRU_ADD_FRIEND_RQ() :type(_DEF_ADD_FRIEND_RQ), userID(0) {
		memset(userName, 0, _DEF_LEGNTH);
		memset(friendName, 0, _DEF_LEGNTH);
	}
	PackType type;
	int userID;
	char userName[_DEF_LEGNTH];
	char friendName[_DEF_LEGNTH];
}_STRU_ADD_FRIEND_RQ;

//��Ӻ��ѻظ�
typedef struct _STRU_ADD_FRIEND_RS {
	_STRU_ADD_FRIEND_RS() : type(_DEF_ADD_FRIEND_RS),result(_add_friend_refuse), aId(0), bId(0) {
		memset(friendName, 0, _DEF_LEGNTH);
	}
	PackType type;
	int result;
	int aId;
	int bId;
	char friendName[_DEF_LEGNTH];
}_STRU_ADD_FRIEND_RS;

//��������Э��ͷ���Լ���id
typedef struct _STRU_OFFLINE_RQ {
	_STRU_OFFLINE_RQ() :type(_DEF_OFFLINE_RQ), userID(0) {}
	PackType type;
	int userID;
}_STRU_OFFLINE_RQ;



//��������Э��ͷ���Լ���id���������ݡ�����id
typedef struct _STRU_CHAT_RQ {
	_STRU_CHAT_RQ() :type(_DEF_CHAT_RQ), userID(0), friendID(0) {
		memset(content, 0, _DEF_CONTENT_LENGTH);
	}
	PackType type;
	int userID;
	int friendID;
	char content[_DEF_CONTENT_LENGTH];
}_STRU_CHAT_RQ;


//����Ⱥ������Э��ͷ���������Լ���id���Լ����ǳ�
typedef struct _STRU_CREATE_GROUP_RQ {
	_STRU_CREATE_GROUP_RQ() :type(_DEF_CREATE_GROUP_RQ), userID(0) {
		memset(userName, 0, _DEF_LEGNTH);
		memset(groupName, 0, _DEF_LEGNTH);
	}
	PackType type;
	int userID;
	char userName[_DEF_LEGNTH];
	char groupName[_DEF_LEGNTH];
}_STRU_CREATE_GROUP_RQ;

//��Ⱥ�ظ�
typedef struct _STRU_CREATE_GROUP_RS {
	_STRU_CREATE_GROUP_RS() : type(_DEF_CREATE_GROUP_RS),result(_create_group_fail), groupId(0) ,userID(0){
		memset(groupName, 0, _DEF_LEGNTH);
		memset(userName, 0, _DEF_LEGNTH);
	}
	PackType type;
	int result;
	int userID;
	char userName[_DEF_LEGNTH];
	int groupId;
	char groupName[_DEF_LEGNTH];
}_STRU_CREATE_GROUP_RS;

//��Ⱥ����Э��ͷ��Ⱥ�����Լ���id���Լ����ǳ�
typedef struct _STRU_ADD_GROUP_RQ {
	_STRU_ADD_GROUP_RQ() :type(_DEF_ADD_GROUP_RQ), userID(0) {
		memset(userName, 0, _DEF_LEGNTH);
		memset(groupName, 0, _DEF_LEGNTH);
	}
	PackType type;
	int userID;
	char userName[_DEF_LEGNTH];
	char groupName[_DEF_LEGNTH];
}_STRU_ADD_GROUP_RQ;

//��Ⱥ�ظ�
typedef struct _STRU_ADD_GROUP_RS {
	_STRU_ADD_GROUP_RS() :type(_DEF_ADD_GROUP_RS), result(_add_group_refuse), groupId(0) {
		memset(groupName, 0, _DEF_LEGNTH);
	}
	PackType type;
	int result;
	int groupId;
	char groupName[_DEF_LEGNTH];
}_STRU_ADD_GROUP_RS;

//��ȡȺ���б�Э��ͷ����Ա��Ϣ��Ⱥ����ͷ��id
typedef struct _STRU_GROUP_INFO {
	_STRU_GROUP_INFO() :type(_DEF_GROUP_INFO), groupID(0) , iconID(0){
		memset(groupName, 0, _DEF_LEGNTH);
		memset(member.name, 0, _DEF_LEGNTH);
		member.id = 0;
		member.iconID = 0;
		member.status = _status_offline;
	}
	PackType type;
	int groupID;
	char groupName[_DEF_LEGNTH];
	int iconID;
	struct {
		char name[_DEF_LEGNTH];
		int id;
		int iconID;
		int status;
	}member;
}_STRU_GROUP_INFO;

//��������Э��ͷ���Լ���id���������ݡ�������
typedef struct _STRU_GROUP_CHAT_RQ {
	_STRU_GROUP_CHAT_RQ() :type(_DEF_GROUP_CHAT_RQ), userID(0), groupID(0) {
		memset(content, 0, _DEF_CONTENT_LENGTH);
		memset(friendName, 0, _DEF_CONTENT_LENGTH);
	}
	PackType type;
	int userID;
	int groupID; 
	char friendName[_DEF_CONTENT_LENGTH];
	char content[_DEF_CONTENT_LENGTH];
}_STRU_GROUP_CHAT_RQ;

