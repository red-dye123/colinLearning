#include "friendlist.h"
#include "ui_friendlist.h"
#include <QCloseEvent>
#include <QMessageBox>

FriendList::FriendList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FriendList)
{
    ui->setupUi(this);

    //new一个垂直布局的层
    m_layout_friend=new QVBoxLayout;
    m_layout_group=new QVBoxLayout;
    //设置小控件间的距离
    m_layout_friend->setSpacing(3);
    m_layout_group->setSpacing(3);
    //设置小控件和大控件间的距离
    m_layout_friend->setContentsMargins(0,0,0,0);
    m_layout_group->setContentsMargins(0,0,0,0);
    //把层添加到大控件上
    ui->wdg_list_friend->setLayout(m_layout_friend);
    ui->wdg_list_group->setLayout(m_layout_group);

    //new一个菜单的对象
    m_pMenu=new QMenu(this);
    //添加菜单项
    m_pMenu->addAction("添加好友");
    m_pMenu->addAction("创建群聊");
    m_pMenu->addAction("添加群聊");
    m_pMenu->addAction("系统设置");
    //绑定菜单项点击的信号和槽函数
    connect(m_pMenu,&QMenu::triggered,this,&FriendList::slot_menuClicked);
}

FriendList::~FriendList()
{
    delete ui;
}

void FriendList::addFriend(useritem *item)
{
    m_layout_friend->addWidget(item);
}

void FriendList::setUserInfo(QString name, QString feeling, int iconID)
{
    ui->lb_name->setText(name);
    ui->le_feeling->setText(feeling);
    ui->pb_icon->setIcon(QIcon(QString(":/tx/%1.png").arg(iconID)));
}

//重写关闭事件
void FriendList::closeEvent(QCloseEvent *event)
{
    //忽略当前时间（否则会继续走父类的关闭函数，还能是关闭窗口）
    event->ignore();
    //询问用户是否确认关闭程序
    if(QMessageBox::Yes == QMessageBox::question(this,"提示","是否确认退出程序?")){
        Q_EMIT sig_offline();
    }
}

void FriendList::addGroup(groupitem *item)
{
    m_layout_group->addWidget(item);
}

void FriendList::on_pb_menu_clicked()
{
    //在鼠标点击位置上向上弹出菜单栏
    //获取鼠标点击的坐标
    QPoint point=QCursor::pos();
    //获取菜单栏的绝对大小
    QSize size=m_pMenu->sizeHint();
    //显示菜单
    m_pMenu->exec(QPoint(point.x(),point.y()-size.height()));
}

//处理菜单项点击的信号
void FriendList::slot_menuClicked(QAction *action)
{
    if("添加好友"==action->text()){
        Q_EMIT sig_addFriend();
    }
    else if("添加群聊"==action->text()){
        Q_EMIT sig_addGroup();
    }
    else if("创建群聊"==action->text()){
        Q_EMIT sig_createGroup();
    }
    else{

    }
}


void FriendList::on_pb_tool1_clicked()
{
    //测试按钮
}

