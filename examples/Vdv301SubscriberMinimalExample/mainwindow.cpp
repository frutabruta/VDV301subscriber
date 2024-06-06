#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cisSubscriber("CustomerInformationService","AllData","2.2CZ1.0","_ibisip_http._tcp",48479)//puvodni port 48479, novy 59631

{
    ui->setupUi(this);

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

void MainWindow::slotDataReceived(QString data)
{
    ui->plainTextEdit_receivedData->setPlainText(data);
}
