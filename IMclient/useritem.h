#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>

namespace Ui {
class useritem;
}

class useritem : public QWidget
{
    Q_OBJECT
signals:
    //通知kernel显示聊天内容
    void sig_showChatDialog(int id);
public:
    explicit useritem(QWidget *parent = nullptr);
    ~useritem();

    //保存并设置好友信息
    void setFriendInfo(int friendID,int status,int iconID,QString name,QString feeling);

    //设置好友的状态为下线
    void setFriendOffline();

    const QString &name() const;

private slots:
    void on_pb_icon_clicked();

private:
    Ui::useritem *ui;
    int m_friendID;
    int m_status;
    int m_iconID;
    QString m_name;
    QString m_feeling;

};

#endif // USERITEM_H
