#pragma once
#include "mediator/mediator.h"
#include "MySQL/CMySql.h"
#include "net/def.h"
#include <map>

//��������ָ��
//������������
class CKernel;
typedef void(CKernel::* PFUN)(char* data, int len, long from);

class CKernel
{
public:
	//��ָ̬��
	static CKernel* pKernel;

	CKernel();
	~CKernel();
	//�򿪷�����
	bool startServer();
	//�رշ�����
	void closeServer();

	//�������н��յ�������
	void dealData(char* data, int len, long from);
	//��Э��ͷ����
	void setProrocolMap();
	//����ע������
	void dealRegisterRq(char* data, int len, long from);
	//�����¼����
	void dealLoginRq(char* data, int len, long from);
	//������������
	void dealChatRq(char* data, int len, long from);

	//��ȡ�û��ĺ�����Ϣ�������Լ���
	void getUserInfo(int userID);
	//�����û�id��ѯ�û���Ϣ
	void getInfoById(int id, _STRU_FRIEND_INFO* info);

	//������������
	void dealOfflineRq(char* data, int len, long from);
	//������Ӻ�������
	void dealAddFriendRq(char* data, int len, long from);
	//����Ӻ��ѻظ�
	void dealAddFriendRs(char* data, int len, long from);

	//������Ӻ�������
	void dealCreateGroupRq(char* data, int len, long from);
	//����Ӻ��ѻظ�
	void dealAddGroupRq(char* data, int len, long from);
	//��ȡȺ����Ϣ
	void getGroupInfo(int userID);
	//������id��ѯȺ����Ϣ
	void getGroupInfoById(int id, _STRU_GROUP_INFO* info);
	//����Ⱥ������
	void dealGroupChatRq(char* data, int len, long from);

private:
	mediator* m_pMediator;
	
	PFUN m_mapProrocol[_DEF_PROEOCOL_COUNT];

	//���ݿ����
	CMySql m_mysql;

	std::map<int,SOCKET> m_mapIdToSocket;
};

