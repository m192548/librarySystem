#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QIcon>
#include <QLayout>
MainWindow::MainWindow(const QString permission,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    adminWidget=nullptr;
    userWidget=nullptr;
    setWindowIcon(QIcon(":/icon/main.ico"));
    QLayout* layout=ui->centralwidget->layout();
    if(!layout)
    {
        qDebug()<<"layout is null!";
        return;
    }
    if(permission=="admin")
    {

        adminWidget=new AdminWidget(this);
        layout->addWidget(adminWidget);
    }
    else
    {
        userWidget=new UserWidget(this);
        layout->addWidget(userWidget);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initData()
{
    if(userWidget)
    {
        userWidget->loadBooksInfo(100,0);
    }
    if(adminWidget)
    {
        adminWidget->initData();
    }
}
