#include "borrowmanagementwidget.h"
#include "ui_borrowmanagementwidget.h"
#include "net.h"
#include "packet.h"
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
BorrowManagementWidget::BorrowManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BorrowManagementWidget)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(9);
    QStringList header{"图书编号","图书名称","读者编号","读者姓名","借阅时间","归还时间","预计归还时间","扣除积分","罚款金额"};
    ui->tableWidget->setColumnWidth(0,200);
    ui->tableWidget->setColumnWidth(1,200);
    ui->tableWidget->setColumnWidth(2,120);
    ui->tableWidget->setColumnWidth(3,100);
    ui->tableWidget->setColumnWidth(4,200);
    ui->tableWidget->setColumnWidth(5,200);
    ui->tableWidget->setColumnWidth(6,200);
    ui->tableWidget->setColumnWidth(7,80);
    ui->tableWidget->setColumnWidth(8,80);
    ui->tableWidget->setHorizontalHeaderLabels(header);
    borrowRecordIsLoaded=false;
    connect(Net::getInstance(),&Net::borrowInfoLoaded,this,&BorrowManagementWidget::onBorrowInfoLoaded);
    connect(Net::getInstance(),&Net::borrowBooksStatusChanged,this,&BorrowManagementWidget::on_borrowBooks);
    connect(Net::getInstance(),&Net::returnBooksStatusChanged,this,&BorrowManagementWidget::on_returnBooks);
    connect(Net::getInstance(),&Net::borrowRecordSearched,this,&BorrowManagementWidget::onBorrowInfoLoaded);
}

BorrowManagementWidget::~BorrowManagementWidget()
{
    delete ui;
}
void BorrowManagementWidget::loadBorrowInfo()
{
    QString sendStr=QString("{\"type\":%1}").arg(LOADBORROWINFO_REQUEST);
    QByteArray send_data=sendStr.toUtf8();
    Net::getInstance()->sendData(send_data);
}
void BorrowManagementWidget::onBorrowInfoLoaded(const QVector<QVector<QString>>& records)
{
    destroyItems();
    qDebug()<<records.size();
    ui->tableWidget->setRowCount(records.size());
    for(int i=0;i<records.size();i++)
    {
        for(int j=0;j<records[i].size();j++)
        {
            QTableWidgetItem* item=new QTableWidgetItem(records[i][j]);
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,j,item);
        }
    }  
}

void BorrowManagementWidget::on_ok_btn_clicked()
{
    QString type=ui->type_cbx->currentText();
    QString book_no=ui->bookno_edit->text();
    QString book_name=ui->bookname_edit->text();
    QString reader_no=ui->readerNo_edit->text();
    QString reader_name=ui->readerName_edit->text();
    QString return_dateTime=ui->returndateTime_edit->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    if(type.isEmpty()||book_no.isEmpty()||reader_no.isEmpty()
        ||reader_name.isEmpty()||return_dateTime.isEmpty())
    {
        QMessageBox::information(this,"操作失败","请填写完整信息");
        return;
    }

    QDateTime current_dateTime=QDateTime::currentDateTime();
    QJsonObject obj;
    obj["book_no"]=book_no;
    obj["book_name"]=book_name;
    obj["reader_no"]=reader_no;
    obj["reader_name"]=reader_name;
    if(ui->type_cbx->currentText()=="借阅")
    {
        obj["type"]=BORROWBOOK_REQUEST;
        obj["borrow_time"]=current_dateTime.toString("yyyy-MM-dd HH:mm:ss");
        obj["due_time"]=return_dateTime;
    }
    else
    {
        obj["type"]=RETURNBOOK_REQUEST;
        obj["return_time"]=current_dateTime.toString("yyyy-MM-dd HH:mm:ss");
    }

    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);

    Net::getInstance()->sendData(sendData);
}

void BorrowManagementWidget::on_borrowBooks(const QJsonObject& replyData)
{
    int res=replyData["res"].toInt();
    if(res==SUCCESS)
    {
        QMessageBox::information(this,"借阅","办理成功!");
        int row=ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0,new QTableWidgetItem(replyData["book_no"].toString()));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(replyData["book_name"].toString()));
        ui->tableWidget->setItem(row,2,new QTableWidgetItem(replyData["reader_no"].toString()));
        ui->tableWidget->setItem(row,3,new QTableWidgetItem(replyData["reader_name"].toString()));
        ui->tableWidget->setItem(row,4,new QTableWidgetItem(replyData["borrow_time"].toString()));
        ui->tableWidget->setItem(row,5,new QTableWidgetItem(replyData["return_time"].toString()));
        ui->tableWidget->setItem(row,6,new QTableWidgetItem(replyData["due_time"].toString()));
        ui->tableWidget->setItem(row,7,new QTableWidgetItem(QString::number(replyData["deducted_Points"].toInt())));
        ui->tableWidget->setItem(row,8,new QTableWidgetItem(QString::number(replyData["fine"].toInt())));

    }
    else if(res == BORROWREPEATE_ERROR)
    {
        QMessageBox::information(this,"借阅","办理失败,该用户已经借阅了该书籍!");
    }
    else if(res==BOOKNO_NOT_EXIT_ERROR)
    {
        QMessageBox::information(this,"借阅","办理失败,图书编号不存在!");
    }
    else if(res==READERNO_NOT_EXIT_ERROR)
    {
        QMessageBox::information(this,"借阅","办理失败,读者编号不存在!");
    }
    else
    {
        QMessageBox::information(this,"借阅","办理失败!");
    }
}

void BorrowManagementWidget::on_type_cbx_currentTextChanged(const QString &arg1)
{
    if(arg1=="借阅")
    {
        ui->returnDateTime_lab->setVisible(true);
        ui->returndateTime_edit->setVisible(true);
    }
    else
    {
        ui->returnDateTime_lab->setVisible(false);
        ui->returndateTime_edit->setVisible(false);
    }
}
void BorrowManagementWidget::on_returnBooks(const QJsonObject& replyData)
{
    if(replyData["res"].toInt()==SUCCESS)
    {
        QMessageBox::information(this,"归还","归还成功!");
    }
    else
    {
         QMessageBox::information(this,"归还","归还失败!");
    }
}


void BorrowManagementWidget::on_search_btn_clicked()
{
    QString book_no=ui->book_no_edit->text();
    QString reader_no=ui->reader_no_edit->text();
    QString book_type=ui->book_type_cbx->currentText();
    QString return_status=ui->isReturn_cbx->currentText();

    if(book_no.isEmpty()&&reader_no.isEmpty()&&book_type=="全部"&&return_status=="全部")
    {
        loadBorrowInfo();
        return;
    }
    QJsonObject obj;
    obj["type"]=SEARCHBORROWRECORD_REQUEST;
    obj["book_no"]=book_no;
    obj["reader_no"]=reader_no;
    obj["book_type"]=book_type;
    obj["return_status"]=return_status;

    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);
    Net::getInstance()->sendData(sendData);
}

 void BorrowManagementWidget::destroyItems()
{
    ui->tableWidget->setUpdatesEnabled(false);
    int row=ui->tableWidget->rowCount();
    int col=ui->tableWidget->columnCount();
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<col;j++)
        {
            QTableWidgetItem* item=ui->tableWidget->item(i,j);
            if(item)
            {
                delete item;
                item=nullptr;
            }
        }
    }
    ui->tableWidget->setUpdatesEnabled(true);
}
