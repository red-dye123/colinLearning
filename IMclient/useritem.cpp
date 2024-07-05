#include "useritem.h"
#include "ui_useritem.h"
#include "net/def.h"
#include <QBitmap>

useritem::useritem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::useritem)
{
    ui->setupUi(this);
}

useritem::~useritem()
{
    delete ui;
}

//保存并设置好友信息
void useritem::setFriendInfo(int friendID, int status, int iconID, QString name, QString feeling)
{
    //保存到成员变量中
    m_friendID=friendID;
    m_status=status;
    m_iconID=iconID;
    m_name=name;
    m_feeling=feeling;

    //设置到控件上
    ui->lb_name->setText(m_name);
    ui->lb_feeling->setText(m_feeling);
    //拼接图片路径
    QString path=QString(":/tx/%1.png").arg(m_iconID);
    if(_status_online == m_status){
        //在线，亮显头像
        ui->pb_icon->setIcon(QIcon(path));
    }
    else{
        //不在线，暗显头像
        QBitmap bt;
        bt.load(path);
        ui->pb_icon->setIcon(bt);
    }
}

//设置好友的状态为下线
void useritem::setFriendOffline()
{
    //改变状态
    m_status=_status_offline;

    //设置头像暗显
    QString path=QString(":/tx/%1.png").arg(m_iconID);
    QBitmap bt;
    bt.load(path);
    ui->pb_icon->setIcon(bt);
}

void useritem::on_pb_icon_clicked()
{
    //通知kernel显示聊天窗口
    Q_EMIT sig_showChatDialog(m_friendID);
}

const QString &useritem::name() const
{
    return m_name;
}

