#include "ibisipsubscribermultiplepublishers.h"




IbisIpSubscriberMultiplePublishers::IbisIpSubscriberMultiplePublishers(QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber) : IbisIpSubscriber(serviceName,  structureName,  version,  serviceType, portNumber)
{
    qDebug()<<Q_FUNC_INFO;

    connect(&httpServerSubscriber ,&HttpServerSubscriber::signalDataReceived,this,&IbisIpSubscriberMultiplePublishers::slotHandleReceivedData) ;

}


IbisIpSubscriberMultiplePublishers::~IbisIpSubscriberMultiplePublishers()
{
    qDebug()<<Q_FUNC_INFO;
    zeroConf.disconnect();//reseni erroru ASSERT failure in DevMgmtSubscriber: "Called object is not of the correct type
    //  this->disconnect();
}



void IbisIpSubscriberMultiplePublishers::unsubscribe(PublisherStruct publisher)
{
    qDebug() <<  Q_FUNC_INFO;
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
    qDebug() <<  Q_FUNC_INFO;
    qDebug().noquote()<<"posting to address: "<<subscriberAddress<<" "<<postRequestContent;

    QNetworkRequest postRequest(subscriberAddress);


    // https://stackoverflow.com/a/53556560

    postRequest.setTransferTimeout(30000);
    postRequest.setRawHeader("Content-Type", "text/xml");
    //postRequest.setRawHeader("Expect", "100-continue");
    //postRequest.setRawHeader("Connection", "keep-Alive");
    //postRequest.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray postRequestContentQByteArray=postRequestContent.toUtf8() ;

    QPointer<QNetworkReply> reply=postManager.post(postRequest,postRequestContentQByteArray);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberMultiplePublishers::slotHttpRequestSubscriptionFinished);

}


void IbisIpSubscriberMultiplePublishers::postUnsubscribe(PublisherStruct publisherCandidate)
{
    postUnsubscribe(createSubscribeDestination(publisherCandidate),xmlGeneratorSubscriber.createUnsubscribeRequest(deviceAddress,httpServerSubscriber.portNumber()));
}


void IbisIpSubscriberMultiplePublishers::postUnsubscribe(QUrl subscriberAddress, QString postRequestContent)
{
    qDebug() <<  Q_FUNC_INFO;
    qDebug().noquote()<<"posting to address: "<<subscriberAddress<<" "<<postRequestContent;

    QNetworkRequest postRequest(subscriberAddress);


    // https://stackoverflow.com/a/53556560

    postRequest.setTransferTimeout(30000);
    postRequest.setRawHeader("Content-Type", "text/xml");
    //postRequest.setRawHeader("Expect", "100-continue");
    //postRequest.setRawHeader("Connection", "keep-Alive");
    //postRequest.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray postRequestContentQByteArray=postRequestContent.toUtf8() ;

    QPointer<QNetworkReply> reply=postManager.post(postRequest,postRequestContentQByteArray);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberMultiplePublishers::slotHttpRequestUnsubscriptionFinished);

}

/*
void IbisIpSubscriberMultiplePublishers::slotHttpRequestSubscriptionFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"subscribe response:"<<str;

    if(reply->error()!=QNetworkReply::NoError)
    {
        qDebug()<<reply->errorString();
        emit signalIsSubscriptionSuccessful(false);
        return;
    }

    QDomDocument qDomResponse;
    bool setContentResult=false;
    if(qDomResponse.setContent(str))
    {
        setContentResult=true;
    }



    if(setContentResult)
    {
        QString subscriptionResult=qDomResponse.elementsByTagName("Active").at(0).firstChildElement("Value").firstChild().nodeValue();
        qDebug()<<"subscription result: "<<subscriptionResult;
        if((subscriptionResult=="true")||(subscriptionResult=="True")||(ignoreSubscribeError))
        {
            qDebug()<<"subscription successful";
            publisherList<<publisherServiceCandidate;
            emit signalSubscriptionSuccessful(publisherServiceCandidate);
        }
        else
        {
            if(!ignoreSubscribeError)
            {


            qDebug()<<"subscription failed";
            emit signalIsSubscriptionSuccessful(false);
            emit signalError(qDomResponse.toString());
            }
        }
    }
    else
    {
        emit signalIsSubscriptionSuccessful(false);
    }


    reply->deleteLater();
    //reply = nullptr;
}*/



void IbisIpSubscriberMultiplePublishers::slotHttpRequestSubscriptionFinished()
{
    QPointer<QNetworkReply> reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
    {
        qDebug() << "Subscription slot without valid QNetworkReply sender";
        emit signalIsSubscriptionSuccessful(false);
        return;
    }

    qDebug() << Q_FUNC_INFO
             << "URL:" << reply->url()
             << "HTTP status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (!reply->isOpen())
    {
        qDebug() << "Reply device not open";
        emit signalIsSubscriptionSuccessful(false);
        reply->deleteLater();
        return;
    }

    const QByteArray bts = reply->readAll();
    const QString str = QString::fromUtf8(bts);
    qDebug() << "subscribe response:";
    qDebug().noquote() << str;

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network error:" << reply->errorString();
        emit signalIsSubscriptionSuccessful(false);
        reply->deleteLater();
        return;
    }

    QDomDocument doc;
    QString parseErrorMsg;
    int parseErrorLine = 0, parseErrorCol = 0;
    if (!doc.setContent(str, &parseErrorMsg, &parseErrorLine, &parseErrorCol))
    {
        qDebug() << "XML parse error:" << parseErrorMsg << "at" << parseErrorLine << ":" << parseErrorCol;
        emit signalIsSubscriptionSuccessful(false);
        emit signalError(QString("Invalid XML: %1 at %2:%3").arg(parseErrorMsg).arg(parseErrorLine).arg(parseErrorCol));
        reply->deleteLater();
        return;
    }

    const QDomNodeList activeNodes = doc.elementsByTagName("Active");
    if (activeNodes.isEmpty())
    {
        qDebug() << "Missing <Active> element";
        if (!ignoreSubscribeError) {
            emit signalIsSubscriptionSuccessful(false);
            emit signalError(doc.toString());
        }
        reply->deleteLater();
        return;
    }

    const QString subscriptionResult = activeNodes.at(0).firstChildElement("Value").firstChild().nodeValue();

    qDebug() << "subscription result:" << subscriptionResult;

    const bool isActiveTrue = (subscriptionResult.compare("true", Qt::CaseInsensitive) == 0);

    if (isActiveTrue || ignoreSubscribeError)
    {
        qDebug() << "subscription successful";
        publisherList << publisherServiceCandidate;
        emit signalSubscriptionSuccessful(publisherServiceCandidate);

        // If you also want to notify the 'success' boolean explicitly:
        emit signalIsSubscriptionSuccessful(true);
    }
    else
    {
        qDebug() << "subscription failed";
        emit signalIsSubscriptionSuccessful(false);
        emit signalError(doc.toString());
    }

    reply->deleteLater();
}


/*

void IbisIpSubscriberMultiplePublishers::slotHttpRequestUnsubscriptionFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"unsusbscription response:";
    qDebug().noquote()<<str;

    if(reply->error()!=QNetworkReply::NoError)
    {
        qDebug()<<reply->errorString();
        emit signalIsUnsubscriptionSuccesful(false);
        reply->deleteLater();
        return;
    }

    // subscribedService=subscribeServiceCandidate;
    QDomDocument qDomResponse;
    bool setContentResult=false;

    if(qDomResponse.setContent(str))
    {
        setContentResult=true;
    }


    if(setContentResult)
    {
        QString unsubscriptionResult=qDomResponse.elementsByTagName("Active").at(0).firstChildElement("Value").firstChild().nodeValue();
        qDebug()<<"unsubscription result: "<<unsubscriptionResult;
        if((unsubscriptionResult=="false")||(unsubscriptionResult=="False"))
        {
            emit signalIsUnsubscriptionSuccesful(true);
            //   emit signalUnsubscriptionSuccessful(subscribedService);
        }
        else
        {
            qDebug()<<"unsubscription failed";
            emit signalIsUnsubscriptionSuccesful(false);
            emit signalError("unsubscription failed");
        }
    }
    else
    {
        emit signalIsUnsubscriptionSuccesful(false);
        emit signalError("unsubscription reply empty");
    }



    reply->deleteLater();
    //reply = nullptr;


}


*/



void IbisIpSubscriberMultiplePublishers::slotHttpRequestUnsubscriptionFinished()
{
    QPointer<QNetworkReply> reply = qobject_cast<QNetworkReply*>(sender());
    if (reply == nullptr)
    {
        qDebug() << "Unsubscription slot without valid QNetworkReply sender";
        emit signalIsUnsubscriptionSuccesful(false);
        return;
    }

    qDebug() << Q_FUNC_INFO
             << "URL:" << reply->url()
             << "HTTP status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    // Prevent QIODevice::read ... device not open
    if (!reply->isOpen())
    {
        qDebug() << "Reply device not open";
        emit signalIsUnsubscriptionSuccesful(false);
        reply->deleteLater();
        return;
    }

    const QByteArray bts = reply->readAll();
    const QString str = QString::fromUtf8(bts);
    qDebug() << "unsubscription response:";
    qDebug().noquote() << str;

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network error:" << reply->errorString();
        emit signalIsUnsubscriptionSuccesful(false);
        reply->deleteLater();
        return;
    }

    QDomDocument doc;
    QString parseErrorMsg;
    int parseErrorLine = 0;
    int parseErrorCol = 0;

    const bool ok = doc.setContent(str, &parseErrorMsg, &parseErrorLine, &parseErrorCol);
    if (!ok)
    {
        qDebug() << "XML parse error:" << parseErrorMsg << "at" << parseErrorLine << ":" << parseErrorCol;
        emit signalIsUnsubscriptionSuccesful(false);
        emit signalError(QString("Invalid XML: %1 at %2:%3").arg(parseErrorMsg).arg(parseErrorLine).arg(parseErrorCol));
        reply->deleteLater();
        return;
    }

    const QDomNodeList activeNodes = doc.elementsByTagName("Active");
    if (activeNodes.isEmpty())
    {
        qDebug() << "Missing <Active> element in unsubscription response";
        emit signalIsUnsubscriptionSuccesful(false);
        emit signalError("Missing <Active> element");
        reply->deleteLater();
        return;
    }

    const QDomElement activeElem = activeNodes.at(0).toElement();
    const QDomElement valueElem = activeElem.firstChildElement("Value");
    if (valueElem.isNull())
    {
        qDebug() << "Missing <Value> element under <Active>";
        emit signalIsUnsubscriptionSuccesful(false);
        emit signalError("Missing <Value> element");
        reply->deleteLater();
        return;
    }

    const QString unsubscriptionResult = valueElem.firstChild().nodeValue();
    qDebug() << "unsubscription result:" << unsubscriptionResult;

    // Treat "false" (case-insensitive) as successful unsubscription
    const bool isInactive = (unsubscriptionResult.compare("false", Qt::CaseInsensitive) == 0);

    if (isInactive)
    {
        emit signalIsUnsubscriptionSuccesful(true);
        // If you want to emit which publisher/service was unsubscribed, you can:
        // emit signalUnsubscriptionSuccessful(subscribedService);
    }
    else
    {
        qDebug() << "unsubscription failed";
        emit signalIsUnsubscriptionSuccesful(false);
        emit signalError(doc.toString());
    }

    reply->deleteLater();
}


void IbisIpSubscriberMultiplePublishers::slotAddServiceManual(PublisherStruct publisher)
{
    qDebug() <<  Q_FUNC_INFO;

    qDebug() <<"service name "<<publisher.serviceName<<" ip address "<<publisher.hostAddress<<" portNumber "<<QString::number(publisher.portNumber)<<" data "<<publisher.ibisIpVersion;


    emit signalUpdateDeviceList();


    if (isTheServiceRequestedOne(mServiceName,mVersion,publisher.serviceName,publisher.ibisIpVersion))
    {
        if(publisherList.contains(publisher)==false)
        {
            qDebug()<<"sending subscribe request to  "<<publisher.hostAddress<<":"<<QString::number(publisher.portNumber)<<" service "<<publisher.serviceName;
            postSubscribe(publisher);
        }
        else
        {
            qDebug()<<"publisher already on list";
        }

    }
    else
    {
        qDebug()<<"service is not the requested one";
    }

    // emit nalezenaSluzba( zcs);

}


void IbisIpSubscriberMultiplePublishers::slotAddServiceManualForce(PublisherStruct publisher)
{
    qDebug() <<  Q_FUNC_INFO;

    qDebug() <<"service name "<<publisher.serviceName<<" ip address "<<publisher.hostAddress<<" portNumber "<<QString::number(publisher.portNumber)<<" data "<<publisher.ibisIpVersion;


    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,publisher.serviceName,publisher.ibisIpVersion))
    {
        if(publisherList.contains(publisher)==false)
        {
            qDebug()<<"sending subscribe request to  "<<publisher.hostAddress<<":"<<QString::number(publisher.portNumber)<<" service "<<publisher.serviceName;

            emit signalIsSubscriptionSuccessful(true);
            emit signalSubscriptionSuccessful(publisher);
        }
        else
        {
            qDebug()<<"publisher already on list";
        }

    }
    else
    {
        qDebug()<<"service is not the requested one";
    }

    // emit nalezenaSluzba( zcs);

}



void IbisIpSubscriberMultiplePublishers::slotNewDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    PublisherStruct newDevice(zcs);
    qDebug()<<newDevice.dumpToQString();
    tryToAddPublisher(newDevice);
}



void IbisIpSubscriberMultiplePublishers::slotUpdateDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    PublisherStruct newDevice(zcs);
    qDebug()<<newDevice.dumpToQString();    
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
            qDebug()<<"device is already on the list";
        }
    }
    else
    {
        qDebug()<<"jina sluzba ";
    }

    emit signalUpdateDeviceList();
}



void IbisIpSubscriberMultiplePublishers::slotRemoveDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    PublisherStruct selectedDevice(zcs);

    qDebug()<<selectedDevice.dumpToQString();

    if(selectedDevice.serviceName.contains(mServiceName))
    {


        if(publisherList.contains(selectedDevice))
        {
            publisherList.remove(publisherList.indexOf(selectedDevice) );
        }
        else
        {
            qDebug()<<"device was not present on the list";
        }
    }
    emit signalUpdateDeviceList();
}





void IbisIpSubscriberMultiplePublishers::slotHandleReceivedData(QString receivedData)
{
    qDebug() <<  Q_FUNC_INFO;
    // QByteArray posledniRequest=InstanceNovehoServeru.bodyPozadavku;
    QDomDocument xmlrequest;
    xmlrequest.setContent(receivedData);
   // timerHeartbeatCheck.start(heartbeatCheckInterval);
    emit signalDataReceived(receivedData);
}
