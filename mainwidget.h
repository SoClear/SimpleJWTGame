#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSqlQuery>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    QString currentUid;
    void refresh();
public slots:
    void on_refreshFight_pushButton_clicked();

private slots:
    void on_myFriends_pushButton_clicked();

    void on_charge_pushButton_clicked();

    void on_shop_pushButton_clicked();

    void on_bag_pushButton_clicked();

    void on_attribute_pushButton_clicked();

    void on_refresh_pushButton_clicked();

    void on_trainingStartOrStop_pushButton_clicked();

    void on_upgrade_pushButton_clicked();



    void on_fight1_pushButton_clicked();

    void on_fight2_pushButton_clicked();

    void on_fight3_pushButton_clicked();

    void on_fight4_pushButton_clicked();

    void on_fight5_pushButton_clicked();

private:
    Ui::MainWidget *ui;
    void fight(int whichStrongEnemy);

};

#endif // MAINWIDGET_H
