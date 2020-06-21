#include "signindialog.h"
#include "ui_signindialog.h"
#include "signupdialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>

SignInDialog::SignInDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignInDialog)
{
    ui->setupUi(this);
}

SignInDialog::~SignInDialog()
{
    delete ui;
}

void SignInDialog::on_signIn_pushButton_clicked()
{
    //新建查询对象
    QSqlQuery query;
    //获取用户输入的uid和password
    QString uid,password;
    uid=ui->uid->text();
    password=ui->password->text();

    if(query.exec("select * from accounts where uid='"+uid+"';")){
        if(query.next()){
            if(uid==""||password==""||uid!=query.value(0).toString()||password!=query.value(1).toString()){
                QMessageBox::warning(this, QString("登录失败"), QString("账号或密码错误"));
            }else{
                //设置为当前用户
                currentUid=uid;
                qDebug()<<"用户"+uid+"登录成功";
                //当前对话框被接受，并且关闭当前对话框
                this->accept();
            }
        }else {
            //没有找到相符的uid
            QMessageBox::warning(this, QString("登录失败"), QString("账号或密码错误"));
        }
    }else {
        qDebug()<<"查询失败";
    }

}

void SignInDialog::on_pushButton_2_clicked()
{
    SignUpDialog signUp;
    signUp.exec();
}
