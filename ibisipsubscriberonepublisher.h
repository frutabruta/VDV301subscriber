#ifndef IBISIPSUBSCRIBERONEPUBLISHER_H
#define IBISIPSUBSCRIBERONEPUBLISHER_H

#include "ibisipsubscriber.h"


class IbisIpSubscriberOnePublisher : public IbisIpSubscriber
{
    Q_OBJECT
public:


    //konstruktor a destruktor
    explicit IbisIpSubscriberOnePublisher(QString serviceName, QString structureName, QString version, QString serviceType, int portName);

    //instance knihoven
   // QTimer *timerHeartbeatCheck = new QTimer(this);
    QTimer timerHeartbeatCheck;
    //promenne
    int heartbeatCheckInterval=120000;
    bool isSubscriptionActive=false;
    bool isCandidateSelected=false;

    QZeroConfService subscribedService;
    QZeroConfService subscribeServiceCandidate;

    //funkce

    void start();
    void newSubscribeRequest();

    void postSubscribe(QUrl subscriberAddress, QString postRequestContent);


private:
    void allConnects();

    void allConnects2();

public slots:
    void slotHeartbeatTimeout();
    void slotAddService(QZeroConfService zcs);
    void slotUpdateService(QZeroConfService zcs);
    void slotHandleReceivedData(QString receivedData);
private slots:
    void slotHttpFinished();
    void slotSubscribeSent(QNetworkReply *subscriptionReply);
    void slotServiceRemoved(QZeroConfService zcs);

signals:
    void signalSubscriptionSuccessful(QZeroConfService zcs);

};

#endif // IBISIPSUBSCRIBERONEPUBLISHER_H
