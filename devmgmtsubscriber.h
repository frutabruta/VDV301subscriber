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

    QNetworkAccessManager manager ;

    //promenne
    QVector<DevMgmtPublisherStruct> deviceListDetected;
    QVector<DevMgmtPublisherStruct> deviceListConfigured;

    //functions
    bool getDeviceConfiguration(QZeroConfService zcs);
    bool getDeviceInformation(QZeroConfService zcs);

    QString getVersion(QDomDocument document, QString element);


    bool getDeviceInformation(DevMgmtPublisherStruct &device);
    bool getDeviceConfiguration(DevMgmtPublisherStruct &device);
protected:
    void deviceInformationToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device);
    void deviceConfigurationToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device);
    void deviceStatusToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device);
private:


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
