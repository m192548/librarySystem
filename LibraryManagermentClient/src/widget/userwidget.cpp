#include "userwidget.h"
#include "ui_userwidget.h"
#include <QLayout>
#include "net.h"
#include "packet.h"
#include <QMessageBox>
#include <QIcon>
#include <QDateTimeEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
UserWidget::UserWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserWidget)
{
    ui->setupUi(this);
    piWidget=nullptr;
    bookIsloaded=false;;
    borrowRecordLoaded=false;
    initPersonalInfoWidget();
    initMyBorrowWidget();
    initBorrowWidget();

    //默认先显示图书借阅界面
    ui->tabWidget->setCurrentIndex(0);

}

UserWidget::~UserWidget()
{
    delete ui;
}
void UserWidget::initBorrowWidget()
{
    ui->borrow_tableWidget->setColumnCount(13);
    ui->borrow_tableWidget->setRowCount(100);
    currentPage_book=0;
    QStringList header;
    header<<"序号"<<"图书编号"<<"图书名称"<<"作者"<<"出版社"<<"出版日期"<<"类别"<<"库存总数"<<"可借数量"<<"借出数量"<<"借阅总次数"<<"状态"<<"操作";
    ui->borrow_tableWidget->setHorizontalHeaderLabels(header);

    ui->borrow_tableWidget->setColumnWidth(0,50);
    ui->borrow_tableWidget->setColumnWidth(1,200);
    ui->borrow_tableWidget->setColumnWidth(2,200);
    ui->borrow_tableWidget->setColumnWidth(3,120);
    ui->borrow_tableWidget->setColumnWidth(4,200);
    ui->borrow_tableWidget->setColumnWidth(5,100);
    ui->borrow_tableWidget->setColumnWidth(6,80);
    ui->borrow_tableWidget->setColumnWidth(7,80);
    ui->borrow_tableWidget->setColumnWidth(8,80);
    ui->borrow_tableWidget->setColumnWidth(9,80);
    ui->borrow_tableWidget->setColumnWidth(10,80);
    ui->borrow_tableWidget->setColumnWidth(11,80);
    ui->borrow_tableWidget->setColumnWidth(12,100);

    int colCount=ui->borrow_tableWidget->columnCount();
    //添加借阅按钮
    for(int i=0;i<100;i++)
    {
        QPushButton* borrow_btn=new QPushButton("借阅");
        borrow_btn->setObjectName("borrow_btn");
        borrow_btn->setFixedSize(80,30);
        borrowBtns.insert(i,borrow_btn);
        connect(borrow_btn,&QPushButton::clicked,this,[this,i]()
        {
                    borrowBook(i);
        });
        QWidget* cellWdiget=new QWidget;
        QHBoxLayout* layout=new QHBoxLayout;
        layout->addWidget(borrow_btn);
        layout->setAlignment(Qt::AlignCenter);
        cellWdiget->setLayout(layout);

        ui->borrow_tableWidget->setCellWidget(i,colCount-1,cellWdiget);
    }


    connect(Net::getInstance(),&Net::borrowBooksStatusChanged,this,&UserWidget::on_borrowBooks);
    connect(Net::getInstance(),&Net::booksLoaded,this,&UserWidget::onBooksLoaded);
    connect(Net::getInstance(),&Net::updateData,this,&UserWidget::updateBooks);

}
void UserWidget::initMyBorrowWidget()
{
    ui->category2_cbx->addItem("全部");
    ui->myBorrow_tableWidget->setColumnCount(10);
    QStringList header{"序号","图书编号","图书名称","读者编号","读者姓名","借阅时间","归还时间","预计归还时间","扣除积分","罚款金额"};
    ui->myBorrow_tableWidget->setColumnWidth(0,50);
    ui->myBorrow_tableWidget->setColumnWidth(1,200);
    ui->myBorrow_tableWidget->setColumnWidth(2,200);
    ui->myBorrow_tableWidget->setColumnWidth(3,120);
    ui->myBorrow_tableWidget->setColumnWidth(4,100);
    ui->myBorrow_tableWidget->setColumnWidth(5,200);
    ui->myBorrow_tableWidget->setColumnWidth(6,200);
    ui->myBorrow_tableWidget->setColumnWidth(7,200);
    ui->myBorrow_tableWidget->setColumnWidth(8,80);
    ui->myBorrow_tableWidget->setColumnWidth(9,80);
    ui->myBorrow_tableWidget->setHorizontalHeaderLabels(header);
    connect(Net::getInstance(),&Net::borrowRecordSearched,this,&UserWidget::onBorrowInfoLoaded);
}
void UserWidget::initPersonalInfoWidget()
{
    if(!piWidget)
    {
        piWidget=new PersonalInformationWidget(this);
    }
    QLayout* layout=ui->personalInfo_widget->layout();
    if(!layout)
    {
        qDebug()<<"personal widget layout is null!";
        return;
    }
    layout->addWidget(piWidget);
}
//响应图书信息加载完成信号
void UserWidget::onBooksLoaded(const QJsonObject& replyData)
{
    m_books.clear();
    bookCount=replyData["book_count"].toInt();
    qDebug()<<"book_count:"<<bookCount;
    totalPage_book=bookCount%100==0?bookCount/100-1:bookCount/100;
    ui->page_no_sbx->setRange(1,totalPage_book+1);
    QJsonArray array=replyData["array"].toArray();
    foreach(const QJsonValue& value, array)
    {
        if (value.isObject())
        {
            QJsonObject bookObj = value.toObject();
            QString no = bookObj["book_no"].toString();
            QString name = bookObj["book_name"].toString();
            QString author = bookObj["author"].toString();
            QString publisher = bookObj["publisher"].toString();
            QString publish_date = bookObj["publish_date"].toString();
            QString category = bookObj["category"].toString();
            QString stock = bookObj["stock"].toString();
            QString available_quantity=bookObj["available_quantity"].toString();
            QString current_borrowed_count = bookObj["current_borrowed_count"].toString();
            QString total_borrowed_count = bookObj["total_borrowed_count"].toString();
            QString status = bookObj["status"].toString();

            QVector<QString> book;
            QStringList list{no,name,author,publisher,publish_date,category,stock,available_quantity,current_borrowed_count,total_borrowed_count,status};
            book.append(list);
            m_books.append(book);
        }

    }
    qDebug()<<"本次加载的图书数量:"<<m_books.size();
    showBooksToTable();

}
//加载图书信息
void UserWidget::UserWidget::loadBooksInfo(int count,int from)
{
    isShowAllBooks=true;
    QString sendStr=QString("{\"type\":%1,\"count\":%2,\"from\":%3}")
                          .arg(LOADBOOK_REQUEST).arg(count).arg(from);
    QByteArray sendData=sendStr.toUtf8();
    Net::getInstance()->sendData(sendData);
}
//显示图书信息到tableWidget上
void UserWidget::showBooksToTable()
{
    int colCount=ui->borrow_tableWidget->columnCount();
    int rowCount=ui->borrow_tableWidget->rowCount();
    for(int i=0;i<rowCount;i++)
    {
        ui->borrow_tableWidget->setRowHeight(i,50);
        for(int j=0;j<colCount;j++)
        {
            if(j==colCount-1)//操作列
            {
                if(i>=m_books.size())
                {
                    borrowBtns[i]->setVisible(false);
                }
                else
                {
                     borrowBtns[i]->setVisible(true);
                    if(m_books[i][10].toInt()==0)//该图书不可借
                    {
                        borrowBtns[i]->setEnabled(false);
                    }
                    else
                    {
                        borrowBtns[i]->setEnabled(true);
                    }
                }
                break;
            }
            //非操作列
            QTableWidgetItem* item=ui->borrow_tableWidget->item(i,j);
            if(!item)
            {
                item=new QTableWidgetItem;
                ui->borrow_tableWidget->setItem(i,j,item);
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags()&~Qt::ItemIsEditable);
            }
            //在图书信息以下的空行
            if(i>=m_books.size())
            {
                ui->borrow_tableWidget->item(i,j)->setText("");
                continue;
            }
            //最后两列前面的列(0-10)
            if(j<colCount-2)
            {
                if(j==0)//第0列
                {
                    item->setText(QString::number(i+currentPage_book*100+1));
                }
                else//1~10列
                {
                    item->setText(m_books[i][j-1]);
                }

            }
            else if(j==colCount-2)//第11列
            {
                item->setText(m_books[i][j-1].toInt()?"可借":"不可借");
            }

        }
    }
    ui->page_lab->setText(QString("%1/%2").arg(currentPage_book+1).arg(totalPage_book+1));
}
void UserWidget::borrowBook(int row)
{

    QDialog dlg;
    dlg.setFixedSize(200,200);
    QDialogButtonBox btnBox;
    btnBox.addButton("确定",QDialogButtonBox::AcceptRole);
    btnBox.addButton("取消",QDialogButtonBox::RejectRole);
    connect(&btnBox,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
    connect(&btnBox,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);
    QLabel label("请输入归还时间");
    label.setFixedHeight(30);
    QDateTimeEdit edit(QDateTime::currentDateTime());
    edit.setFixedHeight(30);
    QVBoxLayout layout;
    layout.addWidget(&label);
    layout.addWidget(&edit);
    layout.addWidget(&btnBox);

    dlg.setLayout(&layout);

    if(dlg.exec()!=QDialog::Accepted)return;
    QString book_no=ui->borrow_tableWidget->item(row,1)->text();
    QString book_name=ui->borrow_tableWidget->item(row,2)->text();
    QString reader_no=piWidget->getReaderNo();
    QString reader_name=piWidget->getReaderName();
    int borrow_points=piWidget->getBorrow_points();
    QDateTime dueDateTime=edit.dateTime();//预计归还时间
    QDateTime currentDateTime=QDateTime::currentDateTime();

    //计算借阅时长
    qint64 secondsDiff = currentDateTime.secsTo(dueDateTime);
    double hoursDiff = static_cast<double>(secondsDiff) / 3600.0;
    if(qRound(hoursDiff)<24)
    {
        QMessageBox::information(this,"借阅","借阅失败，至少借阅24小时!");
        return;
    }
    if(borrow_points<=0)
    {
        QMessageBox::information(this,"借阅","借阅失败，信誉积分不足!");
        return;
    }
    QJsonObject obj;
    obj["book_no"]=book_no;
    obj["book_name"]=book_name;
    obj["reader_no"]=reader_no;
    obj["reader_name"]=reader_name;
    obj["borrow_time"]=currentDateTime.toString("yyyy-MM-dd HH:mm:ss");
    obj["type"]=BORROWBOOK_REQUEST;
    obj["due_time"]=dueDateTime.toString("yyyy-MM-dd HH:mm:ss");

    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);

    Net::getInstance()->sendData(sendData);
    borrowBookRow=row;


}
void UserWidget::loadMyBorrowInfo()
{
    isShowAllBooks=true;
    QString reader_no=piWidget->getReaderNo();
    QJsonObject obj;
    obj["type"]=SEARCHBORROWRECORD_REQUEST;
    obj["reader_no"]=reader_no;
    obj["book_no"]="全部";
    obj["book_type"]="全部";
    obj["return_status"]="全部";
    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);
    Net::getInstance()->sendData(sendData);
}
void UserWidget::searchBooks(const int count,const int from)
{
    isShowAllBooks=false;
    QJsonObject obj;
    obj["type"]=FINDBOOK_REQUEST;
    obj["count"]=count;
    obj["from"]=from;
    obj["book_name"]=search_book_name;
    obj["author"]=search_book_author;
    obj["category"]=search_book_category;

    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);
    Net::getInstance()->sendData(sendData);

}
void UserWidget::on_tabWidget_currentChanged(int index)
{
    qDebug()<<"on_tabWidget_currentChanged";
    if(index==0)
    {
        if(bookIsloaded==false)
        {
            loadBooksInfo(100,0);
        }
    }
    else if(index==1)
    {
        if(borrowRecordLoaded==false)
        {
            loadMyBorrowInfo();
        }
    }

}
void UserWidget::on_borrowBooks(const QJsonObject& replyData)
{
    int res=replyData["res"].toInt();
    qDebug()<<"res:"<<res;
    if(res==SUCCESS)
    {
        QMessageBox::information(this,"借阅","借书成功!");
        //刷新当前图书界面信息
        loadBooksInfo(100,currentPage_book);
        int row=ui->myBorrow_tableWidget->rowCount();
        ui->myBorrow_tableWidget->insertRow(row);
        ui->myBorrow_tableWidget->setItem(row,0,new QTableWidgetItem(replyData["book_no"].toString()));
        ui->myBorrow_tableWidget->setItem(row,1,new QTableWidgetItem(replyData["book_name"].toString()));
        ui->myBorrow_tableWidget->setItem(row,2,new QTableWidgetItem(replyData["reader_no"].toString()));
        ui->myBorrow_tableWidget->setItem(row,3,new QTableWidgetItem(replyData["reader_name"].toString()));
        ui->myBorrow_tableWidget->setItem(row,4,new QTableWidgetItem(replyData["borrow_time"].toString()));
        ui->myBorrow_tableWidget->setItem(row,5,new QTableWidgetItem(replyData["return_time"].toString()));
        ui->myBorrow_tableWidget->setItem(row,6,new QTableWidgetItem(replyData["due_time"].toString()));
        ui->myBorrow_tableWidget->setItem(row,7,new QTableWidgetItem(QString::number(replyData["deducted_Points"].toInt())));
        ui->myBorrow_tableWidget->setItem(row,8,new QTableWidgetItem(QString::number(replyData["fine"].toInt())));


    }
    else if(res == BORROWREPEATE_ERROR)
    {
        QMessageBox::information(this,"借阅","借书失败,您已经借阅了该书籍!");
    }
    else
    {
        QMessageBox::information(this,"借阅","借书失败!");
    }
}
void UserWidget::onBorrowInfoLoaded(const QVector<QVector<QString>>& records)
{
    borrowRecordLoaded=true;
    destroyMyBorrowTableItems();
    qDebug()<<"records size:"<<records.size();
    ui->myBorrow_tableWidget->setRowCount(records.size());
    for(int i=0;i<records.size();i++)
    {
        for(int j=0;j<records[i].size();j++)
        {
            if(j==0)
            {
                QTableWidgetItem* item=new QTableWidgetItem(QString::number(i+1));
                item->setTextAlignment(Qt::AlignCenter);
                ui->myBorrow_tableWidget->setItem(i,j,item);
            }
            else
            {
                QTableWidgetItem* item=new QTableWidgetItem(records[i][j-1]);
                item->setTextAlignment(Qt::AlignCenter);
                ui->myBorrow_tableWidget->setItem(i,j,item);
            }

        }
        ui->myBorrow_tableWidget->setRowHeight(i,50);
    }
    ui->myBorrow_tableWidget->hideColumn(3);
    ui->myBorrow_tableWidget->hideColumn(4);
}

void UserWidget::destroyMyBorrowTableItems()
{
    ui->myBorrow_tableWidget->setUpdatesEnabled(false);
    int row=ui->myBorrow_tableWidget->rowCount();
    int col=ui->myBorrow_tableWidget->columnCount();
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<col;j++)
        {
            QTableWidgetItem* item=ui->myBorrow_tableWidget->item(i,j);
            if(item)
            {
                delete item;
                item=nullptr;
            }
        }
    }
    ui->myBorrow_tableWidget->setUpdatesEnabled(true);
}


void UserWidget::on_findBorrow_btn_clicked()
{
    QString reader_no=piWidget->getReaderNo();
    QString book_name=ui->book_name2_edit->text();
    QString category=ui->category2_cbx->currentText();
    QString return_status=ui->isReturn_cbx->currentText();
    QString borrowDateRange=ui->borrowDateRange_cbx->currentText();
    if(book_name.isEmpty()&&category=="全部"&&return_status=="全部"&&borrowDateRange=="全部")
    {
        loadMyBorrowInfo();
        return;
    }
    QJsonObject obj;
    obj["type"]=FINDMYBORROWINFO_REQUEST;
    obj["reader_no"]=reader_no;
    obj["book_name"]=book_name;
    obj["category"]=category;
    obj["is_return"]=return_status;
    obj["borrowDateRange"]=borrowDateRange;

    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);
    Net::getInstance()->sendData(sendData);

}


void UserWidget::on_findBook_btn_clicked()
{
    //未设置查找条件则加载全部图书
    if(search_book_name.isEmpty()&&search_book_author.isEmpty()&&search_book_category=="全部")
    {
        currentPage_book=0;
        loadBooksInfo(100,0);
        return;
    }
    currentPage_book=0;
    searchBooks(100,0);
}

void UserWidget::destroyBookTableItems()
{
    ui->borrow_tableWidget->setUpdatesEnabled(false);
    int row=ui->borrow_tableWidget->rowCount();
    int col=ui->borrow_tableWidget->columnCount();
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<col;j++)
        {
            if(j==col-1)
            {
                //销毁按钮
                QWidget* cellWidget=ui->borrow_tableWidget->cellWidget(i,j);
                QLayout* layout=cellWidget->layout();
                layout->removeWidget(borrowBtns[i]);
                borrowBtns[i]->deleteLater();
                borrowBtns[i]=nullptr;
                if(layout)
                {
                    layout->deleteLater();
                    layout=nullptr;
                }
                if(cellWidget)
                {
                    cellWidget->deleteLater();
                    cellWidget=nullptr;
                }
                continue;
            }
            //销毁items
            QTableWidgetItem* item=ui->borrow_tableWidget->item(i,j);
            if(item)
            {
                delete item;
                item=nullptr;
            }
        }
    }
    borrowBtns.clear();
    ui->borrow_tableWidget->setUpdatesEnabled(true);
}
void UserWidget::on_last_page_btn_clicked()
{
    if(currentPage_book==0)return;
    currentPage_book--;
    if(isShowAllBooks)
        loadBooksInfo(100,currentPage_book*100);
    else searchBooks(100,currentPage_book*100);

}


void UserWidget::on_next_page_btn_clicked()
{
    if(currentPage_book==totalPage_book)return;
    currentPage_book++;
    int from=currentPage_book*100;
    if(isShowAllBooks)
        loadBooksInfo(100,from);
    else searchBooks(100,from);
}


void UserWidget::on_go_btn_clicked()
{
    currentPage_book=ui->page_no_sbx->value()-1;
    if(isShowAllBooks)
        loadBooksInfo(100,currentPage_book*100);
    else searchBooks(100,currentPage_book*100);
}


void UserWidget::on_book_name1_edit_textChanged(const QString &arg1)
{
    search_book_name=arg1;
}


void UserWidget::on_author_edit_textChanged(const QString &arg1)
{
    search_book_author=arg1;
}


void UserWidget::on_category_cbx_currentTextChanged(const QString &arg1)
{
    search_book_category=arg1;
}

void UserWidget::updateBooks()
{

    if(isShowAllBooks)
    {
        loadBooksInfo(100,currentPage_book*100);
    }
    else
    {
        searchBooks(100,currentPage_book*100);
    }
}
