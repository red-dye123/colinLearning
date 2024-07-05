#pragma once
#include <string.h>
#include <QMap>

//加括号防止计算时出现计算顺序问题
#define _DEF_TCP_PORT (67890)
#define _DEF_TCP_SERVER_IP ("127.0.0.1")

//定义协议头
#define _DEF_PROTOCOL_BASE	(1000)
#define _DEF_LEGNTH (100)
#define _DEF_CONTENT_LENGTH (4096)
#define _DEF_PROTOCOL_COUNT (16)

//注册请求
#define _DEF_REGISTER_RQ	(_DEF_PROTOCOL_BASE + 1)
//注册回复
#define _DEF_REGISTER_RS	(_DEF_PROTOCOL_BASE + 2)
//登录请求
#define _DEF_LOGIN_RQ		(_DEF_PROTOCOL_BASE + 3)
//登录回复
#define _DEF_LOGIN_RS		(_DEF_PROTOCOL_BASE + 4)
//获取好友列表
#define _DEF_FRIEND_INFO	(_DEF_PROTOCOL_BASE + 5)
//聊天请求
#define _DEF_CHAT_RQ		(_DEF_PROTOCOL_BASE + 6)
//聊天回复
#define _DEF_CHAT_RS		(_DEF_PROTOCOL_BASE + 7)
//添加好友请求
#define _DEF_ADD_FRIEND_RQ	(_DEF_PROTOCOL_BASE + 8)
//添加好友回复
#define _DEF_ADD_FRIEND_RS	(_DEF_PROTOCOL_BASE + 9)
//下线请求
#define _DEF_OFFLINE_RQ		(_DEF_PROTOCOL_BASE + 10)

//创建群聊请求
#define _DEF_CREATE_GROUP_RQ	(_DEF_PROTOCOL_BASE + 11)
//建群回复
#define _DEF_CREATE_GROUP_RS	(_DEF_PROTOCOL_BASE + 12)
//加入群聊请求
#define _DEF_ADD_GROUP_RQ	(_DEF_PROTOCOL_BASE + 13)
//加群回复
#define _DEF_ADD_GROUP_RS	(_DEF_PROTOCOL_BASE + 14)
//获取群组列表
#define _DEF_GROUP_INFO		(_DEF_PROTOCOL_BASE + 15)
//群聊请求
#define _DEF_GROUP_CHAT_RQ	(_DEF_PROTOCOL_BASE + 16)

//声明结果
//注册结果
#define _register_success		(0)
#define _register_tel_exists	(1)
#define _register_name_exists	(2)
//登录结果
#define _login_success			(0)
#define _login_tel_not_exists	(1)
#define _login_password_error	(2)
//用户状态
#define _status_online			(0)
#define _status_offline			(1)
//发送结果
#define _send_success			(0)
#define _send_fail				(1)
//添加好友结果
#define _add_friend_success		(0)
#define _add_friend_not_exists	(1)
#define _add_friend_offline		(2)
#define _add_friend_refuse		(3)

//建群结果
#define _create_group_success	(0)
#define _create_group_fail		(1)
//加群结果
#define _add_group_success		(0)
#define _add_group_not_exists	(1)
#define _add_group_refuse		(2)

//群发结果
#define _group_send_success			(0)
#define _group_send_fail			(1)

//声明协议头类型
typedef int PackType;

//请求结构体
//注册请求：协议头、电话号、昵称、密码
typedef struct _STRU_REGISTER_RQ {
    _STRU_REGISTER_RQ() :type(_DEF_REGISTER_RQ) {
        memset(tel, 0, _DEF_LEGNTH);
        memset(name, 0, _DEF_LEGNTH);
        memset(password, 0, _DEF_LEGNTH);
    }
    PackType type;
    char tel[_DEF_LEGNTH];
    char name[_DEF_LEGNTH];
    char password[_DEF_LEGNTH];
}_STRU_REGISTER_RQ;

//注册回复：协议头、注册结果（注册成功、电话已被注册、昵称已被注册）
typedef struct _STRU_REGISTER_RS {
    _STRU_REGISTER_RS() :type(_DEF_REGISTER_RS), result(_register_name_exists) {}
    PackType type;
    int result;
}_STRU_REGISTER_RS;

//登录请求：协议头、电话号码、密码
typedef struct _STRU_LOGIN_RQ {
    _STRU_LOGIN_RQ() :type(_DEF_LOGIN_RQ) {
        memset(tel, 0, _DEF_LEGNTH);
        memset(password, 0, _DEF_LEGNTH);
    }
    PackType type;
    char tel[_DEF_LEGNTH];
    char password[_DEF_LEGNTH];
}_STRU_LOGIN_RQ;

//登录回复：协议头、登录结果（登录成功、密码错误、电话不存在）
typedef struct _STRU_LOGIN_RS {
    _STRU_LOGIN_RS() :type(_DEF_LOGIN_RS), result(_login_password_error),userID(0) {}
    PackType type;
    int userID;
    int result;
}_STRU_LOGIN_RS;

//获取好友列表：协议头、用户id、昵称、签名、头像id、状态
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

//聊天请求：协议头、自己的id、聊天内容、好友id
typedef struct _STRU_CHAT_RQ {
    _STRU_CHAT_RQ() :type(_DEF_CHAT_RQ), userID(0), friendID(0) {
        memset(content, 0, _DEF_CONTENT_LENGTH);
    }
    PackType type;
    int userID;
    int friendID;
    char content[_DEF_CONTENT_LENGTH];
}_STRU_CHAT_RQ;

//聊天回复：协议头、发送结果（成功、用户不在线）
typedef struct _STRU_CHAT_RS {
    _STRU_CHAT_RS() :type(_DEF_CHAT_RS), result(_send_fail),friendId(0) {}
    PackType type;
    int friendId;
    int result;
}_STRU_CHAT_RS;

//添加好友请求（根据昵称添加好友）：协议头、好友的昵称、自己的id、自己的昵称
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

//添加好友回复
typedef struct _STRU_ADD_FRIEND_RS {
    _STRU_ADD_FRIEND_RS() :type(_DEF_ADD_FRIEND_RS), result(_add_friend_refuse),aId(0),bId(0) {
        memset(friendName, 0, _DEF_LEGNTH);
    }
    PackType type;
    int result;
    int aId;
    int bId;
    char friendName[_DEF_LEGNTH];
}_STRU_ADD_FRIEND_RS;

//下线请求：协议头、自己的id
typedef struct _STRU_OFFLINE_RQ {
    _STRU_OFFLINE_RQ() :type(_DEF_OFFLINE_RQ), userID(0) {}
    PackType type;
    int userID;
}_STRU_OFFLINE_RQ;


//创建群聊请求：协议头、组名、自己的id、自己的昵称
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

//建群回复
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

//加群请求：协议头、群名、自己的id、自己的昵称
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

//加群回复
typedef struct _STRU_ADD_GROUP_RS {
    _STRU_ADD_GROUP_RS() : type(_DEF_ADD_GROUP_RS),result(_add_group_refuse), groupId(0) {
        memset(groupName, 0, _DEF_LEGNTH);
    }
    PackType type;
    int result;
    int groupId;
    char groupName[_DEF_LEGNTH];
}_STRU_ADD_GROUP_RS;

//获取群组列表：协议头、成员信息、群名、头像id
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

//聊天请求：协议头、自己的id、聊天内容、好友名
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
