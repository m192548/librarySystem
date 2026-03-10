#ifndef REGEISTWIDGET_H
#define REGEISTWIDGET_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class RegeistWidget;
}

class RegeistWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegeistWidget(QWidget *parent = nullptr);
    ~RegeistWidget();
private:
    QString encryptPassword(const QString &plainPassword);
signals:
    void gotoLogin();

private slots:
    void on_regeist_btn_clicked();

    void on_exit_btn_clicked();

    void on_password_edit2_textChanged(const QString &arg1);

    void on_regeisStatusChanged(int status);

    void on_login_btn_clicked();

    void on_password_edit1_textChanged(const QString &arg1);

private:
    Ui::RegeistWidget *ui;
};

#endif // REGEISTWIDGET_H
