#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QTime>

chatDialog::chatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chatDialog)
{
    ui->setupUi(this);
}

chatDialog::~chatDialog()
{
    delete ui;
}

//设置聊天窗口信息
void chatDialog::setChatInfo(int id, QString name)
{
    //保存信息
    m_friendId=id;
    m_friendName=name;
    //设置到控件上
    setWindowTitle(QString("与【%1】的聊天窗口").arg(m_friendName));
}

void chatDialog::setChatCotent(QString content)
{
    ui->tb_chat->append(QString("【%1】 %2").arg(m_friendName).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}

void chatDialog::setFriendOffline()
{
    ui->tb_chat->append(QString("【%1】 %2 不在线").arg(m_friendName).arg(QTime::currentTime().toString("hh:mm:ss")));
}

void chatDialog::on_pb_send_clicked()
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
    Q_EMIT sig_sendChatMessage(content,m_friendId);
}

