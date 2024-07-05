#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class chatDialog;
}

class chatDialog : public QDialog
{
    Q_OBJECT
signals:
    //把聊天内容发给kernel
    void sig_sendChatMessage(QString content,int id);

public:
    explicit chatDialog(QWidget *parent = nullptr);
    ~chatDialog();
    //设置聊天窗口信息
    void setChatInfo(int id,QString name);

    //设置聊天请求内容到窗口上
    void setChatCotent(QString content);
    //显示聊天对象不在线
    void setFriendOffline();

private slots:
    void on_pb_send_clicked();

private:
    Ui::chatDialog *ui;
    int m_friendId;
    QString m_friendName;
};

#endif // CHATDIALOG_H
