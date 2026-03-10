#include "adminwidget.h"
#include "ui_adminwidget.h"
AdminWidget::AdminWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminWidget)
{
    ui->setupUi(this);
    initUi();
    initQss();
}

AdminWidget::~AdminWidget()
{
    delete ui;
}
void AdminWidget::initUi()
{
    ui->borrowmanage_btn->setChecked(true);
    ui->borrowmanage_btn->setIcon(QIcon(":/icon/borrowManagement2.png"));
    ui->bookmanage_btn->setIcon(QIcon(":/icon/bookManagement1.png"));
    ui->statistical_btn->setIcon(QIcon(":/icon/statistical1.png"));
    ui->personalinfo_btn->setIcon(QIcon(":/icon/personalInfo1.png"));

    borrowMWidget=new BorrowManagementWidget(this);
    bookMWidget=new BookManagementWidget(this);
    saWidget=new StatisticalAnalysisWidget(this);
    piWidget=new PersonalInformationWidget(this);

    ui->stackedWidget->addWidget(borrowMWidget);
    ui->stackedWidget->addWidget(bookMWidget);
    ui->stackedWidget->addWidget(saWidget);
    ui->stackedWidget->addWidget(piWidget);

    //默认是借阅管理界面
    ui->stackedWidget->setCurrentWidget(borrowMWidget);
}
void AdminWidget::initData()
{
    borrowMWidget->loadBorrowInfo();
    borrowMWidget->borrowRecordIsLoaded=true;
}
void AdminWidget::initQss()
{

}
void AdminWidget::on_borrowmanage_btn_clicked(bool checked)
{
    if(!checked)
    {
        ui->borrowmanage_btn->setChecked(true);
        return;
    }
    ui->borrowmanage_btn->setIcon(QIcon(":/icon/borrowManagement2.png"));
    ui->bookmanage_btn->setIcon(QIcon(":/icon/bookManagement1.png"));
    ui->statistical_btn->setIcon(QIcon(":/icon/statistical1.png"));
    ui->personalinfo_btn->setIcon(QIcon(":/icon/personalInfo1.png"));

    ui->bookmanage_btn->setChecked(false);
    ui->statistical_btn->setChecked(false);
    ui->personalinfo_btn->setChecked(false);

    ui->stackedWidget->setCurrentWidget(borrowMWidget);
    if(!borrowMWidget->borrowRecordIsLoaded)
    {
        borrowMWidget->loadBorrowInfo();
        borrowMWidget->borrowRecordIsLoaded=true;
    }
}


void AdminWidget::on_bookmanage_btn_clicked(bool checked)
{
    if(!checked)
    {
        ui->bookmanage_btn->setChecked(true);
        return;
    }
    ui->bookmanage_btn->setIcon(QIcon(":/icon/bookManagement2.png"));
    ui->borrowmanage_btn->setIcon(QIcon(":/icon/borrowManagement1.png"));
    ui->statistical_btn->setIcon(QIcon(":/icon/statistical1.png"));
    ui->personalinfo_btn->setIcon(QIcon(":/icon/personalInfo1.png"));
    ui->borrowmanage_btn->setChecked(false);
    ui->statistical_btn->setChecked(false);
    ui->personalinfo_btn->setChecked(false);

    ui->stackedWidget->setCurrentWidget(bookMWidget);
    if(!bookMWidget->booksIsLoaded)
    {
        bookMWidget->loadBooksInfo(100,0);
        bookMWidget->booksIsLoaded=true;
    }
}

void AdminWidget::on_statistical_btn_clicked(bool checked)
{
    if(!checked)
    {
        ui->statistical_btn->setChecked(true);
        return;
    }
    ui->statistical_btn->setIcon(QIcon(":/icon/statistical2.png"));
    ui->bookmanage_btn->setIcon(QIcon(":/icon/bookManagement1.png"));
    ui->borrowmanage_btn->setIcon(QIcon(":/icon/borrowManagement1.png"));
    ui->personalinfo_btn->setIcon(QIcon(":/icon/personalInfo1.png"));

    ui->borrowmanage_btn->setChecked(false);
    ui->bookmanage_btn->setChecked(false);
    ui->personalinfo_btn->setChecked(false);

    ui->stackedWidget->setCurrentWidget(saWidget);
    if(!saWidget->dataIsLoaded)
    {
        saWidget->getBookCount("category","stock");
        saWidget->dataIsLoaded=true;
    }
}
void AdminWidget::on_personalinfo_btn_clicked(bool checked)
{
    if(!checked)
    {
        ui->personalinfo_btn->setChecked(true);
        return;
    }
    ui->statistical_btn->setIcon(QIcon(":/icon/statistical1.png"));
    ui->bookmanage_btn->setIcon(QIcon(":/icon/bookManagement1.png"));
    ui->borrowmanage_btn->setIcon(QIcon(":/icon/borrowManagement1.png"));
    ui->personalinfo_btn->setIcon(QIcon(":/icon/personalInfo2.png"));

    ui->borrowmanage_btn->setChecked(false);
    ui->statistical_btn->setChecked(false);
    ui->bookmanage_btn->setChecked(false);

    ui->stackedWidget->setCurrentWidget(piWidget);


}
