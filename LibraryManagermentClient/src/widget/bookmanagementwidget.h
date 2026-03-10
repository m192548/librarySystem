#ifndef BOOKMANAGEMENTWIDGET_H
#define BOOKMANAGEMENTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include "addbookwidget.h"
#include <QHash>
#include <QCheckBox>
#include <QJsonArray>
namespace Ui {
class BookManagementWidget;
}

class BookManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookManagementWidget(QWidget *parent = nullptr);
    ~BookManagementWidget();
    void loadBooksInfo(int count,int fromIndex);          //从fromIndex加载count条图书信息
    void showBooksToTable();       //显示图书信息
public:
    bool booksIsLoaded;
private:
    void hideTableCheckBoxColmun();         //隐藏tableWidget操作列
    void showTableCheckBoxColmun();         //显示tableWidget操作列
    void clearItemsAndCellWidgets();   //清空items和控件
    void initTable();
    void searchBook(const int count,const int from,const QString str);
public slots:
    void onBooksLoaded(const QJsonObject& replyData);
    void onAddBook(const QVector<QString>& bookInfo);
    void onBooksDeleted(int status);
    void onBooksModefied(const QJsonObject& replyData);
private slots:
    void on_add_btn_clicked(bool checked);

    void on_checkBox_clicked(bool clicked);


    void on_edit_btn_clicked(bool checked);
    void on_delete_btn_clicked();


    void on_modefy_btn_clicked();

    void on_search_btn_clicked();

    void on_last_page_btn_clicked();

    void on_next_page_btn_clicked();

    void on_go_btn_clicked();

private:
    Ui::BookManagementWidget *ui;
    QVector<QVector<QString>> m_books;
    AddBookWidget* addWidget;
    QHash<int,QCheckBox*> checkBoxs;
    QVector<QVector<QString>> checkedBookInfos;
    QVector<int> checkedRows;

    QString search_str;
    bool isShowAllBooks;        //标记当前是否正在显示所有图书
    int currentPage;
    int totalPage;
    int bookCount;

};

#endif // BOOKMANAGEMENTWIDGET_H
