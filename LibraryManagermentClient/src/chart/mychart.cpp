#include "mychart.h"
#include <QLegendMarker>
#include <QToolTip>
MyChart::MyChart()
{
    m_chart=new QChart;
    m_chart->setAcceptHoverEvents(true);
    m_chartView=new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing); // 抗锯齿，
    //移除默认背景色，使得图表背景透明
    m_chart->setBackgroundVisible(false);
    m_chartView->setStyleSheet("background-color:white;");
    m_chart->createDefaultAxes(); // 创建默认轴（后续替换Y轴）
}
MyChart::MyChart(int type,const QString title)
:MyChart()
{
    m_type=type;
    switch(m_type)
    {
    case BARCHART:
    {
        initBarChart();
    }
        break;
    case PIECHART:
    {
        initPieChart();
    }
        break;
    case LINECHART:
    {
        initLineChart();
    }
        break;
    }
    m_chart->setTitle(title);
    m_chart->setTitleFont(QFont("微软雅黑",12));
    m_chart->setTitleBrush(Qt::black);
}
MyChart::~MyChart()
{


}
int MyChart::getType()const
{
    return m_type;
}
void MyChart::initBarChart()
{
    barAxisX=new QBarCategoryAxis;
    m_barSeries = new QBarSeries();
    axisY = new QValueAxis();
    axisY->setLabelFormat("%d");
    // 设置柱子间距（可选）
    m_barSeries->setBarWidth(0.6);
    m_chart->addSeries(m_barSeries);
    m_chart->setAnimationOptions(QChart::SeriesAnimations); // 显示动画（可选）
    m_chart->legend()->setAlignment(Qt::AlignTop); // 图例居上


}
void MyChart::initPieChart()
{
    m_pieSeries=new QPieSeries;
}
void MyChart::initLineChart()
{
    m_lineSeries=new QLineSeries;
    m_lineSeries->setColor(QColor(255,201,14));
    lineAxisX=new QCategoryAxis;
    axisY = new QValueAxis();

    m_chart->addSeries(m_lineSeries);
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignTop); // 图例居上

    lineAxisX->setGridLineVisible(false);
    lineAxisX->setLabelsFont(QFont("微软雅黑",8));
    // 让标签居中显示（可选，默认左对齐）
    lineAxisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    m_chart->setAxisX(lineAxisX, m_lineSeries); // 绑定X轴

    axisY->setLabelFormat("%d");
    axisY->setGridLineVisible(false);
}
//柱状图相关
void MyChart::setBarSet(const QStringList& barsetLabels)
{
    for(int i=0;i<barsetLabels.size();i++)
    {
        QBarSet* set=new QBarSet(barsetLabels[i]);
        m_barSets.append(set);
        m_barSets[i]->setSelectedColor(QColor(181,255,14));
    }
    for(int i=0;i<m_barSets.size();i++)
    {
        m_barSeries->append(m_barSets[i]);
    }
}
void MyChart::setAxisX(const QString title,QStringList labels,QVector<int> values)
{
    if(m_type==BARCHART)
    {
        barAxisX->append(labels);
        barAxisX->setTitleText(title);
        QObject::connect(m_barSeries, &QBarSeries::hovered, [this,labels](bool status, int index, QBarSet *set) {
            if (status) { // 鼠标进入柱子
                if (index >= 0 && index < labels.size()) {
                    // 获取当前柱子的标签和数值
                    QString label = labels[index];    // X轴标签（如“周三”）
                    int value = set->at(index);        // 柱子对应的数值（如15）
                    // 构造提示文本
                    QString tipText = QString("%1:%2").arg(label).arg(value);
                    // 在鼠标位置显示提示框
                    QToolTip::showText(QCursor::pos(), tipText, m_chartView);
                }
            } else { // 鼠标离开柱子
                QToolTip::hideText(); // 隐藏提示框
            }
        });
        m_chart->addAxis(barAxisX, Qt::AlignBottom); // X轴居下
        barAxisX->setGridLineVisible(false);
        m_barSeries->attachAxis(barAxisX); // 系列绑定X轴
    }
    else if(m_type==LINECHART)
    {
        //清空原数据
        QStringList oldLabels = lineAxisX->categoriesLabels();
        for (const QString &label : oldLabels) {
            lineAxisX->remove(label); // 移除指定标签（连带绑定的数值）
        }
        m_lineSeries->clear();

        //鼠标悬停显示内容
        QObject::connect(m_lineSeries, &QLineSeries::hovered, [this,labels](const QPointF &point, bool state) {
            if (state) { // 鼠标悬停在数据点上
                // 数据点的X值是整数索引，匹配对应的X轴标签
                int xIndex = qRound(point.x());
                if (xIndex >= 0 && xIndex < labels.size()) {
                    QString xLabel = labels[xIndex];
                    int yValue = qRound(point.y()); // Y轴整数数值
                    // 构造提示文本，在鼠标位置显示
                    QString tipText = QString("%1: %2").arg(xLabel).arg(yValue);
                    QToolTip::showText(QCursor::pos(), tipText, m_chartView);
                }
            } else { // 鼠标离开数据点
                QToolTip::hideText();
            }
        });

        //添加点和标签
        for(int i=0;i<labels.size();i++)
        {
            if(i==0||i==labels.size()/2||i==labels.size()-1)
            {
                lineAxisX->append(labels[i],i);
            }
            m_lineSeries->append(i, values[i]);
        }

        lineAxisX->setTitleText(title);
        // 设置X轴范围（从0到标签数量-1，确保所有标签显示）
        lineAxisX->setRange(0, labels.size() - 1);


    }

}
void MyChart::setBarSetValues(const int barSetIndex,const QVector<int>& values,const QColor color)
{
    for(int i=0;i<values.size();i++)
    (*m_barSets[barSetIndex])<<values[i];
    (*m_barSets[barSetIndex]).setColor(color);
}
void MyChart::setAxisY(const QString title,int min,int max,int thickCount)
{
    axisY->setRange(min, max); // Y轴数值范围
    axisY->setTitleText(title); // Y轴标题
    axisY->setTickCount(thickCount); // Y轴刻度数量（包含首尾）
    if(m_type==BARCHART)
    {
         axisY->setTickType(QValueAxis::TicksFixed); // 固定刻度类型
        m_chart->setAxisY(axisY); // Y轴居左
        m_barSeries->attachAxis(axisY); // 系列绑定Y轴
        //m_barSeries->setLabelsFormat("%d");
    }
    else if(m_type==LINECHART)
    {
        if(!m_chart->axisY())
        m_chart->setAxisY(axisY, m_lineSeries);

    }


}
//扇形图相关
void MyChart::setElements(QStringList& labels,QList<int>& counts,QList<QColor> colors)
{
    for(int i=0;i<labels.size();i++)
    {
        m_elements.insert(labels[i],counts[i]);
        m_element_colors.append(colors);
    }
}
void MyChart::setElementsToChart()
{
    QHash<QString,int>::iterator it=m_elements.begin();
    //计算总数量
    int total_count=0;
    for(;it!=m_elements.end();it++)
    {
        total_count+=it.value();
    }
    //设置每个切片信息
    it=m_elements.begin();
    int i=0;
    for(;it!=m_elements.end()||i<m_element_colors.size();it++)
    {
        float res=float(it.value())/float(total_count)*100.0;
        QString s=QString(" %1").arg(res,0,'f',1);
        QPieSlice* slice=m_pieSeries->append(it.key(),it.value());
        QString str=it.key()+s+"%";
        slice->setLabel(str);
        slice->setLabelArmLengthFactor(0.2);//连接线长度
        slice->setLabelPosition(QPieSlice::LabelOutside);//标签在外部
        slice->setColor(m_element_colors[i++]);
        slice->setLabelColor(Qt::black);
        slice->setLabelFont(QFont("微软雅黑",6,QFont::Bold));
        slice->setLabelVisible(true);
        slice->setExplodeDistanceFactor(0.1);
        //点击时分离或合并
        QObject::connect(slice,&QPieSlice::clicked,slice,[slice](){
            slice->setExploded(!slice->isExploded());
        });
        //悬浮时分离且显示切片信息
        QObject::connect(slice,&QPieSlice::hovered,slice,[slice](bool state){
        if(state)
        {
            slice->setExploded(true);
        }
        else
        {
            slice->setExploded(false);
        }
        });
    }
    m_chart->addSeries(m_pieSeries);

    //配置图例
    m_legend=m_chart->legend();
    m_legend->setVisible(true);
    m_legend->setAlignment(Qt::AlignRight);
    m_legend->setZValue(3);
    //设置图例文本
    it=m_elements.begin();
    i=0;
    QList<QLegendMarker*>marker=m_chart->legend()->markers();
    qDebug()<<"marker size:"<<marker;
    for(;it!=m_elements.end();it++)
    {
        if(marker[i])
        {
            marker[i++]->setLabel(it.key());
        }
    }
}
void MyChart::addElements(const QString& label,const int& count,const QColor& color)
{
    m_elements.insert(label,count);
    m_element_colors.append(color);
}
void MyChart::setElementValue(QString& label,int& value)
{
    m_elements[label]=value;
}

void MyChart::setLegendFont(QFont font)
{
    m_chart->legend()->setFont(font);
}
QChartView* MyChart::getChartView()
{
    return m_chartView;
}
