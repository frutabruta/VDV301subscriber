#ifndef IBISIPSUBSCRIBER_H
#define IBISIPSUBSCRIBER_H

#include <QObject>
#include <QtHttpServer>
#include <QtXml>
#include <QPointer>
#include <QLoggingCategory>

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
    IbisIpSubscriber(QString serviceName, QString structureName, QString version, QString serviceType, int portNumber, QString replyPath="");
    ~IbisIpSubscriber();

    //instance knihoven
    XmlGeneratorSubscriber xmlGeneratorSubscriber;

    //variables
    QVector<QZeroConfService> serviceList;
    QHostAddress deviceAddress;



    QNetworkAccessManager postManager;
    // QPointer<QNetworkReply> reply;

    //options
    bool blockBonjour=false;
    bool allowLoopback=false;

    //functions
    QByteArray createOkResponse();
    void findServices(QString serviceType, int start);

    //nezarazeno

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


    QString replyPath() const;
    void setReplyPath(const QString &newReplyPath);

    QString serviceName() const;
    void setServiceName(const QString &newServiceName);

    QString serviceType() const;
    void setServiceType(const QString &newServiceType);

private:

    void allConnects();

    void globalStartBrowse();
    void globalStopBrowse();

protected:

    static constexpr const char* SERVICE_TYPE = "_ibisip_http._tcp";

    static QZeroConf sIbisIpHttpBrowser;
    static int sBrowseRefCount;

    //instance knihoven
    HttpServerSubscriber httpServerSubscriber;

    //variables
    QZeroConf& zeroConf;
    QString mServiceType=SERVICE_TYPE;
    bool mBrowseRequested=false;

    // int mPortNumber=0;
    QString mHeader=""; //unused?
    QString mReplyPath="";
    QString mServiceName="";
    QString mStructureName="";
    QString mVersion="";


    bool mIsIpSet=false;

    //int mSubnetMask=16; // 16 255.255.0.0
    int mSubnetMask=24;                  //24 255.255.0.0

    //funkce
    QUrl createSubscribeDestination(PublisherStruct publisherStruct);
    int isTheServiceRequestedOne(QString selectedServiceName,QString selectedVersion, QZeroConfService zcs);
    int isTheServiceRequestedOne(QString selectedServiceName, QString selectedVersion, PublisherStruct publisherStruct);
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
    void signalResponseNotEmpty(QString response);

    void signalAddressUpdate(QHostAddress address);
    void signalIsSubscriptionSuccesful(bool result);

    //public slots:

protected slots:
  //  void slotHttpRequestGenericFinished();
    void slotHttpRequestFinished();

public slots:
    void slotHttpRequestErrorHappened(QNetworkReply::NetworkError code);

};

#endif // IBISIPSUBSCRIBER_H
