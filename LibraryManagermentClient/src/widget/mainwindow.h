#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "adminwidget.h"
#include "userwidget.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString permisson,QWidget *parent = nullptr);
    ~MainWindow();
    void initData();

private:
    Ui::MainWindow *ui;
    AdminWidget* adminWidget;
    UserWidget* userWidget;
};
#endif // MAINWINDOW_H
