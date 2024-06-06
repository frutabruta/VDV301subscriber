#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "..\..\cissubscriber.h"

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

private:
    Ui::MainWindow *ui;

    CisSubscriber cisSubscriber;

    void allConnects();

private slots:
    void slotDataReceived(QString data);


};
#endif // MAINWINDOW_H
