#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>
#include "personalinformationwidget.h"
#include <QVector>
#include <QPushButton>
#include <QHash>
#include <QJsonObject>
namespace Ui {
class UserWidget;
}

class UserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserWidget(QWidget *parent = nullptr);
    ~UserWidget();
    void loadBooksInfo(int count,int from);          //加载图书信息
private slots:
    void onBooksLoaded(const QJsonObject& replyData);
    void on_tabWidget_currentChanged(int index);
    void on_borrowBooks(const QJsonObject& replyData);
    void onBorrowInfoLoaded(const QVector<QVector<QString>>& records);
    void updateBooks();

    void on_findBorrow_btn_clicked();

    void on_findBook_btn_clicked();

    void on_last_page_btn_clicked();

    void on_next_page_btn_clicked();

    void on_go_btn_clicked();

    void on_book_name1_edit_textChanged(const QString &arg1);

    void on_author_edit_textChanged(const QString &arg1);

    void on_category_cbx_currentTextChanged(const QString &arg1);

private:
    void initBorrowWidget();
    void initMyBorrowWidget();
    void initPersonalInfoWidget();
    void searchBooks(const int count,const int from);
    void showBooksToTable();       //显示图书信息
    void borrowBook(int row);
    void loadMyBorrowInfo();
    void destroyMyBorrowTableItems();
    void destroyBookTableItems();
private:
    bool bookIsloaded;
    bool borrowRecordLoaded;
    Ui::UserWidget *ui;
    PersonalInformationWidget* piWidget;
    QVector<QVector<QString>> m_books;
    QHash<int,QPushButton*>borrowBtns;

    int borrowBookRow;
    int currentPage_book;
    int totalPage_book;
    int bookCount;
    QString search_book_name,search_book_category,search_book_author;
    bool isShowAllBooks;

};

#endif // USERWIDGET_H
