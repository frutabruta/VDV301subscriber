#ifndef IBISIPSUBSCRIBERONEPUBLISHER_H
#define IBISIPSUBSCRIBERONEPUBLISHER_H

#include "ibisipsubscriber.h"



class IbisIpSubscriberOnePublisher : public IbisIpSubscriber
{
    Q_OBJECT
public:

    //construktor a destructor
    explicit IbisIpSubscriberOnePublisher(QString serviceName, QString structureName, QString version, QString serviceType, int portName, QString replyPath="");
    ~IbisIpSubscriberOnePublisher();

    //instance knihoven
    QTimer timerHeartbeatCheck;    

    //variables
    int heartbeatCheckInterval=120000;
    bool isSubscriptionActive=false;
    bool isCandidateSelected=false;

    PublisherStruct subscribedService;
    PublisherStruct subscribeServiceCandidate;
    QVector<PublisherStruct> subscribeCandidateList;

    //functions
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
    void slotAddService(PublisherStruct publisherStruct);
    void slotAddServiceManual(QString serviceName, QString version, QString ipAddress, int portNumber);
    void slotAddServiceManual(PublisherStruct addedPublisher);
    void slotAddServiceManualForce(QString serviceName, QString version, QString ipAddress, int portNumber); //unused
    void slotUpdateService(QZeroConfService zcs);
    void slotHandleReceivedData(QString receivedData);



protected slots:
    void slotServiceRemoved(QZeroConfService zcs);

    void slotHandleResponseContent(QString responseContent);
    void slotSubscriptionFailed(bool subscriptionSuccesful);
signals:
    void signalSubscriptionSuccessful(PublisherStruct zcs);
    void signalUnsubscriptionSuccessful(PublisherStruct zcs);
    void signalIsUnsubscriptionSuccesful(bool result);
};

#endif // IBISIPSUBSCRIBERONEPUBLISHER_H
