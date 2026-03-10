#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "net.h"
#include "packet.h"
#include <QJsonObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QIcon>
#include <QCryptographicHash> // 哈希加密核心类
#include <QRandomGenerator>   // 生成随机盐值
LoginWidget::LoginWidget(QFrame *parent)
    : QFrame(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    mainwindow=nullptr;
    setWindowIcon(QIcon(":/icon/main.ico"));
    setFixedSize(500,350);
    ui->account_edit->setPlaceholderText("请输入账号");
    ui->password_edit->setPlaceholderText("请输入密码");
    ui->password_edit->setEchoMode(QLineEdit::Password);
    ui->account_lab->setPixmap(QPixmap(":/icon/account.png").scaled(20,20,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->password_lab->setPixmap(QPixmap(":/icon/password.png").scaled(20,20,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    btnGroup=new QButtonGroup(this);
    btnGroup->addButton(ui->commonuser_rbtn);
    btnGroup->addButton(ui->manager_rbtn);
    ui->commonuser_rbtn->setChecked(true);
    ui->account_edit->setText("13456650989");
    ui->password_edit->setText("qqq.czx");

    connect(&regeistWidget,&RegeistWidget::gotoLogin,this,&LoginWidget::showWindow);

    connect(Net::getInstance(),&Net::loginStatusChanged,this,&LoginWidget::onLogin);

}

LoginWidget::~LoginWidget()
{
    delete ui;
    if(btnGroup)
    {
        btnGroup->deleteLater();
        btnGroup=nullptr;
    }
}

void LoginWidget::on_login_btn_clicked()
{
    QString account=ui->account_edit->text();
    QString password=ui->password_edit->text();
    QString permission=ui->commonuser_rbtn->isChecked()?"reader":"admin";
    if(account.isEmpty())
    {
        ui->account_tip_lab->setText("请输入账号!");
        return;
    }
    if(password.isEmpty())
    {
         ui->password_tip_lab->setText("请输入密码!");
        return;
    }
    ui->account_tip_lab->clear();
    ui->password_tip_lab->clear();
    QJsonObject sendJson;
    sendJson["type"]=LOGIN_REQUEST;
    sendJson["account"]=account;
    sendJson["permission"]=permission;
    QJsonDocument doc(sendJson);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);
    Net::getInstance()->sendData(sendData);
}


void LoginWidget::on_exit_btn_clicked()
{
    this->close();
}


void LoginWidget::on_register_btn_clicked()
{
    regeistWidget.show();
    this->hide();
}

void LoginWidget::onLogin(const QJsonObject& info)
{
    int status=info["res"].toInt();
    if(status==SUCCESS)//查询成功
    {
        QString hashPassword=info["password"].toString();
        if(verifyPassword(ui->password_edit->text(),hashPassword))
        {
            QMessageBox::information(this,"成功","登录成功");
            mainwindow=new MainWindow(info["permission"].toString());
            mainwindow->initData();
            mainwindow->show();
            this->hide();
        }
        else
        {
            QMessageBox::information(this,"失败","登录失败,账号或密码错误");
        }
    }
    else
    {
        QMessageBox::information(this,"登录","登录失败,账号不存在");
    }

}
 void LoginWidget::showWindow()
{
     regeistWidget.close();
    this->show();
}

void LoginWidget::on_commonuser_rbtn_clicked(bool checked)
{
    if(checked)
    {
        ui->account_edit->setText("13456650989");
        ui->password_edit->setText("qqq.czx");
    }
}


void LoginWidget::on_manager_rbtn_clicked(bool checked)
{

    if(checked)
    {
        ui->account_edit->setText("19970982287");
        ui->password_edit->setText("abc.123");
    }
}


void LoginWidget::on_eye_btn_clicked(bool checked)
{
    if(checked)
    {
        ui->password_edit->setEchoMode(QLineEdit::Normal);
        ui->eye_btn->setIcon(QIcon(":/icon/eye_show.png"));
        ui->eye_btn->setIconSize(QSize(25,25));
    }
    else
    {
        ui->password_edit->setEchoMode(QLineEdit::Password);
        ui->eye_btn->setIcon(QIcon(":/icon/eye_hide.png"));
        ui->eye_btn->setIconSize(QSize(25,25));
    }
}

// 密码加密：明文 + 盐值 → SHA-256密文
QString LoginWidget::encryptPassword(const QString &plainPassword)
{
    QString mixStr = plainPassword + salt;
    QByteArray hashBytes = QCryptographicHash::hash(mixStr.toUtf8(), QCryptographicHash::Sha256);
    return hashBytes.toHex();
}

// 验证密码：明文 + 盐值 → 密文，与存储的密文比对
bool LoginWidget::verifyPassword(const QString &plainPassword,const QString &storedHash)
{
    qDebug()<<"------------";
    qDebug()<<"plainPassword:"<<plainPassword;
    QString newHash = encryptPassword(plainPassword);
    qDebug()<<"newHash:"<<newHash;
    qDebug()<<"storedHash:"<<storedHash;
    qDebug()<<"------------";
    return newHash == storedHash;
}
