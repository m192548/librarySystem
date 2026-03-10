#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "net.h"
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_close_btn_clicked();

    void on_start_btn_clicked();

    void on_clientCountChanged(int count);

private:
    Ui::MainWindow *ui;
    Net net;
    QTimer timer;

};
#endif // MAINWINDOW_H
