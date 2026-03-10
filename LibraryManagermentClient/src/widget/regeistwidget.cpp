#include "regeistwidget.h"
#include "ui_regeistwidget.h"
#include "net.h"
#include <QMessageBox>
#include "packet.h"
#include <QDateTime>
#include <QRandomGenerator64>
#include <QMessageBox>
#include <QCryptographicHash> // 哈希加密核心类
RegeistWidget::RegeistWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegeistWidget)
{
    ui->setupUi(this);
    ui->password_tip_lab->setVisible(false);
    ui->password_edit1->setEchoMode(QLineEdit::Password);
    ui->password_edit2->setEchoMode(QLineEdit::Password);
    connect(Net::getInstance(),&Net::regeistStatus,this,&RegeistWidget::on_regeisStatusChanged);
}

RegeistWidget::~RegeistWidget()
{
    delete ui;
}

void RegeistWidget::on_regeist_btn_clicked()
{
    int number=QRandomGenerator64::global()->bounded(10000000,99999999);
    QString numberStr="RD"+QString::number(number);
    QString account=ui->account_edit->text();
    QString password1=ui->password_edit1->text();
    QString password2=ui->password_edit2->text();
    if(password1!=password2)
    {
        QMessageBox::critical(this,"提示","两次密码输入不一致");
        return;
    }
    QString nickname=ui->nickname_edit->text();
    QString name=ui->name_edit->text();
    QString phone=ui->phone_edit->text();
    QString email=ui->email_edit->text();

    if(account.isEmpty()||password1.isEmpty()||password2.isEmpty()||
        nickname.isEmpty()||name.isEmpty()||phone.isEmpty()||email.isEmpty())
    {
        QMessageBox::critical(this,"注册","请填写完整信息");
        return;
    }
    QString hashPassword=encryptPassword(password1);
    QString create_time=QDateTime::currentDateTime().toString("yyyy-MM-dd HH::mm::ss");
    QString last_login_time="NULL";
    QString sendStr=QString("{"
                      "\"type\":%1,"
                      "\"number\":\"%2\","
                      "\"account\":\"%3\","
                      "\"password\":\"%4\","
                      "\"nickname\":\"%5\","
                      "\"permission\":\"reader\","
                      "\"name\":\"%6\","
                      "\"phone\":\"%7\","
                      "\"email\":\"%8\","
                      "\"create_time\":\"%9\","
                      "\"last_login_time\":\"%10\","
                      "\"borrow_count\":0,"
                      "\"borrow_point\":100}")
                          .arg(REGEIST_REQUEST).arg(numberStr).arg(account)
                          .arg(hashPassword).arg(nickname).arg(name).arg(phone)
                          .arg(email).arg(create_time).arg(last_login_time);

    Net::getInstance()->sendData(sendStr.toUtf8());

}

 void RegeistWidget::on_regeisStatusChanged(int status)
{
    if(status==SUCCESS)
    {
         QMessageBox::information(this,"注册","注册成功!");
    }
    else if(status==ACCOUNT_REPEAT_ERROR)
    {
        QMessageBox::information(this,"注册","注册失败,账号或手机号码重复！");
    }
    else
    {
        QMessageBox::information(this,"注册","注册失败!");
    }

}
void RegeistWidget::on_exit_btn_clicked()
{
    this->close();
}


void RegeistWidget::on_password_edit2_textChanged(const QString &arg1)
{
     if(ui->password_edit1->text().isEmpty())return;
    if(arg1!=ui->password_edit1->text())
    {
         ui->password_tip_lab->setVisible(true);
    }
    else
    {
       ui->password_tip_lab->setVisible(false);
    }

}


void RegeistWidget::on_login_btn_clicked()
{
    emit gotoLogin();
}


void RegeistWidget::on_password_edit1_textChanged(const QString &arg1)
{
    if(ui->password_edit2->text().isEmpty())return;
    if(arg1!=ui->password_edit2->text())
    {
        ui->password_tip_lab->setVisible(true);
    }
    else
    {
        ui->password_tip_lab->setVisible(false);
    }
}
// 密码加密：明文 + 盐值 → SHA-256密文
QString RegeistWidget::encryptPassword(const QString &plainPassword)
{
    QString mixStr = plainPassword + salt;
    QByteArray hashBytes = QCryptographicHash::hash(mixStr.toUtf8(), QCryptographicHash::Sha256);
    return hashBytes.toHex();
}

