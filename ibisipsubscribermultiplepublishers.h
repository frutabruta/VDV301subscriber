#ifndef IBISIPSUBSCRIBERMULTIPLEPUBLISHERS_H
#define IBISIPSUBSCRIBERMULTIPLEPUBLISHERS_H

//#include <QVector>
#include "ibisipsubscriber.h"
#include "publisherstruct.h"

#include "publisherstruct.h"

class IbisIpSubscriberMultiplePublishers : public IbisIpSubscriber
{
    Q_OBJECT
public:
    //constructor
    explicit IbisIpSubscriberMultiplePublishers(QString serviceName, QString structureName, QString version, QString serviceType, int portNumber);
    ~IbisIpSubscriberMultiplePublishers();

    QVector<PublisherStruct> publisherList;

    PublisherStruct publisherServiceCandidate;


    void postSubscribe(QUrl subscriberAddress, QString postRequestContent);
    void postSubscribe(PublisherStruct publisherCandidate);

    void postUnsubscribe(QUrl subscriberAddress, QString postRequestContent);
    void postUnsubscribe(PublisherStruct publisherCandidate);

    void slotAddServiceManual(PublisherStruct publisher);
    void slotAddServiceManualForce(PublisherStruct publisher);

    bool ignoreSubscribeError=false;

    void unsubscribe(PublisherStruct publisher);

    void slotHandleReceivedData(QString receivedData);

private:

public slots:
    void slotRemoveDnsSd(QZeroConfService zcs);
    void slotNewDnsSd(QZeroConfService zcs);
    void slotUpdateDnsSd(QZeroConfService zcs);
protected:

    void tryToAddPublisher(PublisherStruct publisher);
protected slots:
    void slotHttpRequestUnsubscriptionFinished();
    void slotHttpRequestSubscriptionFinished();

signals:
    void signalSubscriptionSuccessful(PublisherStruct zcs);
    void signalIsSubscriptionSuccessful(bool result);
    void signalUnsubscriptionSuccessful(PublisherStruct zcs);
    void signalIsUnsubscriptionSuccesful(bool result);
    void signalNewPublisherDiscovered(PublisherStruct publisher);



};

#endif // IBISIPSUBSCRIBERMULTIPLEPUBLISHERS_H
