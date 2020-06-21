#include "signupdialog.h"
#include "ui_signupdialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>

SignUpDialog::SignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUpDialog)
{
    ui->setupUi(this);
}

SignUpDialog::~SignUpDialog()
{
    delete ui;
}


void SignUpDialog::on_ok_pushButton_clicked()
{
    QString uid,password,ensurePassword;
    uid=ui->uid->text();
    password=ui->password->text();
    ensurePassword=ui->ensurePassword->text();

    if(uid==""||password==""||ensurePassword==""){
        QMessageBox::warning(this,"注册失败","输入的数据有误，请重新输入。");
    }else if (password!=ensurePassword) {
        QMessageBox::warning(this,"注册失败","两次输入的密码不一致");
    }else{
        QSqlQuery query;
        if(query.exec("select uid from accounts where uid='"+uid+"';")){
            if(query.next()){
                QMessageBox::warning(this,"注册失败","uid已被占用");
            }else {
                query.exec("insert into accounts values('"+uid+"','"+password+"');");
                query.exec("insert into bags (uid) values ('"+uid+"');");
                query.exec("insert into friends (uid) values ('"+uid+"');");
                query.exec("insert into players (uid) values ('"+uid+"');");
                QMessageBox::warning(this,"注册成功","账号："+uid+"\n密码："+password+"\n请牢记！！！");
                this->close();
            }
        }else {
            qDebug()<<"查询失败";
        }
    }
}
