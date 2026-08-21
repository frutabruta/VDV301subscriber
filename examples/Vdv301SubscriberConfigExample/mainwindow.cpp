#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QString configurationFilePath,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settings(configurationFilePath, QSettings::IniFormat)
    , cisSubscriber("CustomerInformationService","AllData","2.3CZ1.0","_ibisip_http._tcp",48479)//puvodni port 48479, novy 59631

{
    ui->setupUi(this);

    loadConstants();
    applyConstantsFromFields();

    cisSubscriber.isSubscriptionActive=false ;
    cisSubscriber.start();
    cisSubscriber.newSubscribeRequest();

    allConnects();

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::allConnects()
{
    qDebug() <<  Q_FUNC_INFO;

    connect(&cisSubscriber, &IbisIpSubscriber::signalDataReceived  ,this, &MainWindow::slotDataReceived);
    //  connect(&cisSubscriber,&IbisIpSubscriber::signalUpdateDeviceList,this,&MainWindow::slotUpdateServiceTable);
    //  connect(&cisSubscriber.timerHeartbeatCheck,&QTimer::timeout ,this,&MainWindow::slotHeartbeatTimeout);
    //  connect(&cisSubscriber,&IbisIpSubscriber::signalSubscriptionLost ,this,&MainWindow::slotSubscriptionLost);
    //  connect(&cisSubscriber,&IbisIpSubscriberOnePublisher::signalSubscriptionSuccessful,this,&MainWindow::slotPublisherDoTabulky);

    /*

    connect(&deviceManagementService1_0,&DeviceManagementService::signalParametersChanged,this,&MainWindow::slotParametryZarizeniDoConfigu);
    connect(timerUpdateSeconds, &QTimer::timeout, this, &MainWindow::slotEverySecond);
    connect(timerLedSideCycleViaPoints, &QTimer::timeout, this, &MainWindow::ledIterujVsechnyPanely);
    connect(timerScrollingText, &QTimer::timeout, this, &MainWindow::slotMoveScrollingText);
    connect(timerLabelPageSwitch, &QTimer::timeout, this, &MainWindow::slotHlavniStridejStranky);
    connect(timerDelayedStart, &QTimer::timeout, this, &MainWindow::slotDelayedStartup);
    */




}


void MainWindow::loadConstants()
{
    ui->lineEdit_port->setText(settings.value("cisSubscriber/port").toString());
    ui->lineEdit_replyPath->setText(settings.value("cisSubscriber/replyPath").toString());
    ui->lineEdit_structure->setText(settings.value("cisSubscriber/structure").toString());
    ui->lineEdit_servicename->setText(settings.value("cisSubscriber/serviceName").toString());
    ui->lineEdit_version->setText(settings.value("cisSubscriber/version").toString());
    ui->lineEdit_serviceType->setText(settings.value("cisSubscriber/serviceType").toString());
}


void MainWindow::saveConstants()
{
    settings.setValue("cisSubscriber/port",ui->lineEdit_port->text().toUInt());
    settings.setValue("cisSubscriber/replyPath",ui->lineEdit_replyPath->text());
    settings.setValue("cisSubscriber/structure",ui->lineEdit_structure->text());
    settings.setValue("cisSubscriber/serviceName",ui->lineEdit_servicename->text());
    settings.setValue("cisSubscriber/version",ui->lineEdit_version->text());
    settings.setValue("cisSubscriber/serviceType",ui->lineEdit_serviceType->text());

}


void MainWindow::slotDataReceived(QString data)
{
    ui->plainTextEdit_receivedData->setPlainText(data);
}

void MainWindow::applyConstantsFromFields()
{
    cisSubscriber.setPortNumber(ui->lineEdit_port->text().toInt());
    cisSubscriber.setReplyPath(ui->lineEdit_replyPath->text());
    cisSubscriber.setVersion(ui->lineEdit_version->text());
    cisSubscriber.setServiceName(ui->lineEdit_servicename->text());
    cisSubscriber.setStructureName(ui->lineEdit_structure->text());
    cisSubscriber.setServiceType(ui->lineEdit_serviceType->text());
}

void MainWindow::on_pushButton_save_clicked()
{
    applyConstantsFromFields();
    saveConstants();

    popUpMessage("Setting saved, restart the program!");
}


void MainWindow::popUpMessage(QString messageContent)
{
    QMessageBox msgBox;
    msgBox.setText(messageContent);
    QFont font;
    //font.setBold(true);
    font.setPointSize(30);
    msgBox.setFont(font);
    // msgBox.setStyleSheet("font-size: 30px;");
    msgBox.exec();
}


