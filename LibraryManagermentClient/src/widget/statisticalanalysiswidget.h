#ifndef STATISTICALANALYSISWIDGET_H
#define STATISTICALANALYSISWIDGET_H

#include <QWidget>
#include "mychart.h"
#include <QHash>
#include <QJsonObject>
#include <QResizeEvent>
namespace Ui {
class StatisticalAnalysisWidget;
}

class StatisticalAnalysisWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticalAnalysisWidget(QWidget *parent = nullptr);
    ~StatisticalAnalysisWidget();
    void showBookCountChart();
    void getBookCount(QString col_name,QString count_type);
    void setBookStockElement(const QVector<QString>& categories,const QVector<int>& stocks);
    void setBorrowCount(const QVector<QString>& categories,const QVector<int>& stocks);
    void findBookCountByDate(int days);
    void getReaderBorrowCount();
public:
    bool dataIsLoaded;
protected:
    void resizeEvent(QResizeEvent* event)override;
private slots:

    void on_getBookCount(const QJsonObject& replyData);
    void on_findBorrowCountByDate(const QJsonObject& replyData);
    void on_getReaderBorrowCount(const QJsonObject& replyData);

    void on_oneMonth_btn_clicked(bool checked);

    void on_threeMonth_btn_clicked(bool checked);

    void on_sixMonth_btn_clicked(bool checked);

    void on_oneYear_btn_clicked(bool checked);

private:
    Ui::StatisticalAnalysisWidget *ui;
    MyChart* bookCountChart=nullptr;
    MyChart* borrowCountChart=nullptr;
    MyChart* dateBorrowCountChart=nullptr;
    QHash<QString,int> bookElements;
};

#endif // STATISTICALANALYSISWIDGET_H
