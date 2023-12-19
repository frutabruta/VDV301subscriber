#ifndef IBISIPSUBSCRIBERONEPUBLISHER_H
#define IBISIPSUBSCRIBERONEPUBLISHER_H

#include "ibisipsubscriber.h"


class IbisIpSubscriberOnePublisher : public IbisIpSubscriber
{
    Q_OBJECT
public:


    //konstruktor a destruktor
    explicit IbisIpSubscriberOnePublisher(QString serviceName, QString structureName, QString version, QString serviceType, int portName);
    ~IbisIpSubscriberOnePublisher();

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
    void postUnsubscribe(QUrl subscriberAddress, QString postRequestContent);

    int portNumber() const;
    void setPortNumber(int newPortNumber);

    void unsubscribe();

private:
    void allConnects();

    void allConnects2();

    void checkExistingServices();
public slots:
    void slotHeartbeatTimeout();
    void slotAddService(QZeroConfService zcs);
    void slotUpdateService(QZeroConfService zcs);
    void slotHandleReceivedData(QString receivedData);
private slots:
    void slotHttpRequestSubscriptionFinished();
    void slotSubscribeSent(QNetworkReply *subscriptionReply);
    void slotServiceRemoved(QZeroConfService zcs);

    void slotHttpRequestUnsubscriptionFinished();
signals:
    void signalSubscriptionSuccessful(QZeroConfService zcs);
    void signalIsSubscriptionSuccesful(bool result);
    void signalUnsubscriptionSuccessful(QZeroConfService zcs);
    void signalIsUnsubscriptionSuccesful(bool result);


};

#endif // IBISIPSUBSCRIBERONEPUBLISHER_H
