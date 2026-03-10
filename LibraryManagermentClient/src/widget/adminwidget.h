#ifndef ADMINWIDGET_H
#define ADMINWIDGET_H

#include <QWidget>
#include "borrowmanagementwidget.h"
#include "bookmanagementwidget.h"
#include "statisticalanalysiswidget.h"
#include "personalinformationwidget.h"
#include <QJsonObject>
namespace Ui {
class AdminWidget;
}
class AdminWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWidget(QWidget *parent = nullptr);
    ~AdminWidget();
    void initUi();
    void initData();
    void initQss();

public:
    void setPersonInfomation(const QJsonObject& info);
private slots:
    void on_borrowmanage_btn_clicked(bool checked);

    void on_bookmanage_btn_clicked(bool checked);

    void on_statistical_btn_clicked(bool checked);

    void on_personalinfo_btn_clicked(bool checked);

private:
    Ui::AdminWidget *ui;
    BorrowManagementWidget* borrowMWidget;      //借阅管理界面
    BookManagementWidget* bookMWidget;          //图书管理界面
    StatisticalAnalysisWidget* saWidget;        //统计分析界面
    PersonalInformationWidget* piWidget;        //个人信息界面

};

#endif // ADMINWIDGET_H
