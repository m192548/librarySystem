#ifndef PERSONALINFORMATIONWIDGET_H
#define PERSONALINFORMATIONWIDGET_H

#include <QWidget>
#include <QJsonObject>
namespace Ui {
class PersonalInformationWidget;
}

class PersonalInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PersonalInformationWidget(QWidget *parent = nullptr);
    ~PersonalInformationWidget();
    QString getReaderNo()const;
    QString getReaderName()const;
    int getBorrow_points()const;
private:
    QString encryptPassword(const QString &plainPassword);

private slots:
    void onLoadPersonalInfomation(const QJsonObject& info);
    void on_edit_info_btn_clicked(bool checked);
    void on_save_btn_clicked();
    void onModefyPersonalInfo(int status);
    void onModefyPassword(int status);

    void on_modefy_password_btn_clicked();

private:
    Ui::PersonalInformationWidget *ui;
    QString hashPassword;//当前用户密文密码
    QString tempHashPwd;//临时存储密码
};

#endif // PERSONALINFORMATIONWIDGET_H
