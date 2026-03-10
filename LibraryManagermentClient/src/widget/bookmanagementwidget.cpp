#include "bookmanagementwidget.h"
#include "ui_bookmanagementwidget.h"
#include "net.h"
#include "packet.h"
#include <QStringList>
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
BookManagementWidget::BookManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookManagementWidget)
{
    ui->setupUi(this);

    connect(Net::getInstance(),&Net::booksLoaded,this,&BookManagementWidget::onBooksLoaded);
    connect(Net::getInstance(),&Net::deleteBooksStatusChanged,this,&BookManagementWidget::onBooksDeleted);
    connect(Net::getInstance(),&Net::modefyBooksStatusChanged,this,&BookManagementWidget::onBooksModefied);

    addWidget=nullptr;
    ui->delete_btn->setEnabled(false);
    ui->modefy_btn->setEnabled(false);
    //隐藏修改和删除按钮
    ui->delete_btn->setVisible(false);
    ui->modefy_btn->setVisible(false);

    booksIsLoaded=false;
    initTable();

}

BookManagementWidget::~BookManagementWidget()
{
    delete ui;
}
void BookManagementWidget::initTable()
{
    ui->tableWidget->setColumnCount(12);
    ui->tableWidget->setRowCount(100);
    currentPage=0;
    QStringList header;
    header<<"图书编号"<<"图书名称"<<"作者"<<"出版社"<<"出版日期"<<"类别"<<"库存总数"<<"可借数量"<<"借出数量"<<"借阅总次数"<<"状态"<<"操作";
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tableWidget->setColumnWidth(0,200);
    ui->tableWidget->setColumnWidth(1,200);
    ui->tableWidget->setColumnWidth(2,120);
    ui->tableWidget->setColumnWidth(3,200);
    ui->tableWidget->setColumnWidth(4,100);
    ui->tableWidget->setColumnWidth(5,80);
    ui->tableWidget->setColumnWidth(6,80);
    ui->tableWidget->setColumnWidth(7,80);
    ui->tableWidget->setColumnWidth(8,80);
    ui->tableWidget->setColumnWidth(9,80);
    ui->tableWidget->setColumnWidth(10,80);
    ui->tableWidget->setColumnWidth(11,60);

    int colCount=ui->tableWidget->columnCount();
    //设置复选框
    for(int i=0;i<100;i++)
    {
        QCheckBox* checkBox=new QCheckBox;
        checkBox->setObjectName("checkBox");
        checkBoxs.insert(i,checkBox);
        connect(checkBox,&QCheckBox::clicked,this,&BookManagementWidget::on_checkBox_clicked);
        QWidget* cellWdiget=new QWidget;
        QHBoxLayout* layout=new QHBoxLayout;
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        cellWdiget->setLayout(layout);

        ui->tableWidget->setCellWidget(i,colCount-1,cellWdiget);
    }


    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
}
//响应图书信息加载完成信号
void BookManagementWidget::onBooksLoaded(const QJsonObject& replyData)
{
    m_books.clear();
    bookCount=replyData["book_count"].toInt();
    qDebug()<<"book_count:"<<bookCount;
    totalPage=bookCount%100==0?bookCount/100-1:bookCount/100;
    ui->page_no_sbx->setRange(1,totalPage+1);
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
    ui->tableWidget->hideColumn(ui->tableWidget->columnCount()-1);
    showBooksToTable();

}
//加载图书信息
void BookManagementWidget::BookManagementWidget::loadBooksInfo(int count,int fromIndex)
{
    QString sendStr=QString("{\"type\":%1,\"count\":%2,\"from\":%3}").arg(LOADBOOK_REQUEST).arg(count).arg(fromIndex);
    QByteArray sendData=sendStr.toUtf8();
    Net::getInstance()->sendData(sendData);
    isShowAllBooks=true;
}
//显示图书信息到tableWidget上
void BookManagementWidget::BookManagementWidget::showBooksToTable()
{
    hideTableCheckBoxColmun();//隐藏checkBox列
    int colCount=ui->tableWidget->columnCount();
    int rowCount=ui->tableWidget->rowCount();
    for(int i=0;i<rowCount;i++)
    {
        ui->tableWidget->setRowHeight(i,50);
        for(int j=0;j<colCount-1;j++)
        {
            QTableWidgetItem* item=ui->tableWidget->item(i,j);
            if(!item)
            {
                item=new QTableWidgetItem;
                ui->tableWidget->setItem(i,j,item);
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags()&~Qt::ItemIsEditable);
            }
            if(i>=m_books.size())
            {
                ui->tableWidget->item(i,j)->setText("");
                continue;
            }
            if(j<colCount-2)
            {
                item->setText(m_books[i][j]);
            }
            else if(j==colCount-2)
            {
                item->setText(m_books[i][j].toInt()?"可借":"不可借");
            }

        }
    }
     ui->page_lab->setText(QString("%1/%2").arg(currentPage+1).arg(totalPage+1));
}
void BookManagementWidget::searchBook(const int count,const int from,const QString str)
{
    QJsonObject sendJson;
    sendJson["type"]=FINDBOOK_REQUEST;
    sendJson["str"]=search_str;
    sendJson["count"]=count;
    sendJson["from"]=from;
    isShowAllBooks=false;
    QJsonDocument doc(sendJson);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);

    Net::getInstance()->sendData(sendData);
}
void BookManagementWidget::on_add_btn_clicked(bool checked)
{
    if(checked)//选中状态显示添加图书窗口
    {
        if(!addWidget)
        {
            addWidget=new AddBookWidget;
            ui->main_hlayout->addWidget(addWidget);
            connect(addWidget,&AddBookWidget::addBookSucceed,this,&BookManagementWidget::onAddBook);
        }
    }
    else//未选中隐藏添加图书窗口
    {
        if(addWidget)
        {
            ui->main_hlayout->removeWidget(addWidget);
            addWidget->deleteLater();
            addWidget=nullptr;
        }
    }

}
void BookManagementWidget::onAddBook(const QVector<QString>& bookInfo)
{
    //如果正在显示所有图书，或者新增的图书符合当前的查询结果，则直接添加到tableWidget上
    if(isShowAllBooks||
        (search_str==bookInfo[0]||search_str==bookInfo[1]||search_str==bookInfo[2]||search_str==bookInfo[5]))
    {
        if(bookCount%100==0)//最后一页满
        {
            totalPage+=1;
            ui->page_lab->setText(QString("%1/%2").arg(currentPage+1).arg(totalPage+1));
            return;
        }
        if(currentPage!=totalPage)return;//如果当前不是最后一页则不显示新增图书
        int row=m_books.size()%100;
        int colCount=ui->tableWidget->columnCount();
        for(int j=0;j<colCount-1;j++)
        {
            QTableWidgetItem* item=ui->tableWidget->item(row,j);
            if(item)
            {
                if(j<colCount-2)
                {
                    item->setText(bookInfo[j]);
                }
                else if(j==colCount-2)
                {
                    item->setText(bookInfo[j].toInt()?"可借":"不可借");
                }
            }
        }
    }
    bookCount+=1;
}
void BookManagementWidget::on_checkBox_clicked(bool clicked)
{
    QCheckBox* checkBox=qobject_cast<QCheckBox*>(sender());
    if(!checkBox)return;
    int row=checkBoxs.key(checkBox);
    if(row<0||row>99)qDebug()<<"row 无效:"<<row;
    //遍历该行每列
    QVector<QString> checked_book;
    //先阻断table信号
     ui->tableWidget->blockSignals(true);
    for(int j=0;j<ui->tableWidget->columnCount()-1;j++)
    {
        QTableWidgetItem *item = ui->tableWidget->item(row,j);
        if(clicked)//选中
        {
            if(j>0)//选中行除编号其他一律可编辑
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            item->setForeground(QBrush(Qt::black));

            checked_book.append(ui->tableWidget->item(row,j)->text());

        }
        else//取消选中
        {
            if(!item)
            {
                qDebug()<<"item 无效";
                continue;
            }
           if(row<0||row>99)
            {
                qDebug()<<"row 无效:"<<row;
               continue;
           }
           if(j>0)
           item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
           checked_book.append(ui->tableWidget->item(row,j)->text());

        }

    }
    //恢复table信号
    ui->tableWidget->blockSignals(false);
    if(clicked)//选中则添加到选中图书中
    {
        //添加选中行
        if(checkedRows.indexOf(row)==-1)
        {
            checkedRows.append(row);
        }
        if(!checked_book.isEmpty())checkedBookInfos.append(checked_book);
    }
    else//取消选中则删除选中图书中的该图书
    {
        int index=checkedRows.indexOf(row);
        if(index!=-1)
         checkedRows.removeAt(index);
        if(checkedBookInfos.removeOne(checked_book))
        {
            qDebug()<<"checkedBookInfos 中已经移除";
        }
        else
        {
            qDebug()<<"checkedBookInfos 中移除失败!";
        }

    }
    //设置删除，修改按钮状态
    if(checkedBookInfos.size()>0)
    {
        ui->delete_btn->setEnabled(true);
        ui->modefy_btn->setEnabled(true);
    }
    else
    {
        ui->delete_btn->setEnabled(false);
        ui->modefy_btn->setEnabled(false);
    }

}
void BookManagementWidget::on_edit_btn_clicked(bool checked)
{
    //选中状态，显示复选框和删除、修改按钮
    if(checked)
    {
        showTableCheckBoxColmun();
    }
    else//未选中状态
    {
        hideTableCheckBoxColmun();
    }

}

void BookManagementWidget::onBooksDeleted(int status)
{
    if(status==SUCCESS)
    {
        QMessageBox::information(this,"删除","删除成功");
        //删除成功则重新加载当前页的图书信息
        if(currentPage>0&&currentPage==totalPage&&bookCount%100==1)
        {
            currentPage--;
        }
        loadBooksInfo(100,currentPage*100);
    }
    else
    {
        QMessageBox::information(this,"删除","删除失败");
    }
    hideTableCheckBoxColmun();

}
void BookManagementWidget::on_delete_btn_clicked()
{
    QMessageBox tipBox;
    tipBox.setWindowTitle("删除");
    tipBox.setText("确定删除所选数据？");
    tipBox.addButton("确定",QMessageBox::AcceptRole);
    tipBox.addButton("取消",QMessageBox::RejectRole);
    if(QMessageBox::AcceptRole != tipBox.exec())return;
    QJsonArray book_nosArray;
    for(int i=0;i<checkedBookInfos.size();i++)
    {
        book_nosArray.append(checkedBookInfos[i][0]);
    }
    QJsonObject sendObj;
    sendObj["type"]=DELETEBOOKS_REQUEST;
    sendObj["book_nos"]=book_nosArray;
    QJsonDocument doc(sendObj);
    Net::getInstance()->sendData(doc.toJson(QJsonDocument::Compact));
}


void BookManagementWidget::on_modefy_btn_clicked()
{
    QJsonArray booksArray;
    for(int i=0;i<checkedRows.size();i++)
    {
        QJsonObject book;
        book["book_no"]=ui->tableWidget->item(checkedRows[i],0)->text();
        book["book_name"]=ui->tableWidget->item(checkedRows[i],1)->text();
        book["author"]=ui->tableWidget->item(checkedRows[i],2)->text();
        book["publisher"]=ui->tableWidget->item(checkedRows[i],3)->text();
        book["publish_date"]=ui->tableWidget->item(checkedRows[i],4)->text();
        book["category"]=ui->tableWidget->item(checkedRows[i],5)->text();
        book["stock"]=ui->tableWidget->item(checkedRows[i],6)->text().toInt();
        book["available_quantity"]=ui->tableWidget->item(checkedRows[i],7)->text().toInt();
        book["current_borrowed_count"]=ui->tableWidget->item(checkedRows[i],8)->text().toInt();
        book["total_borrowed_count"]=ui->tableWidget->item(checkedRows[i],9)->text().toInt();
        book["status"]=ui->tableWidget->item(checkedRows[i],10)->text();

        booksArray.append(book);
    }
    QJsonObject obj;
    obj["type"]=MODEFYBOOKS_REQUEST;
    obj["array"]=booksArray;
    QJsonDocument doc(obj);
    Net::getInstance()->sendData(doc.toJson(QJsonDocument::Compact));
}
void BookManagementWidget::onBooksModefied(const QJsonObject& replyData)
{
    int status=replyData["res"].toInt();
    if(status==SUCCESS)
    {
        QMessageBox::information(this,"修改","修改成功");
    }
    else
    {
        QString errorStr="修改失败:";
        QJsonArray array=replyData["book_nos"].toArray();
        for(int i=0;i<array.size();i++)
        {
            errorStr+=array[i].toString()+" ";
        }
        QMessageBox::information(this,"修改",errorStr);
    }
    hideTableCheckBoxColmun();

}
void BookManagementWidget::showTableCheckBoxColmun()
{
    ui->tableWidget->showColumn(ui->tableWidget->columnCount()-1);
    if(m_books.size()<ui->tableWidget->rowCount())
    {
        for(int i=m_books.size();i<checkBoxs.size();i++)
        {
            checkBoxs[i]->setVisible(false);
        }
    }
    else
    {
        for(int i=0;i<checkBoxs.size();i++)
        {
            checkBoxs[i]->setVisible(true);
        }
    }
    //隐藏修改和删除按钮
    ui->delete_btn->setVisible(true);
    ui->modefy_btn->setVisible(true);


    ui->delete_btn->setEnabled(false);
    ui->modefy_btn->setEnabled(false);
}
void BookManagementWidget::hideTableCheckBoxColmun()
{
    ui->edit_btn->setChecked(false);
    for(int i=0;i<checkedRows.size();i++)
    {
        checkBoxs[checkedRows[i]]->setChecked(false);
        //设置该行单元格不编辑
        for(int j=1;j<ui->tableWidget->columnCount()-1;j++)
        {
            QTableWidgetItem *item = ui->tableWidget->item(checkedRows[i],j);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
    }
    ui->delete_btn->setVisible(false);
    ui->modefy_btn->setVisible(false);

    checkedBookInfos.clear();
    checkedRows.clear();

    ui->tableWidget->hideColumn(ui->tableWidget->columnCount()-1);
}
void BookManagementWidget::on_search_btn_clicked()
{
    search_str=ui->search_edit->text();
    if(search_str.isEmpty())//str为空则显示所有图书
    {
        loadBooksInfo(100,0);
        isShowAllBooks=true;
        currentPage=0;
    }
    else
    {
        searchBook(100,0,search_str);
        currentPage=0;
    }


}
void BookManagementWidget::clearItemsAndCellWidgets()
{
    int row=ui->tableWidget->rowCount();
    int col=ui->tableWidget->columnCount();
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<col;j++)
        {
            if(j<col-1)//清除items
            {
                QTableWidgetItem* item=ui->tableWidget->item(i,j);
                if(item)
                {
                    delete item;
                    item=nullptr;
                }
            }
            else//清除控件
            {
                QWidget* cellWidget=ui->tableWidget->cellWidget(i,j);
                if(cellWidget)
                {
                    QLayout* layout=cellWidget->layout();
                    if(layout)
                    {
                        //删除容器里的checkBox
                        checkBoxs[i]->deleteLater();
                        checkBoxs[i]=nullptr;
                        checkBoxs.remove(i);
                        //移除中心窗口
                        ui->tableWidget->removeCellWidget(i,j);
                        //释放内存
                        layout->deleteLater();
                        layout=nullptr;
                        cellWidget->deleteLater();
                        cellWidget=nullptr;

                    }
                }
            }

        }
    }
    checkBoxs.clear();
    checkedRows.clear();

}

void BookManagementWidget::on_last_page_btn_clicked()
{
    if(currentPage==0)return;
    hideTableCheckBoxColmun();
    currentPage--;
    if(isShowAllBooks)
    loadBooksInfo(100,currentPage*100);
    else searchBook(100,currentPage*100,search_str);

}


void BookManagementWidget::on_next_page_btn_clicked()
{
    if(currentPage==totalPage)return;
    currentPage++;
    hideTableCheckBoxColmun();
    int from=currentPage*100;
    if(isShowAllBooks)
        loadBooksInfo(100,from);
    else searchBook(100,from,search_str);
}


void BookManagementWidget::on_go_btn_clicked()
{
    currentPage=ui->page_no_sbx->value()-1;
    if(isShowAllBooks)
    loadBooksInfo(100,currentPage*100);
    else searchBook(100,currentPage*100,search_str);
}

