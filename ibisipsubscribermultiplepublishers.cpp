#include "ibisipsubscribermultiplepublishers.h"

Q_LOGGING_CATEGORY(IbisIpSubscriberMultiplePublishersLog, "IbisIpSubscriberMultiplePublishers")


IbisIpSubscriberMultiplePublishers::IbisIpSubscriberMultiplePublishers(QZeroConf& zeroConf, QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber) :
    IbisIpSubscriber(zeroConf, serviceName,  structureName,  version,  serviceType, portNumber)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog)<<Q_FUNC_INFO;

    connect(&httpServerSubscriber ,&HttpServerSubscriber::signalDataReceived,this,&IbisIpSubscriberMultiplePublishers::slotHandleReceivedData) ;
    connect(this,&IbisIpSubscriber::signalResponseNotEmpty,this,&IbisIpSubscriberMultiplePublishers::slotHandleResponseContent);

}


IbisIpSubscriberMultiplePublishers::~IbisIpSubscriberMultiplePublishers()
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog)<<Q_FUNC_INFO;
    zeroConf.disconnect();//reseni erroru ASSERT failure in DevMgmtSubscriber: "Called object is not of the correct type
    //  this->disconnect();
}



void IbisIpSubscriberMultiplePublishers::unsubscribe(PublisherStruct publisher)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;
    postUnsubscribe(publisher);
}


void IbisIpSubscriberMultiplePublishers::postSubscribe(PublisherStruct publisherCandidate)
{
    publisherServiceCandidate=publisherCandidate;
    deviceAddress=selectNonLoopbackAddressInSubnet(publisherCandidate.hostAddress,mSubnetMask);
    postSubscribe(createSubscribeDestination(publisherCandidate),xmlGeneratorSubscriber.createSubscribeRequest(deviceAddress,httpServerSubscriber.portNumber()));
}


void IbisIpSubscriberMultiplePublishers::postSubscribe(QUrl subscriberAddress, QString postRequestContent)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;
    qCDebug(IbisIpSubscriberMultiplePublishersLog).noquote()<<"posting to address: "<<subscriberAddress<<" "<<postRequestContent;

    QNetworkRequest postRequest(subscriberAddress);


    // https://stackoverflow.com/a/53556560

    postRequest.setTransferTimeout(30000);
    postRequest.setRawHeader("Content-Type", "text/xml");
    //postRequest.setRawHeader("Expect", "100-continue");
    //postRequest.setRawHeader("Connection", "keep-Alive");
    //postRequest.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray postRequestContentQByteArray=postRequestContent.toUtf8() ;

    QPointer<QNetworkReply> reply=postManager.post(postRequest,postRequestContentQByteArray);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberMultiplePublishers::slotHttpRequestFinished);

}


void IbisIpSubscriberMultiplePublishers::postUnsubscribe(PublisherStruct publisherCandidate)
{
    postUnsubscribe(createSubscribeDestination(publisherCandidate),xmlGeneratorSubscriber.createUnsubscribeRequest(deviceAddress,httpServerSubscriber.portNumber()));
}


void IbisIpSubscriberMultiplePublishers::postUnsubscribe(QUrl subscriberAddress, QString postRequestContent)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;
    qCDebug(IbisIpSubscriberMultiplePublishersLog).noquote()<<"posting to address: "<<subscriberAddress<<" "<<postRequestContent;

    QNetworkRequest postRequest(subscriberAddress);


    // https://stackoverflow.com/a/53556560

    postRequest.setTransferTimeout(30000);
    postRequest.setRawHeader("Content-Type", "text/xml");
    //postRequest.setRawHeader("Expect", "100-continue");
    //postRequest.setRawHeader("Connection", "keep-Alive");
    //postRequest.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray postRequestContentQByteArray=postRequestContent.toUtf8() ;

    QPointer<QNetworkReply> reply=postManager.post(postRequest,postRequestContentQByteArray);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberMultiplePublishers::slotHttpRequestFinished);

}



void IbisIpSubscriberMultiplePublishers::slotAddServiceManual(PublisherStruct publisher)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;

    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<"service name "<<publisher.serviceName<<" ip address "<<publisher.hostAddress<<" portNumber "<<QString::number(publisher.portNumber)<<" data "<<publisher.ibisIpVersion;


    emit signalUpdateDeviceList();


    if (isTheServiceRequestedOne(mServiceName,mVersion,publisher))
    {
        if(publisherList.contains(publisher)==false)
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"sending subscribe request to  "<<publisher.hostAddress<<":"<<QString::number(publisher.portNumber)<<" service "<<publisher.serviceName;
            postSubscribe(publisher);
        }
        else
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"publisher already on list";
        }

    }
    else
    {
        qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"service is not the requested one";
    }

    // emit nalezenaSluzba( zcs);

}


void IbisIpSubscriberMultiplePublishers::slotAddServiceManualForce(PublisherStruct publisher)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;

    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<"service name "<<publisher.serviceName<<" ip address "<<publisher.hostAddress<<" portNumber "<<QString::number(publisher.portNumber)<<" data "<<publisher.ibisIpVersion;


    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,publisher))
    {
        if(publisherList.contains(publisher)==false)
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"sending subscribe request to  "<<publisher.hostAddress<<":"<<QString::number(publisher.portNumber)<<" service "<<publisher.serviceName;

            emit signalIsSubscriptionSuccessful(true);
            emit signalSubscriptionSuccessful(publisher);
        }
        else
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"publisher already on list";
        }

    }
    else
    {
        qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"service is not the requested one";
    }

    // emit nalezenaSluzba( zcs);

}



void IbisIpSubscriberMultiplePublishers::slotNewDnsSd(QZeroConfService zcs)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;

    PublisherStruct newDevice(zcs);
    qCDebug(IbisIpSubscriberMultiplePublishersLog)<<newDevice.dumpToQString();
    tryToAddPublisher(newDevice);
}



void IbisIpSubscriberMultiplePublishers::slotUpdateDnsSd(QZeroConfService zcs)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;

    PublisherStruct newDevice(zcs);
    qCDebug(IbisIpSubscriberMultiplePublishersLog)<<newDevice.dumpToQString();
    tryToAddPublisher(newDevice);
}


void IbisIpSubscriberMultiplePublishers::tryToAddPublisher(PublisherStruct publisher)
{
    if(publisher.serviceName.contains(mServiceName))
    {
        if(!publisherList.contains(publisher))
        {
            emit signalNewPublisherDiscovered(publisher);
        }
        else
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"device is already on the list";
        }
    }
    else
    {
        qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"jina sluzba ";
    }

    emit signalUpdateDeviceList();
}



void IbisIpSubscriberMultiplePublishers::slotRemoveDnsSd(QZeroConfService zcs)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;

    PublisherStruct selectedDevice(zcs);

    qCDebug(IbisIpSubscriberMultiplePublishersLog)<<selectedDevice.dumpToQString();

    if(selectedDevice.serviceName.contains(mServiceName))
    {


        if(publisherList.contains(selectedDevice))
        {
            publisherList.remove(publisherList.indexOf(selectedDevice) );
        }
        else
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"device was not present on the list";
        }
    }
    emit signalUpdateDeviceList();
}





void IbisIpSubscriberMultiplePublishers::slotHandleReceivedData(QString receivedData)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) <<  Q_FUNC_INFO;
    // QByteArray posledniRequest=InstanceNovehoServeru.bodyPozadavku;
    QDomDocument xmlrequest;
    xmlrequest.setContent(receivedData);
    // timerHeartbeatCheck.start(heartbeatCheckInterval);
    emit signalDataReceived(receivedData);
}



void IbisIpSubscriberMultiplePublishers::slotHandleResponseContent(QString responseContent)
{
    qCDebug(IbisIpSubscriberMultiplePublishersLog) << "data response:" << responseContent;

    QDomDocument doc;
    QString parseErrorMsg;
    int parseErrorLine = 0;
    int parseErrorCol = 0;

    if (!doc.setContent(responseContent, &parseErrorMsg, &parseErrorLine, &parseErrorCol))
    {
        qCDebug(IbisIpSubscriberMultiplePublishersLog)
        << "XML parse error:" << parseErrorMsg << "at" << parseErrorLine << ":" << parseErrorCol;
        emit signalIsSubscriptionSuccessful(false); //check
        return;
    }

    QString rootTag=doc.firstChild().nodeName();
    if(rootTag=="xml")
    {
        rootTag=doc.firstChild().nextSibling().nodeName();
    }
    qCDebug(IbisIpSubscriberMultiplePublishersLog)<<"rootTag "<<rootTag;

    if(rootTag=="SubscribeResponse")
    {
        QDomNodeList activeNodes = doc.elementsByTagName("Active");
        if (activeNodes.isEmpty())
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog) << "Missing <Active> element";
            emit signalIsSubscriptionSuccessful(false);
            emit signalError(doc.toString());
            return;
        }

        const QString subscriptionResult = activeNodes.at(0).firstChildElement("Value").firstChild().nodeValue();

        qCDebug(IbisIpSubscriberMultiplePublishersLog) << "subscription result:" << subscriptionResult;

        if (subscriptionResult.compare("true", Qt::CaseInsensitive) == 0)
        {
            //subscribedService = subscribeServiceCandidate;
            //this->isSubscriptionActive = true;
            emit signalIsSubscriptionSuccessful(true);
           // emit signalSubscriptionSuccessful(subscribedService);
        }
        else
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog) << "subscription failed";
            emit signalIsSubscriptionSuccessful(false);
            emit signalError(doc.toString());
        }
    }
    else if(rootTag=="UnsubscribeResponse")
    {
        const QDomNodeList activeNodes = doc.elementsByTagName("Active");
        if (activeNodes.isEmpty())
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog) << "Missing <Active> element in unsubscription response";
            emit signalIsUnsubscriptionSuccesful(false);
            emit signalError("Missing <Active> element");
            return;
        }

        const QString unsubscriptionResult = activeNodes.at(0).firstChildElement("Value").firstChild().nodeValue();

        qCDebug(IbisIpSubscriberMultiplePublishersLog) << "unsubscription result:" << unsubscriptionResult;

        if (unsubscriptionResult.compare("false", Qt::CaseInsensitive) == 0)
        {
            //this->isSubscriptionActive = false;
            emit signalIsUnsubscriptionSuccesful(true);
         //   emit signalUnsubscriptionSuccessful(subscribedService);
        }
        else
        {
            qCDebug(IbisIpSubscriberMultiplePublishersLog) << "unsubscription failed";
            emit signalIsUnsubscriptionSuccesful(false);
            emit signalError("unsubscription failed");
        }
    }
    else
    {
        qCDebug(IbisIpSubscriberMultiplePublishersLog)<<" unexpected tag "<<rootTag;
    }


}
