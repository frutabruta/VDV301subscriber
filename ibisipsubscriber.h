#ifndef IBISIPSUBSCRIBER_H
#define IBISIPSUBSCRIBER_H

#include <QObject>
#include <QtHttpServer>
#include <QtXml>
#include "httpserversubscriber.h"
#include "QtZeroConf/qzeroconf.h"
#include "xmlgeneratorsubscriber.h"
#include "publisherstruct.h"
class IbisIpSubscriber : public QObject
{
    Q_OBJECT
public:

    //konstruktor a destruktor
    // explicit IbisIpSubscriber(QObject *parent = nullptr);
    IbisIpSubscriber(QString serviceName, QString structureName, QString version, QString serviceType, int portNumber);

    //instance knihoven
    XmlGeneratorSubscriber xmlGeneratorSubscriber;

    //variables
    QVector<QZeroConfService> serviceList;
    QHostAddress deviceAddress;
    QNetworkAccessManager postManager;
    QNetworkReply *reply;

    //functions
    QByteArray createOkResponse();
    void findServices(QString serviceType, int start);
    QString createSubscribeRequest(QHostAddress clientIpAddress, int port);
    QString createUnsubscribeRequest(QHostAddress clientIpAddress, int port);

    //nezarazeno
    //int portNumber() const;
    //void setPortNumber(int newPortNumber);

    QString version() const;
    void setVersion(const QString &newVersion);

    bool isIpSet() const;
    void setIsIpSet(bool newIsIpSet);

    void start();
    int portNumber() const;
    void setPortNumber(int newPortNumber);
    QHostAddress selectNonLoopbackAddressInSubnet(QHostAddress addressOfPublisher, int mask);
    int subnetMask() const;
    void setSubnetMask(int newSubnetMask);

    QString structureName() const;
    void setStructureName(const QString &newStructureName);


private:

    void allConnects();

protected:

    //instance knihoven
    HttpServerSubscriber httpServerSubscriber;

    //variables
    QZeroConf zeroConf;
    QString mServiceType="_ibisip_http._tcp";

   // int mPortNumber=0;
    QString mHeader=""; //unused?
    QString mServiceName="";
    QString mStructureName="";
    QString mVersion="";

    bool mIsIpSet=false;

    int mSubnetMask=16;

    //funkce
    QUrl createSubscribeDestination(PublisherStruct publisherStruct);
    int isTheServiceRequestedOne(QString selectedServiceName,QString selectedVersion, QZeroConfService zcs);
    int isTheServiceRequestedOne(QString selectedServiceName, QString selectedVersion, QString testedServiceName, QString testedVersion);
    int deleteServiceFromList(QVector<QZeroConfService> &serviceList, QZeroConfService selectedService);
    QHostAddress selectNonLoopbackAddress();
    //ostatni

    void postGenericRequest(QUrl subscriberAddress, QString postRequestContent);

signals:
    int signalDataReceived (QString receivedData);
    int signalError (QString errorText);
    void signalUpdateDeviceList();
    void signalSubscriptionLost();

//public slots:

protected slots:
    void slotHttpRequestGenericFinished();
private slots:


};

#endif // IBISIPSUBSCRIBER_H
