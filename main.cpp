#include "mainwidget.h"
#include "signindialog.h"
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QObject>
#include <QDebug>
//打开数据库
bool openDB(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("JWT_mysql");
    db.setUserName("root");
    db.setPassword("123456");     //设置数据库连接账号的密码
    bool ok = db.open();
    if(ok){
        qDebug()<<"Database connected at main.cpp"<<endl;
    }else{
        qDebug()<<"Database connect failed at main.cpp"<<endl;

    }
    return ok;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget mainWidget;
    SignInDialog signIn;
    if(!openDB()||signIn.exec()!=QDialog::Accepted) exit(0);
//    qDebug()<<signIn.currentUid;
    mainWidget.currentUid=signIn.currentUid;
    mainWidget.refresh();
    mainWidget.on_refreshFight_pushButton_clicked();
    mainWidget.show();


    return a.exec();
}
