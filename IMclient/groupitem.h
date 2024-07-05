#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <QWidget>

namespace Ui {
class groupitem;
}

class groupitem : public QWidget
{
    Q_OBJECT
signals:
    //通知kernel显示聊天内容
    void sig_showGroupChatDialog(int id);
public:
    explicit groupitem(QWidget *parent = nullptr);
    ~groupitem();

    //保存并设置群聊信息
    void setGroupInfo(int groupID,int iconID,QString name);

    const QString &name() const;

private slots:
    void on_pb_icon_clicked();

private:
    Ui::groupitem *ui;
    int m_groupID;
    int m_iconID;
    QString m_name;
};

#endif // GROUPITEM_H
