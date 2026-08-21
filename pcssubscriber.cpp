#include "pcssubscriber.h"


PcsSubscriber::PcsSubscriber(QString serviceName,QString structureName,QString version,QString serviceType, int portNumber): IbisIpSubscriberMultiplePublishers(serviceName,structureName,version, serviceType, portNumber)
{
    qDebug()<<Q_FUNC_INFO;
    allConnects();
    findServices(mServiceType,1);
}

void PcsSubscriber::allConnects()
{
    qDebug()<<Q_FUNC_INFO;
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &PcsSubscriber::slotNewDnsSd);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &PcsSubscriber::slotRemoveDnsSd);
    connect(&zeroConf, &QZeroConf::serviceUpdated, this, &PcsSubscriber::slotRemoveDnsSd);

    connect(this,&IbisIpSubscriberMultiplePublishers::signalSubscriptionSuccessful,  this, &PcsSubscriber::slotNewPublisherAdded);
    connect(this, &IbisIpSubscriberMultiplePublishers::signalNewPublisherDiscovered, this, &PcsSubscriber::slotNewPublisherDiscovered);


//    connect(&manager,&QNetworkAccessManager::finished,this,&PcsSubscriber::slotRequestReceived);
//    connect(this,&DevMgmtSubscriber::downloadFinished,this,&PcsSubscriber::slotHandleData);
//    connect(&httpServerSubscriber ,&HttpServerSubscriber::signalDataReceived,this,&PcsSubscriber::slotHandleReceivedData) ;
//    connect(&httpServerSubscriber ,&HttpServerSubscriber::signalWholeRequest,this,&PcsSubscriber::slotHandleWholeRequest) ;
}

void PcsSubscriber::startCounting(PcsPublisherStruct pcsPublisher)
{
    qDebug() <<  Q_FUNC_INFO;

    QString addressAfterBackslash="/"+mServiceName+"/StartCounting";
    QString addressComplete="http://"+pcsPublisher.hostAddress.toString()+":"+QString::number(pcsPublisher.portNumber)+addressAfterBackslash;
    qDebug()<<"adresaCile string "<<addressComplete;
    QUrl subscriptionDestination=QUrl(addressComplete);
    postGenericRequest(subscriptionDestination,xmlGeneratorSubscriber.startCounting(pcsPublisher.doorNumber));
}


void PcsSubscriber::stopCounting(PcsPublisherStruct pcsPublisher)
{
    qDebug() <<  Q_FUNC_INFO;
    QString addressAfterBackslash="/"+mServiceName+"/StopCounting";
    QString addressComplete="http://"+pcsPublisher.hostAddress.toString()+":"+QString::number(pcsPublisher.portNumber)+addressAfterBackslash;
    qDebug()<<"adresaCile string "<<addressComplete;
    QUrl subscriptionDestination=QUrl(addressComplete);
    postGenericRequest(subscriptionDestination,xmlGeneratorSubscriber.stopCounting(pcsPublisher.doorNumber));

    /*
    if(!subscribedService.isNull())
    {
    }
    else
    {
        emit signalError("not subscribed");
    }
*/

}

void PcsSubscriber::setCountersZero(PcsPublisherStruct pcsPublisher)
{
    qDebug() <<  Q_FUNC_INFO;
    QString addressAfterBackslash="/"+mServiceName+"/SetCounterData";
    QString addressComplete="http://"+pcsPublisher.hostAddress.toString()+":"+QString::number(pcsPublisher.portNumber)+addressAfterBackslash;
    qDebug()<<"adresaCile string "<<addressComplete;
    QUrl subscriptionDestination=QUrl(addressComplete);
    postGenericRequest(subscriptionDestination,xmlGeneratorSubscriber.setCounterData(pcsPublisher.doorNumber));

    /*
    if(!subscribedService.isNull())
    {
    }
    else
    {
        emit signalError("not subscribed");
    }
*/
}




void PcsSubscriber::slotStartCountingFinished(QString input)
{
    emit responseReceived(input);
}

void PcsSubscriber::slotNewPublisherDiscovered(PublisherStruct publisherStruct)
{
    qDebug()<<Q_FUNC_INFO;
    postSubscribe(publisherStruct);
}


void PcsSubscriber::slotNewPublisherAdded(PublisherStruct publisherStruct)
{
    qDebug()<<Q_FUNC_INFO;
    pcsPublisherList<<PcsPublisherStruct(publisherStruct,1);

}



