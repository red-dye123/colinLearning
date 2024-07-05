#include "groupitem.h"
#include "ui_groupitem.h"
#include "net/def.h"

groupitem::groupitem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::groupitem)
{
    ui->setupUi(this);
}

groupitem::~groupitem()
{
    delete ui;
}

void groupitem::setGroupInfo(int groupID, int iconID, QString name)
{
    //保存到成员变量中
    m_groupID=groupID;
    m_iconID=iconID;
    m_name=name;

    //设置到控件上
    ui->lb_name->setText(m_name);
    //拼接图片路径
    QString path=QString(":/tx/%1.png").arg(m_iconID);
    ui->pb_icon->setIcon(QIcon(path));

}

const QString &groupitem::name() const
{
    return m_name;
}

void groupitem::on_pb_icon_clicked()
{
    Q_EMIT sig_showGroupChatDialog(m_groupID);
}

