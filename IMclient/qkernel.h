#ifndef QKERNEL_H
#define QKERNEL_H

#include <QObject>
#include "imdialog.h"
#include "mediator/mediator.h"
#include "net/def.h"
#include "friendlist.h"
#include <QMap>
#include "chatdialog.h"
#include "groupchatdialog.h"

//声明函数指针
class QKernel;
typedef void(QKernel::* PFUN)(char* data, int len, long from);

class QKernel : public QObject
{
    Q_OBJECT
public:
    explicit QKernel(QObject *parent = nullptr);
    ~QKernel();

    //QT使用utf-8，vs使用gb3212,在客户端做字符转换
    //utf-8转GB2312
    void utf8Togb2312(QString src,char* dest ,int len);
    //GB2312转utf-8
    QString gb2312Toutf8(char* src);

    //绑定协议头数组
    void setProrocolMap();
    //处理注册回复
    void dealRegisterRs(char* data, int len, long from);
    //处理登录回复
    void dealLoginRs(char* data, int len, long from);
    //处理好友信息
    void dealFriendInfoRs(char* data,int len,long from);
    //处理聊天请求
    void dealChatRq(char* data,int len,long from);
    //处理聊天回复
    void dealChatRs(char* data,int len,long from);
    //处理添加好友请求
    void dealAddFriendRq(char* data,int len,long from);
    //处理添加好友回复
    void dealAddFriendRs(char* data,int len,long from);

    //处理下线请求
    void dealOfflineRq(char* data,int len,long from);

    //处理建群回复
    void dealCreateGroupRs(char* data,int len,long from);
    //处理加群回复
    void dealAddGroupRs(char* data,int len,long from);
    //处理群聊信息
    void dealGroupInfoRs(char* data,int len,long from);
    //处理群聊请求
    void dealGroupChatRq(char* data,int len,long from);

signals:

public slots:
    //把接收到的数据传给kernel
    void slot_transmitData(char* data,int len,long from);
    //处理注册信息
    void slot_commitRegister(QString name,QString tel,QString pass);
    //处理登录信息
    void slot_commitLogin(QString tel,QString pass);
    //处理显示聊天窗口
    void slot_showChatDialog(int id);
    //处理聊天内容
    void slot_sendChatMessage(QString content,int id);

    //处理关闭程序的信号
    void slot_closeApp();
    //处理下线信号
    void slot_offline();
    //处理添加好友的信号
    void slot_addFriend();

    //处理创建群组的信号
    void slot_createGroup();
    //处理添加群组的信号
    void slot_addGroup();
    //处理群聊窗口
    void slot_showGroupChatDialog(int id);
    //处理群聊内容
    void slot_sendGroupChatMessage(QString content,int id);

private:
    int m_id;
    QString m_mname;
    IMDialog* m_pImDialog;
    mediator* m_pMediator;
    FriendList* m_pFriendList;
    //函数指针数组
    PFUN m_mapProtocol[_DEF_PROTOCOL_COUNT];
    //保存好友
    QMap<int,useritem*> m_mapIdToUseritem;
    //保存与好友的聊天窗口
    QMap<int,chatDialog*> m_mapIdToChatdlg;

    //保存群聊
    QMap<int,groupitem*> m_mapIdToGroupitem;
    //保存群聊的聊天窗口
    QMap<int,groupchatdialog*> m_mapIdToGroupChatdlg;

};

#endif // QKERNEL_H

