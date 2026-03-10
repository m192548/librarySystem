#include "statisticalanalysiswidget.h"
#include "ui_statisticalanalysiswidget.h"
#include <QRandomGenerator64>
#include "net.h"
#include "packet.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
StatisticalAnalysisWidget::StatisticalAnalysisWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatisticalAnalysisWidget)
{
    ui->setupUi(this);
    bookCountChart=new MyChart(MyChart::PIECHART,"图书库存统计图");
    borrowCountChart=new MyChart(MyChart::BARCHART,"图书借阅统计图");
    dateBorrowCountChart=new MyChart(MyChart::LINECHART,"图书借阅数量趋势图");
    dataIsLoaded=false;

    ui->oneMonth_btn->setChecked(true);

    connect(Net::getInstance(),&Net::getBookCount,this,&StatisticalAnalysisWidget::on_getBookCount);
    connect(Net::getInstance(),&Net::findBorrowCountByDate,this,&StatisticalAnalysisWidget::on_findBorrowCountByDate);
    connect(Net::getInstance(),&Net::getReaderBorrowCount,this,&StatisticalAnalysisWidget::on_getReaderBorrowCount);

    //设置折线图
    QChartView* view=dateBorrowCountChart->getChartView();
    QVBoxLayout* layout=(QVBoxLayout*)ui->date_borrow_count_widget->layout();
    layout->insertWidget(0,view);
}

StatisticalAnalysisWidget::~StatisticalAnalysisWidget()
{
    delete ui;
}
void StatisticalAnalysisWidget::showBookCountChart()
{
    bookCountChart->setElementsToChart();
    bookCountChart->setLegendFont(QFont("微软雅黑",10));

    QChartView* view=bookCountChart->getChartView();
    ui->stockChart_widget->layout()->addWidget(view);


}
void StatisticalAnalysisWidget::setBookStockElement(const QVector<QString>& categories,const QVector<int>& stocks)
{
    dataIsLoaded=true;
    for(int i=0;i<categories.size();i++)
    {
        qDebug()<<categories[i]<<","<<stocks[i];
        bookElements.insert(categories[i],stocks[i]);
        int r=QRandomGenerator64::global()->bounded(0,255);
        int g=QRandomGenerator64::global()->bounded(0,255);
        int b=QRandomGenerator64::global()->bounded(0,255);
        bookCountChart->addElements(categories[i],stocks[i],QColor(r,g,b));
    }
    showBookCountChart();
}
void StatisticalAnalysisWidget::setBorrowCount(const QVector<QString>& categories,const QVector<int>& stocks)
{
    int maxValue=*std::max_element(stocks.begin(),stocks.end());
    QStringList barSetLab{""};
    borrowCountChart->setBarSet(barSetLab);
    borrowCountChart->setAxisX("图书类型",categories.toList());
    borrowCountChart->setAxisY("借阅次数",0,int(maxValue+maxValue*0.3),10);
    borrowCountChart->setBarSetValues(0,stocks,QColor(41,48,255));

    QChartView* view=borrowCountChart->getChartView();
    ui->borrowChart_widget->layout()->addWidget(view);


}
void StatisticalAnalysisWidget::findBookCountByDate(int days)
{
    QJsonObject obj;
    obj["type"]=FINDBORROWCOUNTBYDATE_REQUEST;
    obj["days"]=days;
    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);
    Net::getInstance()->sendData(sendData);
}
void StatisticalAnalysisWidget::getBookCount(QString col_name,QString count_type)
{
    QJsonObject obj;
    obj["type"]=GETBOOKCOUNT_REQUEST;
    obj["col_name"]=col_name;
    obj["count_type"]=count_type;

    QJsonDocument doc(obj);
    QByteArray sendData=doc.toJson(QJsonDocument::Compact);
    Net::getInstance()->sendData(sendData);
}
void StatisticalAnalysisWidget::on_getBookCount(const QJsonObject& replyData)
{
    QJsonArray array=replyData["array"].toArray();
    QVector<QString> category_names;
    QVector<int> counts;
    QString count_name=replyData["count_name"].toString();
    for(int i=0;i<array.size();i++)
    {
        QJsonObject obj=array[i].toObject();
        category_names.append(obj["category"].toString());
        counts.append(obj["count"].toInt());
        qDebug()<<obj["category"].toString()<<","<<obj["count"].toInt();
    }

    if(count_name=="stock")
    {
        qDebug()<<"stock on_getBookCount";
        setBookStockElement(category_names,counts);
        getBookCount("category","total_borrowed_count");
    }
    else if(count_name=="total_borrowed_count")
    {
        qDebug()<<"borrow_count on_getBookCount size:"<<category_names.size()<<","<<counts;
        setBorrowCount(category_names,counts);
        findBookCountByDate(30);
    }
}
void StatisticalAnalysisWidget::on_findBorrowCountByDate(const QJsonObject& replyData)
{
    QJsonArray array=replyData["array"].toArray();
    QVector<QString> dates;
    QVector<int> counts;
    for(int i=0;i<array.size();i++)
    {
        QJsonObject obj=array[i].toObject();
        dates.append(obj["date"].toString("yyyy/MM/dd"));
        counts.append(obj["count"].toInt());
        qDebug()<<obj["date"].toString()<<","<<obj["count"].toInt();
    }
    int maxValue=*std::max_element(counts.begin(),counts.end());
    dateBorrowCountChart->setAxisX("日期",dates.toList(),counts);
    dateBorrowCountChart->setAxisY("借阅数量",0,maxValue,maxValue*0.2/30);


    //获取用户借阅数量
    getReaderBorrowCount();
}
void StatisticalAnalysisWidget::getReaderBorrowCount()
{
    QByteArray sendData=QString("{\"type\":%1}").arg(GETREADERBORROWCOUNT_REQUEST).toUtf8();
    Net::getInstance()->sendData(sendData);
}
void StatisticalAnalysisWidget::on_getReaderBorrowCount(const QJsonObject& replyData)
{
    QJsonArray array=replyData["array"].toArray();
    ui->tableWidget->setRowCount(array.size());
    int width=ui->tableWidget->width()/3;
    ui->tableWidget->setColumnWidth(0,width);
    ui->tableWidget->setColumnWidth(1,width);
    ui->tableWidget->setColumnWidth(2,width);
    for(int i=0;i<array.size();i++)
    {
        if(i>=100)break;
        QJsonObject obj=array[i].toObject();
        QTableWidgetItem* item1=ui->tableWidget->item(i,0);

        if(!item1)
        {
            item1=new QTableWidgetItem(QString::number(i+1));
            item1->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,0,item1);
        }
        else
        {
            item1->setText(QString::number(i+1));
        }
        QTableWidgetItem* item2=ui->tableWidget->item(i,1);
        if(!item2)
        {
            item2=new QTableWidgetItem(obj["reader_no"].toString());
            item2->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,1,item2);
        }
        else
        {
            item2->setText(obj["reader_no"].toString());
        }
        QTableWidgetItem* item3=ui->tableWidget->item(i,2);
        if(!item3)
        {
            item3=new QTableWidgetItem(obj["count"].toString());
            item3->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,2,item3);
        }
        else
        {
            item3->setText(obj["count"].toString());
        }
    }
}
void StatisticalAnalysisWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    int width=ui->tableWidget->width()/3;
    ui->tableWidget->setColumnWidth(0,width);
    ui->tableWidget->setColumnWidth(1,width);
    ui->tableWidget->setColumnWidth(2,width);
}

void StatisticalAnalysisWidget::on_oneMonth_btn_clicked(bool checked)
{
    if(!checked)
    {
        ui->oneMonth_btn->setChecked(true);
        return;
    }
    findBookCountByDate(30);
    ui->threeMonth_btn->setChecked(false);
    ui->sixMonth_btn->setChecked(false);
    ui->oneYear_btn->setChecked(false);
}


void StatisticalAnalysisWidget::on_threeMonth_btn_clicked(bool checked)
{
    if(!checked)
    {
        ui->threeMonth_btn->setChecked(true);
        return;
    }
    findBookCountByDate(90);
    ui->oneMonth_btn->setChecked(false);
    ui->sixMonth_btn->setChecked(false);
    ui->oneYear_btn->setChecked(false);
}


void StatisticalAnalysisWidget::on_sixMonth_btn_clicked(bool checked)
{
    if(!checked)
    {
        ui->sixMonth_btn->setChecked(true);
        return;
    }
    findBookCountByDate(180);
    ui->oneMonth_btn->setChecked(false);
    ui->threeMonth_btn->setChecked(false);
    ui->oneYear_btn->setChecked(false);
}


void StatisticalAnalysisWidget::on_oneYear_btn_clicked(bool checked)
{
    if(!checked)
    {
        ui->oneYear_btn->setChecked(true);
        return;
    }
    findBookCountByDate(365);
    ui->oneMonth_btn->setChecked(false);
    ui->threeMonth_btn->setChecked(false);
    ui->sixMonth_btn->setChecked(false);
}

