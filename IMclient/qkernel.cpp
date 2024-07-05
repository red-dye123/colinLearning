#include "qkernel.h"
#include "mediator/TcpClientMediator.h"
#include <QDebug>
#include <QMessageBox>
#include "useritem.h"
#include <QTextCodec>
#include <QInputDialog>

QKernel::QKernel(QObject *parent) : QObject(parent)
{
    qDebug()<<__func__;
    //给登录&注册窗口new对象，并显示
    m_pImDialog=new IMDialog;
    m_pImDialog->show();

    //绑定关闭程序的信号和槽函数
    connect(m_pImDialog,&IMDialog::sig_closeApp,this,&QKernel::slot_closeApp);
    //绑定处理注册信息的信号和槽函数
    connect(m_pImDialog,&IMDialog::sig_commitRegister,this,&QKernel::slot_commitRegister);
    //绑定处理登录信息的信号和槽函数
    connect(m_pImDialog,&IMDialog::sig_commitLogin,this,&QKernel::slot_commitLogin);

    //给好友列表界面new对象
    m_pFriendList=new FriendList;

    //绑定添加好友的信号和槽函数
    connect(m_pFriendList,&FriendList::sig_addFriend,this,&QKernel::slot_addFriend);
    //绑定下显得信号和槽函数
    connect(m_pFriendList,&FriendList::sig_offline,this,&QKernel::slot_offline);
    //绑定创建群组的信号和槽函数
    connect(m_pFriendList,&FriendList::sig_createGroup,this,&QKernel::slot_createGroup);
    //绑定添加群组信号和槽函数
    connect(m_pFriendList,&FriendList::sig_addGroup,this,&QKernel::slot_addGroup);

    //打开网络
    m_pMediator=new TcpClientMediator;
    if(!m_pMediator->openNet()){
        QMessageBox::about(m_pImDialog,"info","net error");
        exit(0);
    }

    //绑定处理所有数据的信号和槽信号
    connect(m_pMediator,&TcpClientMediator::sig_transmitData,this,&QKernel::slot_transmitData);

    setProrocolMap();
}

QKernel::~QKernel()
{
    qDebug()<<__func__;
    if(m_pImDialog){
        m_pImDialog->hide();
        delete m_pImDialog;
        m_pImDialog=nullptr;
    }
    if(m_pMediator){
        m_pMediator->closeNet();
        delete m_pMediator;
        m_pMediator=nullptr;
    }
    if(m_pFriendList){
        m_pFriendList->hide();
        delete  m_pFriendList;
        m_pFriendList=nullptr;
    }
    for(auto ite=m_mapIdToChatdlg.begin();ite!=m_mapIdToChatdlg.end();){
        chatDialog* chat=ite.value();
        if(chat){
            chat->hide();
            delete chat;
            chat=nullptr;
        }
        //把无效节点从map中移除
        ite=m_mapIdToChatdlg.erase(ite);
    }
}

//utf-8转GB2312
void QKernel::utf8Togb2312(QString src, char *dest, int len/*目标空间大小*/)
{
    QTextCodec* dc = QTextCodec::codecForName("gb2312");
    QByteArray ba=dc->fromUnicode(src);
    strcpy_s(dest,len,ba.data());
}
//GB2312转utf-8
QString QKernel::gb2312Toutf8(char *src)
{
    QTextCodec* dc = QTextCodec::codecForName("gb2312");
    return dc->toUnicode(src);
}
//转发
void QKernel::slot_transmitData(char* data,int len,long from){
    qDebug()<<__func__;
    //取出协议头
    PackType type=*(PackType*)data;

    int index=type-_DEF_PROTOCOL_BASE-1;
    if(index>=0 && index<_DEF_PROTOCOL_COUNT){
        PFUN pf=m_mapProtocol[index];
        if(pf){
            (this->*pf)(data,len,from);
        }
        else{
            qDebug()<<"type2:"<<type;
        }
    }
    else{
        qDebug()<<"type1:"<<type;
    }
}

//处理注册信息
void QKernel::slot_commitRegister(QString name, QString tel, QString pass)
{
    qDebug()<<__func__;
    //1.打包
    _STRU_REGISTER_RQ rq;
    //strcpy()复制字符串内容，直到'\0'结束
    //memcpt()复制寄存器内容，按长度结束
    //strcpy_s(rq.name,name.toStdString().c_str());
    utf8Togb2312(name,rq.name,sizeof(rq.name));
    strcpy_s(rq.tel,tel.toStdString().c_str());
    strcpy_s(rq.password,pass.toStdString().c_str());

    //字符串转换:char*/char[],std::string,QString
    //char*/char[]是基础类型，std::string、QString是类，基础类型可以直接给类对象赋值
    //std::string调用c_str()转换成char*
    //QString调用toStdString()转换成std::string

    //2.发给服务器
    m_pMediator->sendData((char*)&rq,sizeof(rq),0);

}

//处理登录信息
void QKernel::slot_commitLogin(QString tel, QString pass)
{
    qDebug()<<__func__;
    //1.打包
    _STRU_LOGIN_RQ rq;
    strcpy_s(rq.tel,tel.toStdString().c_str());
    strcpy_s(rq.password,pass.toStdString().c_str());
    //2.发给服务器
    m_pMediator->sendData((char*)&rq,sizeof(rq),0);
}

//处理显示聊天窗口
void QKernel::slot_showChatDialog(int id)
{
    qDebug()<<__func__<<endl;
    //判断map中是否有这个窗口
    if(m_mapIdToChatdlg.count(id)>0){
        //找到窗口并显示
        chatDialog* chat=m_mapIdToChatdlg[id];
        chat->showNormal();
    }
}

//绑定协议头数组
void QKernel::setProrocolMap(){
    //初始化数组
    memset(m_mapProtocol,0,sizeof(m_mapProtocol));
    //给数组赋值
    m_mapProtocol[_DEF_REGISTER_RS-_DEF_PROTOCOL_BASE-1]=&QKernel::dealRegisterRs;
    m_mapProtocol[_DEF_LOGIN_RS-_DEF_PROTOCOL_BASE-1]=&QKernel::dealLoginRs;
    m_mapProtocol[_DEF_FRIEND_INFO-_DEF_PROTOCOL_BASE-1]=&QKernel::dealFriendInfoRs;
    m_mapProtocol[_DEF_CHAT_RQ-_DEF_PROTOCOL_BASE-1]=&QKernel::dealChatRq;
    m_mapProtocol[_DEF_CHAT_RS-_DEF_PROTOCOL_BASE-1]=&QKernel::dealChatRs;
    m_mapProtocol[_DEF_OFFLINE_RQ-_DEF_PROTOCOL_BASE-1]=&QKernel::dealOfflineRq;
    m_mapProtocol[_DEF_ADD_FRIEND_RQ-_DEF_PROTOCOL_BASE-1]=&QKernel::dealAddFriendRq;
    m_mapProtocol[_DEF_ADD_FRIEND_RS-_DEF_PROTOCOL_BASE-1]=&QKernel::dealAddFriendRs;

    m_mapProtocol[_DEF_CREATE_GROUP_RS-_DEF_PROTOCOL_BASE-1]=&QKernel::dealCreateGroupRs;
    m_mapProtocol[_DEF_ADD_GROUP_RS-_DEF_PROTOCOL_BASE-1]=&QKernel::dealAddGroupRs;
    m_mapProtocol[_DEF_GROUP_INFO-_DEF_PROTOCOL_BASE-1]=&QKernel::dealGroupInfoRs;
    m_mapProtocol[_DEF_GROUP_CHAT_RQ-_DEF_PROTOCOL_BASE-1]=&QKernel::dealGroupChatRq;

}

//处理注册回复
void QKernel::dealRegisterRs(char* data, int len, long from){
    qDebug()<<__func__;
    //1.拆包
    _STRU_REGISTER_RS* rs=(_STRU_REGISTER_RS*)data;

    //2.根据注册结果提示用户
    switch (rs->result) {
    case _register_success:
        QMessageBox::about(m_pImDialog,"提示","注册成功");
        break;
    case _register_tel_exists:
        QMessageBox::about(m_pImDialog,"提示","注册失败，电话号码已被注册");
        break;
    case _register_name_exists:
        QMessageBox::about(m_pImDialog,"提示","注册失败，昵称已被占用");
        break;
    default:
        break;
    }

}

//处理登录回复
void QKernel::dealLoginRs(char* data, int len, long from){
    qDebug()<<__func__;
    //1.拆包
    _STRU_LOGIN_RS* rs=(_STRU_LOGIN_RS*)data;

    //2.根据注册结果提示用户
    switch (rs->result) {
    case _login_success:{
        //进行页面跳转，隐藏登录界面，显示好友界面
        m_pImDialog->hide();
        m_pFriendList->show();
        //保存登录用户的id
        m_id=rs->userID;
    }
        break;
    case _login_tel_not_exists:
        QMessageBox::about(m_pImDialog,"提示","登录失败，电话号码未注册");
        break;
    case _login_password_error:
        QMessageBox::about(m_pImDialog,"提示","登录失败，密码错误");
        break;
    default:
        break;
    }
}

//处理好友信息
void QKernel::dealFriendInfoRs(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_FRIEND_INFO* rq=(_STRU_FRIEND_INFO*)data;
    QString name=gb2312Toutf8(rq->name);
    QString feeling=gb2312Toutf8(rq->feeling);

    //2.判断是不是自己的信息
    if(rq->userID==m_id){
        //保存自己的昵称
        m_mname=name;
        //把自己的信息设置到界面上
        m_pFriendList->setUserInfo(name,feeling,rq->iconID);
    }

    //好友的信息，判断列表中是否已经存在这个好友了
    if(m_mapIdToUseritem.count(rq->userID)>0){
        //有好友，更新好友信息（好友状态变化——上线和下线）
        useritem* item=m_mapIdToUseritem[rq->userID];
        item->setFriendInfo(rq->userID,rq->status,rq->iconID,name,feeling);
    }
    else{
        //没有好友，new一个好友
        useritem* item=new useritem;
        //设置好友信息
        item->setFriendInfo(rq->userID,rq->status,rq->iconID,name,feeling);
        //把好友添加到列表上
        m_pFriendList->addFriend(item);
        //保存usetitem
        m_mapIdToUseritem[rq->userID]=item;

        //绑定显示聊天窗口的信号和槽函数
        connect(item,&useritem::sig_showChatDialog,this,&QKernel::slot_showChatDialog);

        //new一个跟这个好友的聊天窗口
        chatDialog* chat=new chatDialog;
        //设置窗口信息
        chat->setChatInfo(rq->userID,name);
        //保存聊天窗口
        m_mapIdToChatdlg[rq->userID]=chat;

        //绑定发送聊天内容的信号和槽函数
        connect(chat,&chatDialog::sig_sendChatMessage,this,&QKernel::slot_sendChatMessage);
    }
}

//处理聊天请求
void QKernel::dealChatRq(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_CHAT_RQ* rq=(_STRU_CHAT_RQ*)data;

    //当前是B客户端，聊天内容是A客户端发来的
    //2.找到与A客户端的聊天窗口
    if(m_mapIdToChatdlg.count(rq->userID)>0){
        //设置聊天内容到窗口上，并显示聊天窗口
        chatDialog* chat=m_mapIdToChatdlg[rq->userID];
        QByteArray baEncoded(rq->content,sizeof(rq->content));
        QByteArray ba=QByteArray::fromBase64(baEncoded);
        QString decoded=QString::fromUtf8(ba);
        chat->setChatCotent(decoded);
        chat->showNormal();
    }
}

//处理聊天回复
void QKernel::dealChatRs(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_CHAT_RS* rs=(_STRU_CHAT_RS*)data;

    //我是A客户端，B客户端不在线
    //2.找到与B客户端的聊天窗口
    if(m_mapIdToChatdlg.count(rs->friendId)>0){
        //显示B客户端不在线，并显示聊天窗口
        chatDialog* chat=m_mapIdToChatdlg[rs->friendId];
        chat->setFriendOffline();
        chat->showNormal();
    }
}

//处理添加好友请求
void QKernel::dealAddFriendRq(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_FRIEND_RQ* rq=(_STRU_ADD_FRIEND_RQ*)data;

    //2.提示用户有人添加好友，询问是否同意
    _STRU_ADD_FRIEND_RS rs;
    if(QMessageBox::question(m_pFriendList,"添加好友",QString("【%1】申请添加你为好友，是否同意").arg(rq->userName))==QMessageBox::Yes){
        rs.result=_add_friend_success;
    }
    else{
        rs.result=_add_friend_refuse;
    }
    strcpy_s(rs.friendName,m_mname.toStdString().c_str());
    rs.aId=rq->userID;
    rs.bId=m_id;

    //把结果发给服务器端
    m_pMediator->sendData((char*)&rs,sizeof(rs),0);
}

//处理添加好友回复
void QKernel::dealAddFriendRs(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_FRIEND_RS* rs=(_STRU_ADD_FRIEND_RS*)data;
    QString friendName=gb2312Toutf8(rs->friendName);

    //2.根据结果提示用户
    switch (rs->result) {
    case _add_friend_success:
        QMessageBox::about(m_pFriendList,"标题",QString("添加【%1】为好友成功").arg(rs->friendName));
        break;
    case _add_friend_not_exists:
        QMessageBox::about(m_pFriendList,"标题",QString("添加【%1】为好友失败，好友不存在").arg(friendName));
        break;
    case _add_friend_offline:
        QMessageBox::about(m_pFriendList,"标题",QString("添加【%1】为好友失败，好友不在线").arg(friendName));
        break;
    case _add_friend_refuse:
        QMessageBox::about(m_pFriendList,"标题",QString("【%1】拒绝添加你为好友").arg(rs->friendName));
        break;
    default:
        break;

    }
}

//处理下线请求
void QKernel::dealOfflineRq(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_OFFLINE_RQ* rq=(_STRU_OFFLINE_RQ*)data;

    //2.找到好友的useritem
    if(m_mapIdToUseritem.count(rq->userID)>0){
        useritem* item=m_mapIdToUseritem[rq->userID];
        //设置好友的状态为下线
        item->setFriendOffline();
    }
}

//处理建群回复
void QKernel::dealCreateGroupRs(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_CREATE_GROUP_RS* rs=(_STRU_CREATE_GROUP_RS*)data;
    QString groupName=gb2312Toutf8(rs->groupName);

    //2.根据结果提示用户
    switch (rs->result) {
    case _create_group_fail:
        QMessageBox::about(m_pFriendList,"标题",QString("创建【%1】群聊失败").arg(rs->groupName));
        break;
    case _create_group_success:
        QMessageBox::about(m_pFriendList,"标题",QString("创建【%1】群聊成功").arg(rs->groupName));
        break;
    default:
        break;

    }
}

//处理加群回复
void QKernel::dealAddGroupRs(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_GROUP_RS* rs=(_STRU_ADD_GROUP_RS*)data;
    QString groupName=gb2312Toutf8(rs->groupName);

    //2.根据结果提示用户
    switch (rs->result) {
    case _add_group_refuse:
        QMessageBox::about(m_pFriendList,"标题",QString("加入【%1】群聊被拒绝").arg(rs->groupName));
        break;
    case _add_group_success:
        QMessageBox::about(m_pFriendList,"标题",QString("成功加入【%1】群聊").arg(rs->groupName));
        break;
    case _add_group_not_exists:
        QMessageBox::about(m_pFriendList,"标题",QString("【%1】群聊不存在").arg(rs->groupName));
        break;
    default:
        break;

    }
}

//处理群信息
void QKernel::dealGroupInfoRs(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_GROUP_INFO* rq=(_STRU_GROUP_INFO*)data;
    QString name=gb2312Toutf8(rq->groupName);

    //判断列表中是否已经存在这个群了
    if(m_mapIdToGroupitem.count(rq->groupID)>0){
        //有群，更新群信息
        groupitem* item=m_mapIdToGroupitem[rq->groupID];
        item->setGroupInfo(rq->groupID,rq->iconID,name);
    }
    else{
        //没有群，new一个群
        groupitem* item=new groupitem;
        //设置群信息
        item->setGroupInfo(rq->groupID,rq->iconID,name);
        //把群添加到列表上
        m_pFriendList->addGroup(item);
        //保存groupitem
        m_mapIdToGroupitem[rq->groupID]=item;

        //绑定显示聊天窗口的信号和槽函数
        connect(item,&groupitem::sig_showGroupChatDialog,this,&QKernel::slot_showGroupChatDialog);
        //new一个跟这个好友的聊天窗口
        groupchatdialog* chat=new groupchatdialog;
        //设置窗口信息
        chat->setChatInfo(rq->groupID,name);
        //保存聊天窗口
        m_mapIdToGroupChatdlg[rq->groupID]=chat;

        //绑定发送聊天内容的信号和槽函数
        connect(chat,&groupchatdialog::sig_sendGroupChatMessage,this,&QKernel::slot_sendGroupChatMessage);
    }
}

//处理群聊天信息
void QKernel::dealGroupChatRq(char *data, int len, long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_GROUP_CHAT_RQ* rq=(_STRU_GROUP_CHAT_RQ*)data;

    //当前是B客户端，聊天内容是A客户端发来的
    //2.找到与A客户端的聊天窗口
    if(m_mapIdToGroupChatdlg.count(rq->groupID)>0){
        //设置聊天内容到窗口上，并显示聊天窗口
        groupchatdialog* chat=m_mapIdToGroupChatdlg[rq->groupID];
        QByteArray baEncoded(rq->content,sizeof(rq->content));
        QByteArray ba=QByteArray::fromBase64(baEncoded);
        QString decoded=QString::fromUtf8(ba);
        chat->setChatCotent(rq->friendName,decoded);
        chat->showNormal();
    }
}

//发送聊天信息
void QKernel::slot_sendChatMessage(QString content, int id)
{
    qDebug()<<__func__;
    //1.打包
    _STRU_CHAT_RQ rq;
    rq.userID=m_id;
    rq.friendID=id;
    QByteArray ba(content.toUtf8());
    QByteArray baEncoded=ba.toBase64();
    //strcpy_s(rq.content,content.toStdString().c_str());
    memcpy(rq.content,baEncoded.data(),baEncoded.size());
    //2.发给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),0);
}

//处理关闭程序的信号
void QKernel::slot_closeApp()
{
    qDebug()<<__func__;
    //1.回收资源
    if(m_pImDialog){
        m_pImDialog->hide();
        delete m_pImDialog;
        m_pImDialog=nullptr;
    }
    if(m_pMediator){
        m_pMediator->closeNet();
        delete m_pMediator;
        m_pMediator=nullptr;
    }
    if(m_pFriendList){
        m_pFriendList->hide();
        delete  m_pFriendList;
        m_pFriendList=nullptr;
    }
    for(auto ite=m_mapIdToChatdlg.begin();ite!=m_mapIdToChatdlg.end();){
        chatDialog* chat=ite.value();
        if(chat){
            chat->hide();
            delete chat;
            chat=nullptr;
        }
        //把无效节点从map中移除
        ite=m_mapIdToChatdlg.erase(ite);
    }

    //2.退出进程
    exit(0);
}

//下线
void QKernel::slot_offline()
{
    qDebug()<<__func__;
    //1.给服务器端发送下线通知
    _STRU_OFFLINE_RQ rq;
    rq.userID=m_id;
    m_pMediator->sendData((char*)&rq,sizeof(rq),0);

    //2.回收资源，退出进程
    slot_closeApp();
}

//添加好友
void QKernel::slot_addFriend()
{
    //1.弹出输入窗口，让用户输入昵称
    QString friendName=QInputDialog::getText(m_pFriendList,"添加好友","请输入好友的昵称：");
    QString friendNameTmp=friendName;

    //2.判断用户输入是否合法
    if(friendName.isEmpty() || friendNameTmp.remove(" ").isEmpty()){
        QMessageBox::about(m_pFriendList,"提示","用户昵称不能为空");
        return;
    }

//    //3.判断输入的昵称是否是自己的昵称
//    if(friendName==m_mname){
//        QMessageBox::about(m_pFriendList,"提示","不能添加自己为好友");
//        return;
//    }

    //4.判断昵称是否已经是好友
    for(useritem* item:m_mapIdToUseritem){
        if(item->name() == friendName){
            QMessageBox::about(m_pFriendList,"提示","已经是好友了");
            return;
        }
    }

    //5.给服务器发送添加好友请求
    _STRU_ADD_FRIEND_RQ rq;
    rq.userID=m_id;
    utf8Togb2312(friendName,rq.friendName,sizeof(rq.friendName));
    //strcpy_s(rq.friendName,friendName.toStdString().c_str());
    strcpy_s(rq.userName,m_mname.toStdString().c_str());

    m_pMediator->sendData((char*)&rq,sizeof(rq),0);
}

//建群
void QKernel::slot_createGroup()
{
    //1.弹出输入窗口，让用户输入
    QString groupName=QInputDialog::getText(m_pFriendList,"创建群聊","请输入群名：");
    QString groupNameTmp=groupName;
    //2.判断用户输入是否合法
    if(groupName.isEmpty() || groupNameTmp.remove(" ").isEmpty()){
        QMessageBox::about(m_pFriendList,"提示","群名不能为空");
        return;
    }
    //4.判断已经入群
    for(groupitem* item:m_mapIdToGroupitem){
        if(item->name() == groupName){
            QMessageBox::about(m_pFriendList,"提示","已经在群里了");
            return;
        }
    }
    //5.给服务器发送建群请求
    _STRU_CREATE_GROUP_RQ rq;
    rq.userID=m_id;
    utf8Togb2312(groupName,rq.groupName,sizeof(rq.groupName));
    utf8Togb2312(m_mname,rq.userName,sizeof(rq.userName));

    m_pMediator->sendData((char*)&rq,sizeof(rq),0);
}

//加群
void QKernel::slot_addGroup()
{
    //1.弹出输入窗口，让用户输入
    QString groupName=QInputDialog::getText(m_pFriendList,"加入群聊","请输入群名：");
    QString groupNameTmp=groupName;
    //2.判断用户输入是否合法
    if(groupName.isEmpty() || groupNameTmp.remove(" ").isEmpty()){
        QMessageBox::about(m_pFriendList,"提示","群名不能为空");
        return;
    }
    //4.判断已经入群
    for(groupitem* item:m_mapIdToGroupitem){
        if(item->name() == groupName){
            QMessageBox::about(m_pFriendList,"提示","已经在群里了");
            return;
        }
    }
    //5.给服务器发送建群请求
    _STRU_ADD_GROUP_RQ rq;
    rq.userID=m_id;
    utf8Togb2312(groupName,rq.groupName,sizeof(rq.groupName));
    utf8Togb2312(m_mname,rq.userName,sizeof(rq.userName));

    m_pMediator->sendData((char*)&rq,sizeof(rq),0);
}

//显示群消息
void QKernel::slot_showGroupChatDialog(int id)
{
    qDebug()<<__func__<<endl;
    //判断map中是否有这个窗口
    if(m_mapIdToGroupChatdlg.count(id)>0){
        //找到窗口并显示
        groupchatdialog* chat=m_mapIdToGroupChatdlg[id];
        chat->showNormal();
    }
}

//发送群消息
void QKernel::slot_sendGroupChatMessage(QString content, int id)
{
    qDebug()<<__func__;
    //1.打包
    _STRU_GROUP_CHAT_RQ rq;
    rq.userID=m_id;
    rq.groupID=id;
    strcpy_s(rq.friendName,m_mname.toStdString().c_str());
    //strcpy_s(rq.content,content.toStdString().c_str());
    QByteArray ba(content.toUtf8());
    QByteArray baEncoded=ba.toBase64();
    memcpy(rq.content,baEncoded.data(),baEncoded.size());
    //2.发给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),0);
}
