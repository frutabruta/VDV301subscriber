#ifndef DEVMGMTSUBSCRIBER_H
#define DEVMGMTSUBSCRIBER_H
#include "ibisipsubscriber.h"
#include "devmgmtpublisherstruct.h"

class DevMgmtSubscriber : public IbisIpSubscriber
{
    Q_OBJECT
public:

    //constructor
    explicit DevMgmtSubscriber(QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber);

    ~DevMgmtSubscriber();
    //library instances
    //QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkAccessManager manager ;
    //promenne
    int mPortNumber=0;
    QString mServiceName="";
    QString mStructureName="";
    QString mServiceType="";
    QString mVersion="";

    QVector<DevMgmtPublisherStruct> deviceListDetected;
    QVector<DevMgmtPublisherStruct> deviceListConfigured;


    //functions
    bool getDeviceConfiguration(QZeroConfService zcs);
    bool getDeviceInformation(QZeroConfService zcs);

    QString getVersion(QDomDocument document, QString element);
    bool getDeviceInformation(DevMgmtPublisherStruct &device);
    bool getDeviceConfiguration(DevMgmtPublisherStruct &device);
private:




    void allConnects();
public slots:

    void slotNewDnsSd(QZeroConfService zcs);
    void slotRemoveDnsSd(QZeroConfService zcs);
    void slotHandleData(QString input);
    QByteArray slotRequestReceived(QNetworkReply *reply);
    void slotUpdateDeviceInfo();
private slots:


signals:

    void downloadFinished(QString vystup);


};

#endif // DEVMGMTSUBSCRIBER_H
