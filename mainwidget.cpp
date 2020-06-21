#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QMessageBox>
#include <QDebug>
#include <QInputDialog>
#include <QSqlQuery>


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::fight(int whichStrongEnemy){
    QSqlQuery query;
    query.exec("select lv,hp,ap,dp,qi from players where uid='"+currentUid+"';");
    query.next();
    int lv,hp,ap,dp,qi;
    lv=query.value(0).toInt();
    hp=query.value(1).toInt();
    ap=query.value(2).toInt();
    dp=query.value(3).toInt();
    qi=query.value(4).toInt();

    if(qi>=5){
        int health=hp*20;
        int attack=ap*10;
        int defense=dp*5;

        int enemyHealth=(lv+whichStrongEnemy)*20;
        int enemyAttack=(lv+whichStrongEnemy)*10;
        int enemyDefense=(lv+whichStrongEnemy)*5;

        int myRealDamage=attack-enemyDefense;
        int enemyRealDamage=enemyAttack-defense;



        if(myRealDamage<0)myRealDamage=0;
        if(enemyRealDamage<0)enemyRealDamage=0;

        if(myRealDamage==0&&enemyRealDamage==0){
            query.exec("update players set exp=exp+20+lv,qi=qi-5 WHERE uid='"+currentUid+"';");
            QMessageBox::information(this,"比武结果","比武胜利，经验+"+QString::number(20+lv)+"，气血-5");
        }else {
            bool successFlag=false;
            while (true) {
                //玩家先手
                enemyHealth-=myRealDamage;
                if(enemyHealth<=0){successFlag=true;break;}
                health-=enemyRealDamage;
                if(health<=0){successFlag=false;break;}
            }
            if(successFlag){
                //攻击越牛逼的敌人，加经验越多
                query.exec("update players set exp=exp+20+"+QString::number(lv*(whichStrongEnemy+1))+",qi=qi-5 WHERE uid='"+currentUid+"';");
                QMessageBox::information(this,"比武结果","比武胜利，经验+"+QString::number(lv*(whichStrongEnemy+1))+"，气血-5");
            }else {
                query.exec("update players set qi=qi-5 WHERE uid='"+currentUid+"';");
                QMessageBox::information(this,"比武结果","比武失败，气血-5");
            }
        }
        //刷新界面
        refresh();
    }else {
        QMessageBox::warning(this,"比武结果","气血值不足！");
    }
}


void MainWidget::refresh(){
    QSqlQuery query;
    query.exec("select lv,exp,qi,trainingRoomStartTime from players where uid='"+currentUid+"';");
    query.next();
    int lv,exp,qi;
    QString trainingRoomStartTime;
    lv=query.value(0).toInt();
    exp=query.value(1).toInt();
    qi=query.value(2).toInt();
    trainingRoomStartTime=query.value(3).toString();
    ui->welcome_label->setText("欢迎您，"+currentUid+"("+QString::number(lv)+"级）");
    ui->EXP_label->setText("经验："+QString::number(exp)+"/"+QString::number(lv*100));
    ui->qi_label->setText("气血："+QString::number(qi));


    query.exec("SELECT TIMESTAMPDIFF(HOUR,(SELECT trainingRoomStartTime FROM players WHERE uid='"+currentUid+"'),(SELECT CURRENT_TIMESTAMP));");
    query.next();
    if(query.value(0).isNull()){
        ui->trainingRoomState_label->setText("练功房：未修炼");
        ui->trainingStartOrStop_pushButton->setText("开始");
    }else {
        int during=query.value(0).toInt();
        if(during<4){
            ui->trainingRoomState_label->setText("练功房：未满4小时");
            ui->trainingStartOrStop_pushButton->hide();
        }else {
            ui->trainingRoomState_label->setText("练功房：已满4小时");
            ui->trainingStartOrStop_pushButton->setText("停止");
            ui->trainingStartOrStop_pushButton->show();
        }
    }

    this->update();
}


void MainWidget::on_myFriends_pushButton_clicked()
{
    //好友模块太复杂了，这里按照没有好友做吧。
    //QMessageBox::warning(nullptr, QString("好友"), QString("您目前没有好友！"), QMessageBox::Yes, QMessageBox::Yes);
    QSqlQuery query;
    query.exec("select uid from accounts where uid!='"+currentUid+"';");
    QDialog *myFriendsDialog=new QDialog(this);
    QVBoxLayout *vLayout=new QVBoxLayout(myFriendsDialog);
    while (query.next()) {
        QLabel *label=new QLabel(query.value(0).toString(),myFriendsDialog);
        vLayout->addWidget(label);
    }
    myFriendsDialog->setLayout(vLayout);
    myFriendsDialog->exec();
}

void MainWidget::on_charge_pushButton_clicked()
{
    bool isOk=false;
    int money=QInputDialog::getInt(this,"充值","请输入充值GB数量",1,1,1000000,1,&isOk);
    if(isOk){
        QSqlQuery query;
        query.exec("update bags set GB=GB+"+QString::number(money)+" where uid='"+currentUid+"';");
        QMessageBox::information(this,"充值成功","成功充值了"+QString::number(money)+"GB");
        qDebug()<<"玩家充钱了，他充了"+QString::number(money)+"GB";
    }else {
        qDebug()<<"玩家看了看自己的钱包，并没有充钱";
    }
}

void MainWidget::on_shop_pushButton_clicked()
{
    //新建一个查询的实例
    QSqlQuery query;
    //尝试查询数据库
    if(query.exec("select * from shop;")){
        //查询成功，则

        //新建QDialog控件items
        //使用QDialog而不是QWidget的原因是QDialog是弹出的，而且能阻塞父窗口
        QDialog *items =new QDialog(this);

        //新建垂直布局
        QVBoxLayout *layout = new QVBoxLayout;

        //QSqlQuery返回的数据集，record是停在第一条记录之前的。
        //所以，你获得数据集后，必须执行next()或first()到第一条记录，
        //这时候record才是有效的。否则，exec完直接value肯定报错
        if(query.next()){
            //声明商品各项信息
            QString id,itemName,comment;
            int cost;

            //让记录指针回到第一条记录之前
            query.previous();
            //有下一条记录时，循环创建按钮
            while (query.next()) {
                //为商品的各项信息赋值
                id=query.value(0).toString();
                itemName=query.value(1).toString();
                cost=query.value(2).toInt();
                comment=query.value(5).toString();



                //创建按钮
                QPushButton *button=new QPushButton(id+itemName,this);

                //设置点击按钮的功能
                connect(button,&QPushButton::clicked,this,[=](){
                    //玩家是否确认购买
                    bool isOk=false;
                    //获取当前行囊中GB数量
                    QSqlQuery queryGB;
                    queryGB.exec("select GB from bags WHERE uid='"+currentUid+"';");
                    //别忘了指针指向第一条结果
                    queryGB.next();
                    int currentGB=queryGB.value(0).toInt();
                    //购买物品时的提示
                    QString purchaseTips=
                            "当前行囊中GB数量："+QString::number(currentGB)+
                            "\n\n商品："+itemName+
                            "\n单价："+QString::number(cost)+" GB"+
                            "\n功效："+comment+
                            "\n\n请输入购买数量↓";
                    //弹出购买对话框
                    int count=QInputDialog::getInt(this,"购买物品",purchaseTips,1,1,1000000,1,&isOk);
                    if(isOk){
                        if(currentGB-cost*count>=0){
                            QSqlQuery changeGB;
                            changeGB.exec("update bags set GB=GB-"+QString::number(cost*count)+" where uid='"+currentUid+"';");
                            changeGB.exec("update bags set "+itemName+"="+itemName+"+"+QString::number(count)+" where uid='"+currentUid+"';");
                            QMessageBox::information(this,"购买成功","购买了"+QString::number(count)+"个"+itemName);
                        }else {
                            QMessageBox::warning(this,"购买失败","当前行囊中GB数量："+QString::number(currentGB)+"，还需要"+QString::number(cost*count-currentGB)+"GB");
                        }

                    }else {
                        qDebug()<<"玩家看了看商店，却没有买东西";
                    }
                });
                //在垂直布局中添加按钮
                layout->addWidget(button);
            }

            //设置布局不要有空余
            //layout->setSpacing(0);

            //设置items的布局为layout
            items->setLayout(layout);

            //设置窗口大小为150*200
            //items->setFixedSize(150,200);

            //显示item（exec()是模态，show()是非模态）
            items->exec();

        }else {
            qDebug()<<"没有记录";
        }

    }else {
        qDebug()<<"数据库查询失败";
    }
}


void MainWidget::on_bag_pushButton_clicked()
{
    //新建一个查询的实例
    QSqlQuery query;
    //尝试查询数据库
    if(query.exec("select * from shop;")){
        //查询成功，则

        //新建QDialog控件items
        //使用QDialog而不是QWidget的原因是QDialog是弹出的，而且能阻塞父窗口
        QDialog *items =new QDialog(this);

        //新建垂直布局
        QVBoxLayout *layout = new QVBoxLayout;

        //QSqlQuery返回的数据集，record是停在第一条记录之前的。
        //所以，你获得数据集后，必须执行next()或first()到第一条记录，
        //这时候record才是有效的。否则，exec完直接value肯定报错
        if(query.next()){
            //声明商店中商品各项信息
            QString id,itemName,comment;
            int addQi,addEXP;

            //让记录指针回到第一条记录之前
            query.previous();

            //获取当前行囊中GB数量和各个物品的数量
            QSqlQuery queryMyBag;
            queryMyBag.exec("select GB from bags WHERE uid='"+currentUid+"';");
            //,`气血丸小号`,`气血丸中号`,`气血丸大号`,`经验书小号`,`经验书中号`,`经验书大号`
            //别忘了指针指向第一条结果
            queryMyBag.next();
            int currentGB=queryMyBag.value(0).toInt();

            QLabel *GB_label=new QLabel("当前GB："+QString::number(currentGB));
            layout->addWidget(GB_label);

            int currentGoodsCount=0;
            //有下一条记录时，循环创建按钮
            while (query.next()) {
                //为商品的各项信息赋值
                id=query.value(0).toString();
                itemName=query.value(1).toString();
                addQi=query.value(3).toInt();
                addEXP=query.value(4).toInt();
                comment=query.value(5).toString();



                //创建按钮
                QPushButton *button=new QPushButton(id+itemName,this);

                //设置点击按钮的功能
                connect(button,&QPushButton::clicked,this,[=](){
                    //玩家是否确认使用
                    bool isOk=false;
                    QSqlQuery goodsQuery;
                    goodsQuery.exec("select `"+itemName+"` from bags where uid="+"'"+currentUid+"';");
                    goodsQuery.next();
                    int goodsCount=goodsQuery.value(0).toInt();
                    //使用物品时的提示
                    QString purchaseTips=
                            "物品："+itemName+
                            "\n功效："+comment+
                            "\n\n拥有数量："+QString::number(goodsCount)+
                            "\n\n请输入使用数量↓";
                    //弹出使用物品对话框
                    int count=QInputDialog::getInt(this,"使用物品",purchaseTips,1,1,1000000,1,&isOk);
                    if(isOk){
                        if(goodsCount-count>=0){
                            QSqlQuery change;
                            change.exec("update bags set `"+itemName+"`=`"+itemName+"`-"+QString::number(count)+" where uid='"+currentUid+"';");
                            change.exec("update players set qi=qi+"+QString::number(addQi*count)+" where uid='"+currentUid+"';");
                            change.exec("update players set exp=exp+"+QString::number(addEXP*count)+" where uid='"+currentUid+"';");
                            QMessageBox::information(this,"使用成功","使用了"+QString::number(count)+"个"+itemName);
                            //刷新主界面状态
                            this->refresh();
                        }else {
                            QMessageBox::warning(this,"使用失败","行囊中此物品数量不足。");
                        }

                    }else {
                        qDebug()<<"玩家看了看背包，没有使用东西";
                    }
                });
                //在垂直布局中添加按钮
                layout->addWidget(button);
                //当前物品项目加一
                currentGoodsCount++;
            }

            //设置布局不要有空余
            //layout->setSpacing(0);

            //设置items的布局为layout
            items->setLayout(layout);

            //设置窗口大小为150*200
            //items->setFixedSize(150,200);

            //显示item（exec()是模态，show()是非模态）
            items->exec();

        }else {
            qDebug()<<"没有记录";
        }

    }else {
        qDebug()<<"数据库查询失败";
    }
}


void MainWidget::on_attribute_pushButton_clicked()
{
    QDialog *attributeDialog=new QDialog(this);
    QVBoxLayout *vLayout=new QVBoxLayout(attributeDialog);

    QSqlQuery query;
    query.exec("select * from players where uid='"+currentUid+"';");
    if(query.next()){
        QString properties[9]={"账号：","等级：","经验：","气血：","血量：","攻击：","防御：","上次练功房开始时间：","待分配技能点："};
        //hp,ap,dp;
        //真实属性为hp*20,ap*10,dp*5
        int power[3]={20,10,5};
        int fightPoint[3];
        //八个标签对应上面properties的前八个
        QLabel *label_p[8];
        for (int i=0;i<8;i++) {

            if(i>=4&&i<=6){
                fightPoint[i-4]=query.value(i).toInt();

//                properties[i]+=QString::number(fightPoint[i-4]*power[i-4]);
                label_p[i]=new QLabel(properties[i]+QString::number(fightPoint[i-4]*power[i-4]));
            }else {
//                properties[i]+=query.value(i).toString();
                label_p[i]=new QLabel(properties[i]+query.value(i).toString());
            }

            vLayout->addWidget(label_p[i]);
        }
        //当前待分配的技能点
        int skillPoint=query.value(8).toInt();
        //显示待分配技能点的标签
        QLabel *label1=new QLabel(properties[8]+QString::number(skillPoint));
        //分配技能点的按钮
        QPushButton *assign=new QPushButton("分配");
        //分配技能点的功能
        connect(assign,&QPushButton::clicked,this,[=](){

            QDialog *assignSkillPoints=new QDialog(attributeDialog);
            QVBoxLayout *vLayout=new QVBoxLayout(assignSkillPoints);
            QString str[5]={"待分配技能点：","血量点：","攻击点：","防御点：","全部重新分配"};
            QString spStr[3]={"hp","ap","dp"};

            //查询当前技能点
            QSqlQuery query;
            query.exec("select skillPoint from players where uid='"+currentUid+"';");
            query.next();
            //如果当前技能点等于skillPoint，则刷新界面
            if(query.value(0).toInt()!=skillPoint){
                label1->setText(properties[8]+QString(query.value(0).toString()));
                attributeDialog->update();
            }
            //待分配技能点的标签
            QLabel *waitingAssignSkillPoint=new QLabel(str[0]+QString(query.value(0).toString()));
            vLayout->addWidget(waitingAssignSkillPoint);
            //各种fightPoint的标签
            QLabel *fightPoint_label[3];
            for (int i=1;i<4;i++) {
                //str[i]+=QString::number(fightPoint[i-1]);
                //各种fightPoint的标签
                fightPoint_label[i-1]=new QLabel(str[i]+QString::number(fightPoint[i-1]));
                QPushButton *pushButton=new QPushButton("+1",assignSkillPoints);
                //技能点加一按钮的功能
                connect(pushButton,&QPushButton::clicked,assignSkillPoints,[=](){
                    QSqlQuery spQuery;
                    //从数据库中查询skillPoint,hp,ap,dp
                    spQuery.exec("select skillPoint,hp,ap,dp from players where uid='"+currentUid+"'");
                    spQuery.next();
                    //从数据库中查询到的待分配的技能点
                    int currentSP=spQuery.value(0).toInt();
                    int currentHP=spQuery.value(1).toInt();
                    int currentAP=spQuery.value(2).toInt();
                    int currentDP=spQuery.value(3).toInt();
                    int currentFP[3]={currentHP,currentAP,currentDP};
                    //判断是否有足量的skillPoint
                    if(currentSP>=1){
                        QSqlQuery query;
                        //数据库中的 skillPoint(待分配技能点)-1，对应的技能点+1
                        query.exec("update players set skillPoint=skillPoint-1 ,"+spStr[i-1]+"="+spStr[i-1]+"+1"+" WHERE uid='"+currentUid+"';");
                        //改变显示的技能点余量标签的文字
                        waitingAssignSkillPoint->setText(str[0]+QString::number(currentSP-1));
                        //改变显示的当前fightPoint标签的文字
                        fightPoint_label[i-1]->setText(str[i]+QString::number(currentFP[i-1]+1));

                        //改变父窗口显示待分配技能点的标签label1
                        label1->setText(properties[8]+QString::number(currentSP-1));


                        //根据当前的i值改变父窗口显示血量的标签label_p[4],显示攻击的标签5,显示防御的标签6
                        label_p[i+3]->setText(properties[i+3]+QString::number((currentFP[i-1]+1)*power[i-1]));

                        //刷新界面
                        assignSkillPoints->update();
                        //刷新父界面
                        attributeDialog->update();
                    }else {
                        QMessageBox::warning(assignSkillPoints,"分配失败","没有足够的技能点");
                    }
                });
                QHBoxLayout *hLayout=new QHBoxLayout(assignSkillPoints);

                hLayout->addWidget(fightPoint_label[i-1]);
                hLayout->addWidget(pushButton);
                vLayout->addLayout(hLayout);
            }
            QPushButton *reassign=new QPushButton(str[4]);
            //全部重新分配技能点的功能
            connect(reassign,&QPushButton::clicked,assignSkillPoints,[=](){
                //TODO

                QSqlQuery query;
                query.exec("UPDATE players set hp=0,ap=0,dp=0,skillPoint=lv*3 WHERE uid='"+currentUid+"';");
                for (int i=0;i<3;i++) {
                    fightPoint_label[i]->setText(str[i+1]+"0");
                }
                query.exec("select lv from players where uid='"+currentUid+"';");
                query.next();
                int lv=query.value(0).toInt();
                waitingAssignSkillPoint->setText(str[0]+QString::number(lv*3));
                assignSkillPoints->update();

            });
            vLayout->addWidget(reassign);
            assignSkillPoints->setLayout(vLayout);
            assignSkillPoints->exec();
        });
        QHBoxLayout *hLayout=new QHBoxLayout();
        hLayout->addWidget(label1);
        hLayout->addWidget(assign);
        vLayout->addLayout(hLayout);
        attributeDialog->setLayout(vLayout);
        attributeDialog->exec();
    }

}

void MainWidget::on_refresh_pushButton_clicked()
{
    refresh();
}

void MainWidget::on_trainingStartOrStop_pushButton_clicked()
{
    QSqlQuery query;
    query.exec("SELECT TIMESTAMPDIFF(HOUR,(SELECT trainingRoomStartTime FROM players WHERE uid='"+currentUid+"'),(SELECT CURRENT_TIMESTAMP));");
    query.next();
    int during=query.value(0).toInt();
    if(query.value(0).isNull()){
        //为空即是未修练，未修练时点击此按钮应该开始修炼
        query.exec("UPDATE players set trainingRoomStartTime=(SELECT CURRENT_TIMESTAMP) WHERE uid='"+currentUid+"';");
    }else if(during>=4){
        //修炼满4小时，点击此按钮是为了停止修炼，获取收益
        //重置为未修练状态
        query.exec("UPDATE players set trainingRoomStartTime=NULL WHERE uid='"+currentUid+"';");
        //增加相应的经验和气血
        query.exec("UPDATE players SET exp=exp+(1000+lv*20), qi=qi+(50+lv) WHERE uid='"+currentUid+"';");
    }
    //修炼不足4小时的情况下，此按钮被隐藏，并不会被点击。
    //点击此按钮就两种情况，未修练和修炼满4小时
    //隐藏不用考虑未满4小时的情况
    //else {}

    //刷新界面
    refresh();
}

void MainWidget::on_upgrade_pushButton_clicked()
{
    QSqlQuery query;
    query.exec("select lv,exp from players where uid='"+currentUid+"';");
    query.next();
    int lv=query.value(0).toInt();
    int exp=query.value(1).toInt();
    if(exp-lv*100>=0){
        query.exec("update players set exp=exp-lv*100, lv=lv+1, skillPoint=skillPoint+3 where uid='"+currentUid+"';");
        refresh();
    }else {
        QMessageBox::warning(this,"升级失败","经验不足,还差"+QString::number(lv*100-exp)+"点经验。");
    }
    on_refreshFight_pushButton_clicked();
}

void MainWidget::on_refreshFight_pushButton_clicked()
{
    QSqlQuery query;
    query.exec("select lv from players where uid='"+currentUid+"';");
    query.next();
    int lv=query.value(0).toInt();
    ui->fight1_label->setText("1.新人1（"+QString::number(lv)+")级");
    ui->fight2_label->setText("2.新人2（"+QString::number(lv+1)+")级");
    ui->fight3_label->setText("3.新人3（"+QString::number(lv+2)+")级");
    ui->fight4_label->setText("4.新人4（"+QString::number(lv+3)+")级");
    ui->fight5_label->setText("5.新人5（"+QString::number(lv+4)+")级");
}

void MainWidget::on_fight1_pushButton_clicked()
{
    fight(0);
}

void MainWidget::on_fight2_pushButton_clicked()
{
    fight(1);
}

void MainWidget::on_fight3_pushButton_clicked()
{
    fight(2);
}

void MainWidget::on_fight4_pushButton_clicked()
{
    fight(3);
}

void MainWidget::on_fight5_pushButton_clicked()
{
    fight(4);
}
