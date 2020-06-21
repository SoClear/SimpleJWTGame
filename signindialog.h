#ifndef SIGNINDIALOG_H
#define SIGNINDIALOG_H

#include <QDialog>

namespace Ui {
class SignInDialog;
}

class SignInDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignInDialog(QWidget *parent = nullptr);
    //当前登录成功用户的uid
    QString currentUid;
    ~SignInDialog();

private slots:
    void on_signIn_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::SignInDialog *ui;
};

#endif // SIGNINDIALOG_H
