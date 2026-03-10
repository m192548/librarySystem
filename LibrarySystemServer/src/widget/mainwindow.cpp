#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include "packet.h"
#include "net.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->close_btn->setEnabled(false);
    connect(&net,&Net::clientCountChanged,this,&MainWindow::on_clientCountChanged);
    connect(&timer,&QTimer::timeout,this,[&]()
    {
        ui->datetime_lab->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    });
    connect(&net,&Net::recvMessage,this,[&](const QString& str)
    {
        ui->textEdit->append(str);
    });
    ui->address_edit->setText("127.0.0.1");
    ui->name_edit->setText("LIBRARY_SERVER");
    ui->port_edit->setText("1925");

    ui->port_edit->setReadOnly(true);
    ui->name_edit->setReadOnly(true);
    ui->address_edit->setReadOnly(true);
    ui->password_edit->setReadOnly(true);
    timer.start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_btn_clicked()
{
    QString portStr=ui->port_edit->text();
    if(portStr.isEmpty())
    {
        QMessageBox::information(this,"开启","请输入端口号!");
        return;
    }
    if(!net.openServer(QHostAddress::Any,portStr.toInt()))
    {
        QMessageBox::critical(this,"失败","开启服务器失败!");
        return;
    }
    QMessageBox::information(this,"成功","开启服务器成功!");
    ui->start_btn->setEnabled(false);
    ui->close_btn->setEnabled(true);
}


void MainWindow::on_close_btn_clicked()
{
    net.closeServer();
    ui->start_btn->setEnabled(true);
     ui->close_btn->setEnabled(false);

}

void MainWindow::on_clientCountChanged(int count)
{
    ui->clientCount_lab->setText(QString::number(count));
}

