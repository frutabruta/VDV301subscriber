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

    reply=postManager.post(postRequest,postRequestContentQByteArray);
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

    reply=postManager.post(postRequest,postRequestContentQByteArray);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberMultiplePublishers::slotHttpRequestUnsubscriptionFinished);

}


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
}





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
