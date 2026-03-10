#ifndef MYCHART_H
#define MYCHART_H
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QHash>
#include <QList>
#include <QColor>
#include <QObject>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QLineSeries>
#include <QCategoryAxis>
//自定义图表类型
class MyChart:public QObject
{
    Q_OBJECT
public:
    enum ChartType
    {
        BARCHART,
        PIECHART,
        LINECHART
    };
    MyChart();
    MyChart(int type,const QString title);
    ~MyChart();
    void initBarChart();
    void initPieChart();
    void initLineChart();
    QChartView* getChartView();
    int getType()const;
    void setElementsToChart();
    void setChartTitle(QString title,QFont font=QFont("微软雅黑",14,QFont::Bold),QColor color=QColor(30,30,30));
    void setLegendFont(QFont font);         //设置图例字体

    //柱状图相关
    void setBarSet(const QStringList& barsetLabels);
    //设置x轴，当图表类型是柱状图(BARCHART)的时候省略第三个参数，当图表类型是折线图(LINECHART)的时候传入第三个参数
    void setAxisX(const QString title,QStringList labels,QVector<int> values=QVector<int>());
    void setBarSetValues(const int barSetIndex,const QVector<int>& values,const QColor color);
    void setAxisY(const QString title,int min,int max,int thickCount);



    //扇形图相关
    void setElements(QStringList& labels,QList<int>& counts,QList<QColor> colors=QList<QColor>());
    void addElements(const QString& label,const int& count,const QColor& color);
    void setElementValue(QString& label,int& value);



private:
    QChart* m_chart=nullptr;                //chart
    QChartView* m_chartView=nullptr;        //图表视图

    //扇形图相关
    QPieSeries* m_pieSeries=nullptr;        //

    //柱状图相关
    QVector<QBarSet*> m_barSets;
    QBarSeries *m_barSeries = nullptr;
    QStringList XTextLabel;//x轴分类标签
    QBarCategoryAxis *barAxisX=nullptr;//x轴
    QValueAxis *axisY=nullptr;//y轴
    int m_barSetCount;//柱子类型数量

    //折线图相关
    QLineSeries* m_lineSeries=nullptr;
    QCategoryAxis* lineAxisX=nullptr;



    QHash<QString,int> m_elements;  //标签及对应的数量
    QList<QColor> m_element_colors; //各个块的颜色
    QLegend* m_legend=nullptr;      //图例
    int m_type;

};

#endif // MYCHART_H
