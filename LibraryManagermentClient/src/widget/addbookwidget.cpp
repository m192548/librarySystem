#include "addbookwidget.h"
#include "ui_addbookwidget.h"
#include "net.h"
#include "packet.h"
#include <QMessageBox>
AddBookWidget::AddBookWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddBookWidget)
{
    ui->setupUi(this);
    connect(Net::getInstance(),&Net::addbookStatusChanged,this,&AddBookWidget::on_addbook_reply);
}

AddBookWidget::~AddBookWidget()
{
    delete ui;
}

void AddBookWidget::on_ok_btn_clicked()
{
    QString number=ui->book_no_edit->text();
    QString name=ui->book_name_edit->text();
    QString author=ui->author_edit->text();
    QString publisher=ui->publisher_edit->text();
    QString publishDate=ui->publishDate_edit->date().toString("yyyy-MM-dd");
    QString category=ui->category_edit->currentText();
    int stock=ui->stock_count_sbox->value();
    int current_count=ui->avaliable_count_sbox->value();
    int status=ui->status_cbx->currentText()=="可借"?1:0;

    if(number.isEmpty()||name.isEmpty()||author.isEmpty()||publisher.isEmpty())
    {
        QMessageBox::information(this,"失败","请填写完整信息!");
        return;
    }
    QString sendStr=QString("{"
                      "\"type\":%1,"
                      "\"book_no\":\"%2\","
                      "\"book_name\":\"%3\","
                      "\"author\":\"%4\","
                      "\"publisher\":\"%5\","
                      "\"publish_date\":\"%6\","
                      "\"category\":\"%7\","
                      "\"stock\":%8,"
                      "\"current_count\":%9,"
                              "\"status\":%10}")
                          .arg(ADDBOOK_REQUEST).arg(number).arg(name).arg(author).arg(publisher)
                          .arg(publishDate).arg(category).arg(stock).arg(current_count)
                          .arg(status);
    Net::getInstance()->sendData(sendStr.toUtf8());

}

void AddBookWidget::on_addbook_reply(int status)
{
    if(status==SUCCESS)
    {
        QString number=ui->book_no_edit->text();
        QString name=ui->book_name_edit->text();
        QString author=ui->author_edit->text();
        QString publisher=ui->publisher_edit->text();
        QString publishDate=ui->publishDate_edit->date().toString("yyyy-MM-dd");
        QString category=ui->category_edit->currentText();
        int stock=ui->stock_count_sbox->value();
        int avaliable_count=ui->avaliable_count_sbox->value();
        int current_borrowed_count=ui->borrow_count_sbox->value();
        int total_borrowed_count=ui->total_borrowed_count_sbox->value();
        QString status=ui->status_cbx->currentText();

        QVector<QString> bookInfo;
        bookInfo.append(number);
        bookInfo.append(name);
        bookInfo.append(author);
        bookInfo.append(publisher);
        bookInfo.append(publishDate);
        bookInfo.append(category);
        bookInfo.append(QString::number(stock));
        bookInfo.append(QString::number(avaliable_count));
        bookInfo.append(QString::number(current_borrowed_count));
        bookInfo.append(QString::number(total_borrowed_count));
        bookInfo.append(status);

        emit addBookSucceed(bookInfo);

        QMessageBox::information(this,"添加图书","添加图书成功");


    }
    else if(status==BOOKNOREPEATE_ERROR)
    {
        QMessageBox::information(this,"添加图书","添加图书失败,图书编号重复！");
    }
    else
    {
        QMessageBox::information(this,"添加图书","添加图书失败！");
    }
}

