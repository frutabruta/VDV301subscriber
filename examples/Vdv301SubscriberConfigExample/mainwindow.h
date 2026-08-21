#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

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
    explicit MainWindow(QString configurationFilePath, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSettings settings;

    CisSubscriber cisSubscriber;



    void allConnects();

    void loadConstants();
    void saveConstants();
    void applyConstantsFromFields();
    void popUpMessage(QString messageContent);
private slots:
    void slotDataReceived(QString data);


    void on_pushButton_save_clicked();
};
#endif // MAINWINDOW_H
