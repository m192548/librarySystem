#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QFrame>
#include "mainwindow.h"
#include <QButtonGroup>
#include "regeistwidget.h"
#include <QJsonObject>
namespace Ui {
class LoginWidget;
}

//登录窗口
class LoginWidget : public QFrame
{
    Q_OBJECT

public:
    explicit LoginWidget(QFrame *parent = nullptr);
    ~LoginWidget();
private:
    QString generateSalt();
    QString encryptPassword(const QString &plainPassword);
    bool verifyPassword(const QString &plainPassword, const QString &storedHash);
private slots:
    void on_login_btn_clicked();

    void on_exit_btn_clicked();

    void on_register_btn_clicked();

    void onLogin(const QJsonObject& info);
    void showWindow();

    void on_commonuser_rbtn_clicked(bool checked);

    void on_manager_rbtn_clicked(bool checked);

    void on_eye_btn_clicked(bool checked);

private:
    Ui::LoginWidget *ui;
    MainWindow* mainwindow;
    QButtonGroup* btnGroup;
    RegeistWidget regeistWidget;

};

#endif // LOGINWIDGET_H
