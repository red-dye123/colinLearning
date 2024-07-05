#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include <QDialog>
#include "useritem.h"
#include "groupitem.h"
#include <QVBoxLayout>
#include <QMenu>

namespace Ui {
class FriendList;
}

class FriendList : public QDialog
{
    Q_OBJECT
signals:
    //通知kernel下线
    void sig_offline();
    //通知kernel添加好友
    void sig_addFriend();
    //通知kernel创建群组
    void sig_createGroup();
    //通知kernel添加群组
    void sig_addGroup();

public:
    explicit FriendList(QWidget *parent = nullptr);
    ~FriendList();

    //把好友添加到列表上
    void addFriend(useritem* item);
    //设置自己的信息
    void setUserInfo(QString name,QString feeling,int iconID);

    //重写关闭事件
    void closeEvent(QCloseEvent* event);

    //把群聊添加到表上
    void addGroup(groupitem* item);


private slots:
    void on_pb_menu_clicked();
    //处理菜单项点击的信号
    void slot_menuClicked(QAction* action);

    void on_pb_tool1_clicked();

private:
    Ui::FriendList *ui;
    //把小控件添加到大控件上，需要有一个层layout
    QVBoxLayout* m_layout_friend;
    QVBoxLayout* m_layout_group;
    //定义一个菜单的对象
    QMenu* m_pMenu;

};

#endif // FRIENDLIST_H
