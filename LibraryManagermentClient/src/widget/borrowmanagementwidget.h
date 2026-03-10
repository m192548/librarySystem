#ifndef BORROWMANAGEMENTWIDGET_H
#define BORROWMANAGEMENTWIDGET_H

#include <QWidget>

namespace Ui {
class BorrowManagementWidget;
}

class BorrowManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BorrowManagementWidget(QWidget *parent = nullptr);
    ~BorrowManagementWidget();
    void loadBorrowInfo();
public:
    bool borrowRecordIsLoaded;
private slots:
    void on_ok_btn_clicked();
    void on_borrowBooks(const QJsonObject& replyData);
    void on_returnBooks(const QJsonObject& replyData);

    void on_type_cbx_currentTextChanged(const QString &arg1);

    void on_search_btn_clicked();

private:
    void onBorrowInfoLoaded(const QVector<QVector<QString>>& records);
    void destroyItems();


private:
    Ui::BorrowManagementWidget *ui;
};

#endif // BORROWMANAGEMENTWIDGET_H
