#include "groupchatdialog.h"
#include "ui_groupchatdialog.h"
#include <QTime>

groupchatdialog::groupchatdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::groupchatdialog)
{
    ui->setupUi(this);
}

groupchatdialog::~groupchatdialog()
{
    delete ui;
}

void groupchatdialog::setChatInfo(int id, QString name)
{
    //保存信息
    m_groupId=id;
    m_groupName=name;
    //设置到控件上
    setWindowTitle(QString("【%1】群聊窗口").arg(m_groupName));
}

void groupchatdialog::setChatCotent(QString friendName,QString content)
{
    ui->tb_chat->append(QString("【%1】 %2").arg(friendName).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}

void groupchatdialog::on_pb_send_clicked()
{
    //1.从控件上获取纯文本数据
    QString content=ui->te_chat->toPlainText();

    //2.校验用户输入的数据是否合法
    if(content.isEmpty() || content.remove(" ").isEmpty()){
        ui->te_chat->setText("");
        return;
    }

    //3.获取带格式的数据
    content=ui->te_chat->toHtml();

    //4.清空编辑窗口
    ui->te_chat->setText("");

    //5.把数据显示到浏览窗口上
    ui->tb_chat->append(QString("【我】 %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);

    //6.把数据发给kernel
    Q_EMIT sig_sendGroupChatMessage(content,m_groupId);
}

