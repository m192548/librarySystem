#ifndef ADDBOOKWIDGET_H
#define ADDBOOKWIDGET_H

#include <QWidget>

namespace Ui {
class AddBookWidget;
}

class AddBookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddBookWidget(QWidget *parent = nullptr);
    ~AddBookWidget();
signals:
    void addBookSucceed(const QVector<QString>& books);
private slots:
    void on_ok_btn_clicked();

    void on_addbook_reply(int status);

private:
    Ui::AddBookWidget *ui;
};

#endif // ADDBOOKWIDGET_H
