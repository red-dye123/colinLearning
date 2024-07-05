#ifndef GROUPCHATDIALOG_H
#define GROUPCHATDIALOG_H

#include <QDialog>

namespace Ui {
class groupchatdialog;
}

class groupchatdialog : public QDialog
{
    Q_OBJECT
signals:
    void sig_sendGroupChatMessage(QString content,int id);
public:
    explicit groupchatdialog(QWidget *parent = nullptr);
    ~groupchatdialog();

    //设置聊天窗口信息
    void setChatInfo(int id,QString name);

    //设置聊天请求内容到窗口上
    void setChatCotent(QString fiendName,QString content);

private slots:
    void on_pb_send_clicked();

private:
    Ui::groupchatdialog *ui;
    int m_groupId;
    QString m_groupName;
};

#endif // GROUPCHATDIALOG_H
