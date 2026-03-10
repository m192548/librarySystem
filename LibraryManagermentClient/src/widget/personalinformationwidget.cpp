#include "personalinformationwidget.h"
#include "ui_personalinformationwidget.h"
#include "net.h"
#include "packet.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCryptographicHash>
PersonalInformationWidget::PersonalInformationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PersonalInformationWidget)
{
    ui->setupUi(this);
    ui->reader_no_edit->setReadOnly(true);
    ui->account_edit->setReadOnly(true);
    ui->nick_name_edit->setReadOnly(true);
    ui->name_edit->setReadOnly(true);
    ui->phone_edit->setReadOnly(true);
    ui->email_edit->setReadOnly(true);
    ui->borrow_count_edit->setReadOnly(true);
    ui->borrow_point_edit->setReadOnly(true);


    ui->save_btn->setEnabled(false);

    connect(Net::getInstance(),&Net::personalInfoLoaded,this,&PersonalInformationWidget::onLoadPersonalInfomation);
    connect(Net::getInstance(),&Net::modefyPersonalInfoStatusChanged,this,&PersonalInformationWidget::onModefyPersonalInfo);
    connect(Net::getInstance(),&Net::modefyPasswordStatusChanged,this,&PersonalInformationWidget::onModefyPassword);

}

PersonalInformationWidget::~PersonalInformationWidget()
{
    delete ui;
}

void PersonalInformationWidget::on_edit_info_btn_clicked(bool checked)
{
    if(checked)
    {
        ui->account_edit->setReadOnly(false);
        ui->nick_name_edit->setReadOnly(false);
        ui->name_edit->setReadOnly(false);
        ui->phone_edit->setReadOnly(false);
        ui->email_edit->setReadOnly(false);

        ui->account_edit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        ui->nick_name_edit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        ui->name_edit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        ui->phone_edit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        ui->email_edit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

        ui->save_btn->setEnabled(true);
    }
    else
    {
        ui->account_edit->setReadOnly(true);
        ui->nick_name_edit->setReadOnly(true);
        ui->name_edit->setReadOnly(true);
        ui->phone_edit->setReadOnly(true);
        ui->email_edit->setReadOnly(true);

        ui->account_edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        ui->nick_name_edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        ui->name_edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        ui->phone_edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        ui->email_edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);

        ui->save_btn->setEnabled(false);
    }
}
void PersonalInformationWidget::onLoadPersonalInfomation(const QJsonObject& info)
{
    QString reader_no=info["number"].toString();
    QString account=info["account"].toString();
    hashPassword=info["password"].toString();
    QString nick_name=info["nickname"].toString();
    QString name=info["name"].toString();
    QString phone=info["phone"].toString();
    QString email=info["email"].toString();
    QString borrow_count=info["borrow_count"].toString();
    QString borrow_point=info["borrow_point"].toString();

    ui->reader_no_edit->setText(reader_no);
    ui->account_edit->setText(account);
    ui->nick_name_edit->setText(nick_name);
    ui->name_edit->setText(name);
    ui->phone_edit->setText(phone);
    ui->email_edit->setText(email);
    ui->borrow_count_edit->setText(borrow_count);
    ui->borrow_point_edit->setText(borrow_point);

}

void PersonalInformationWidget::on_save_btn_clicked()
{
    QString number=ui->reader_no_edit->text();
    QString nickname=ui->nick_name_edit->text();
    QString name=ui->name_edit->text();
    QString phone=ui->phone_edit->text();
    QString email=ui->email_edit->text();

    QJsonObject obj;
    obj["type"]=MODEFYPERSONALINFO_REQUEST;
    obj["number"]=number;
    obj["nickname"]=nickname;
    obj["name"]=name;
    obj["phone"]=phone;
    obj["email"]=email;

    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);

    Net::getInstance()->sendData(sendData);
}

void PersonalInformationWidget::onModefyPersonalInfo(int status)
{
    if(status==SUCCESS)
    {
        QMessageBox::information(this,"保存","保存成功!");
        on_edit_info_btn_clicked(false);
        ui->edit_info_btn->setChecked(false);
    }
    else
    {
        QMessageBox::information(this,"保存","保存失败!");
    }
}

void PersonalInformationWidget::on_modefy_password_btn_clicked()
{
    QDialog dlg;
    dlg.setFixedSize(300,200);
    QVBoxLayout totalLayout(&dlg);

    QLabel old_password_lab("原密码",&dlg);
    old_password_lab.setFixedHeight(25);
    old_password_lab.setAlignment(Qt::AlignmentFlag::AlignLeft);
    QLabel new_password_lab1("新密码",&dlg);
    new_password_lab1.setFixedHeight(25);
    new_password_lab1.setAlignment(Qt::AlignmentFlag::AlignLeft);

    QLabel new_password_lab2("请再次输入新密码",&dlg);
    new_password_lab2.setFixedHeight(25);
    new_password_lab2.setAlignment(Qt::AlignmentFlag::AlignLeft);
    QLineEdit old_password_edit(&dlg),new_password_edit1(&dlg),new_password_edit2(&dlg);
    old_password_edit.setFixedHeight(25);
    new_password_edit1.setFixedHeight(25);
    new_password_edit2.setFixedHeight(25);

    old_password_edit.setEchoMode(QLineEdit::Password);
    new_password_edit1.setEchoMode(QLineEdit::Password);
    new_password_edit2.setEchoMode(QLineEdit::Password);

    QGridLayout gridlayout(&dlg);
    gridlayout.addWidget(&old_password_lab,0,0,Qt::AlignCenter);
    gridlayout.addWidget(&old_password_edit,0,1,Qt::AlignCenter);
    gridlayout.addWidget(&new_password_lab1,1,0,Qt::AlignCenter);
    gridlayout.addWidget(&new_password_edit1,1,1,Qt::AlignCenter);
    gridlayout.addWidget(&new_password_lab2,2,0,Qt::AlignCenter);
    gridlayout.addWidget(&new_password_edit2,2,1,Qt::AlignCenter);

    QDialogButtonBox btnBox(&dlg);
    btnBox.addButton("确定",QDialogButtonBox::AcceptRole);
    btnBox.addButton("取消",QDialogButtonBox::RejectRole);
    connect(&btnBox,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
    connect(&btnBox,&QDialogButtonBox::rejected,&dlg,&QDialog::rejected);


    totalLayout.addLayout(&gridlayout);
    totalLayout.addWidget(&btnBox);

    if(dlg.exec()==QDialog::Accepted)
    {
        QString reader_no=ui->reader_no_edit->text();
        QString old_password=old_password_edit.text();
        QString new_password1=new_password_edit1.text();
        QString new_password2=new_password_edit2.text();
        if(old_password.isEmpty()||new_password1.isEmpty()||new_password2.isEmpty())
        {
            QMessageBox::information(this,"提示","请填写完整信息!");
            return;
        }
        if(new_password1!=new_password2)
        {
            QMessageBox::information(this,"提示","两次密码输入不一致!");
            return;
        }

        if(hashPassword!=encryptPassword(old_password))
        {
            QMessageBox::information(this,"修改密码","原密码错误!");
            return;
        }
        tempHashPwd=new_password1;
        QJsonObject obj;
        obj["type"]=MODEFYPASSWORD_REQUEST;
        obj["number"]=reader_no;
        obj["new_password"]=encryptPassword(new_password1);//加密

        QJsonDocument doc(obj);
        QByteArray sendData=doc.toJson(QJsonDocument::Compact);
        Net::getInstance()->sendData(sendData);
    }

}
void PersonalInformationWidget::onModefyPassword(int status)
{
    if(status==SUCCESS)
    {
        QMessageBox::information(this,"修改密码","修改密码成功!");
        hashPassword=tempHashPwd;
    }
    else
    {
        QMessageBox::information(this,"修改密码","修改密码失败!");
    }
}
QString PersonalInformationWidget::getReaderNo()const
{
    return ui->reader_no_edit->text();
}
QString PersonalInformationWidget::getReaderName()const
{
    return ui->name_edit->text();
}
int PersonalInformationWidget::getBorrow_points()const
{
    return ui->borrow_point_edit->text().toInt();
}
// 密码加密：明文 + 盐值 → SHA-256密文
QString PersonalInformationWidget::encryptPassword(const QString &plainPassword)
{
    QString mixStr = plainPassword + salt;
    QByteArray hashBytes = QCryptographicHash::hash(mixStr.toUtf8(), QCryptographicHash::Sha256);
    return hashBytes.toHex();
}
