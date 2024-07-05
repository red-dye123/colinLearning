#include "imdialog.h"
#include "ui_imdialog.h"
#include <QDebug>
#include <QMessageBox>

IMDialog::IMDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::IMDialog)
{
    ui->setupUi(this);
}

IMDialog::~IMDialog()
{
    delete ui;
}

void IMDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT sig_closeApp();
}


void IMDialog::on_pb_clear_register_clicked()
{
    qDebug()<<__func__;
    ui->le_name_register->setText("");
    ui->le_password_register->setText("");
    ui->le_tel_register->setText("");
}


void IMDialog::on_pb_commit_register_clicked()
{
    qDebug()<<__func__;
    //1.从控件上获取用户输入的数据
    QString name=ui->le_name_register->text();
    QString tel=ui->le_tel_register->text();
    QString pass=ui->le_password_register->text();
    QString nameTmp=name;
    QString telTmp=tel;
    QString passTmp=pass;

    //2.校验用户数据合法性
    //2.1校验是否为空或全空格（移除所有空格后是否为空）
    if(name.isEmpty() || tel.isEmpty() || pass.isEmpty() || nameTmp.remove(" ").isEmpty() || telTmp.remove(" ").isEmpty() || passTmp.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","输入的内容不能为空或者全空格");
        return ;
    }
    //2.2校验长度(昵称不能超过15，密码不超过20，电话号码必须为11位)
    if(name.length()>15 || pass.length()>20 || tel.length()!=11){
        QMessageBox::about(this,"提示","昵称不能超过15，密码不超过20，电话号码必须为11位");
        return ;
    }
    //2.3 校验内容（电话号码必须是数字，满足电话号码的规律；昵称不允许有特殊字符；密码只能是字母、数字、下划线）——TODO:正则表达式

    //3.把数据传给kernel
    Q_EMIT sig_commitRegister(name,tel,pass);
}


void IMDialog::on_pb_clear_clicked()
{
    qDebug()<<__func__;
    ui->le_password->setText("");
    ui->le_tel->setText("");
}


void IMDialog::on_pb_commit_clicked()
{
    qDebug()<<__func__;
    //1.从控件上获取用户输入的数据
    QString tel=ui->le_tel->text();
    QString pass=ui->le_password->text();
    QString telTmp=tel;
    QString passTmp=pass;

    //2.校验用户数据合法性
    //2.1校验是否为空或全空格（移除所有空格后是否为空）
    if(tel.isEmpty() || pass.isEmpty() || telTmp.remove(" ").isEmpty() || passTmp.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","输入的内容不能为空或者全空格");
        return ;
    }
    //2.2校验长度(密码不超过20，电话号码必须为11位)
    if(pass.length()>20 || tel.length()!=11){
        QMessageBox::about(this,"提示","密码不超过20，电话号码必须为11位");
        return ;
    }
    //2.3 校验内容（电话号码必须是数字，满足电话号码的规律；密码只能是字母、数字、下划线）——TODO:正则表达式

    //3.把登录信息传给kernel
    Q_EMIT sig_commitLogin(tel,pass);
}

