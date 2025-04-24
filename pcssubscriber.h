#ifndef PCSSUBSCRIBER_H
#define PCSSUBSCRIBER_H

//#include "VDV301subscriber/ibisipsubscriberonepublisher.h"
#include "VDV301subscriber/ibisipsubscribermultiplepublishers.h"
#include "xmlgeneratorsubscriberpcs.h"
#include "pcspublisherstruct.h"


class PcsSubscriber :  public IbisIpSubscriberMultiplePublishers // public IbisIpSubscriberOnePublisher
{
    Q_OBJECT


public:

    explicit PcsSubscriber(QString serviceName, QString structureName, QString version, QString serviceType, int portNumber);

    void startCounting(PcsPublisherStruct pcsPublisher);
    void stopCounting(PcsPublisherStruct pcsPublisher);
    void setCountersZero(PcsPublisherStruct pcsPublisher);

    QVector<PcsPublisherStruct> pcsPublisherList;


    void allConnects();


public slots:
    void slotStartCountingFinished(QString input);
    void slotNewPublisherAdded(PublisherStruct publisherStruct);
    void slotNewPublisherDiscovered(PublisherStruct publisherStruct);
private:
    XmlGeneratorSubscriberPcs xmlGeneratorSubscriber;

signals:
    void responseReceived(QString data);




};

#endif // PCSSUBSCRIBER_H
