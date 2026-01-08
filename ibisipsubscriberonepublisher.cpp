#include "ibisipsubscriberonepublisher.h"

Q_LOGGING_CATEGORY(IbisIpSubscriberOnePublisherLog, "IbisIpSubscriberOnePublisher")

IbisIpSubscriberOnePublisher::IbisIpSubscriberOnePublisher(QString serviceName,QString structureName,QString version,QString serviceType, int portName, QString replyPath) : IbisIpSubscriber(serviceName,structureName, version, serviceType, portName, replyPath)
{
    qCDebug(IbisIpSubscriberOnePublisherLog)<<Q_FUNC_INFO;

    // vsechnyConnecty();    
}



IbisIpSubscriberOnePublisher::~IbisIpSubscriberOnePublisher()
{
    qCDebug(IbisIpSubscriberOnePublisherLog)<<Q_FUNC_INFO;
    disconnect();
    //  unsubscribe();
}


void IbisIpSubscriberOnePublisher::start()
{
    qCDebug(IbisIpSubscriberOnePublisherLog)<<Q_FUNC_INFO;
    timerHeartbeatCheck.start(heartbeatCheckInterval);
    httpServerSubscriber.setReplyPath(mReplyPath);
    httpServerSubscriber.start();
    allConnects();
    allConnects2();
}



void IbisIpSubscriberOnePublisher::allConnects()
{
    qCDebug(IbisIpSubscriberOnePublisherLog)<<Q_FUNC_INFO;

    connect(&timerHeartbeatCheck, &QTimer::timeout, this, &IbisIpSubscriberOnePublisher::slotHeartbeatTimeout);
    connect(&httpServerSubscriber ,&HttpServerSubscriber::signalDataReceived,this,&IbisIpSubscriberOnePublisher::slotHandleReceivedData) ;
}


void IbisIpSubscriberOnePublisher::allConnects2()
{
    qCDebug(IbisIpSubscriberOnePublisherLog)<<Q_FUNC_INFO;

    connect(&zeroConf, &QZeroConf::serviceAdded, this, &IbisIpSubscriberOnePublisher::slotAddService);
    connect(&zeroConf, &QZeroConf::serviceUpdated, this, &IbisIpSubscriberOnePublisher::slotUpdateService);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &IbisIpSubscriberOnePublisher::slotServiceRemoved);

    //experimental
    connect(this,&IbisIpSubscriberOnePublisher::signalResponseNotEmpty,this,&IbisIpSubscriberOnePublisher::slotHandleResponseContent);
}

void IbisIpSubscriberOnePublisher::newSubscribeRequest()
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;
    isSubscriptionActive=false;
    isCandidateSelected=false;


    findServices(mServiceType,1);
    checkExistingServices();
}

void IbisIpSubscriberOnePublisher::postSubscribe(QUrl subscriberAddress, QString postRequestContent)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;
    qCDebug(IbisIpSubscriberOnePublisherLog).noquote()<<"posting to address: "<<subscriberAddress<<" "<<postRequestContent;

    QNetworkRequest postRequest(subscriberAddress);


    // https://stackoverflow.com/a/53556560

    postRequest.setTransferTimeout(30000);
    postRequest.setRawHeader("Content-Type", "text/xml");
    //postRequest.setRawHeader("Expect", "100-continue");
    //postRequest.setRawHeader("Connection", "keep-Alive");
    //postRequest.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray postRequestContentQByteArray=postRequestContent.toUtf8() ; 

    QPointer<QNetworkReply> reply=postManager.post(postRequest,postRequestContentQByteArray);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberOnePublisher::slotHttpRequestFinished);

}


void IbisIpSubscriberOnePublisher::postUnsubscribe(QUrl subscriberAddress, QString postRequestContent)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;
    qCDebug(IbisIpSubscriberOnePublisherLog).noquote()<<"posting to address: "<<subscriberAddress<<" "<<postRequestContent;

    QNetworkRequest postRequest(subscriberAddress);


    // https://stackoverflow.com/a/53556560

    postRequest.setTransferTimeout(30000);
    postRequest.setRawHeader("Content-Type", "text/xml");
    //postRequest.setRawHeader("Expect", "100-continue");
    //postRequest.setRawHeader("Connection", "keep-Alive");
    //postRequest.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray postRequestContentQByteArray=postRequestContent.toUtf8() ;

    QPointer<QNetworkReply> reply=postManager.post(postRequest,postRequestContentQByteArray);
  //  connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberOnePublisher::slotHttpRequestUnsubscriptionFinished);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberOnePublisher::slotHttpRequestFinished);

}

int IbisIpSubscriberOnePublisher::portNumber() const
{

    return httpServerSubscriber.portNumber();
}

void IbisIpSubscriberOnePublisher::setPortNumber(int newPortNumber)
{
    httpServerSubscriber.setPortNumber(newPortNumber);
}


void IbisIpSubscriberOnePublisher::slotUpdateService(QZeroConfService zcs)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;

    slotAddService(zcs);
}

void IbisIpSubscriberOnePublisher::checkExistingServices()
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;
    foreach(QZeroConfService service, serviceList)
    {
        if(!isSubscriptionActive)
        {
            slotAddService(service);
        }
    }
}

void IbisIpSubscriberOnePublisher::slotAddService(QZeroConfService zcs)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;

    PublisherStruct publisherStruct(zcs);


    //  QString serviceName=zcs->name();
    //   QString ipAddress=zcs->ip().toString();
    //   QString version=zcs.data()->txt().value("ver");
    //  int portNumber=zcs->port();

    qCDebug(IbisIpSubscriberOnePublisherLog) <<"service name "<<publisherStruct.serviceName<<" ip address "<<publisherStruct.hostAddress<<" portNumber "<<QString::number(publisherStruct.portNumber)<<" data" <<publisherStruct.ibisIpVersion;

    if(!serviceList.contains(zcs))
    {
        serviceList.append(zcs);
    }


    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,zcs))
    {
        if(this->isCandidateSelected==false)
        {
            if(this->isSubscriptionActive==false)
            {
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"sending subscribe request to  "<<publisherStruct.hostAddress<<":"<<QString::number(publisherStruct.portNumber)<<" service "<<publisherStruct.serviceName;

                QString addressAfterBackslash="/"+mServiceName+"/Subscribe"+mStructureName;
                QString addressComplete="http://"+publisherStruct.hostAddress.toString()+":"+QString::number(publisherStruct.portNumber)+addressAfterBackslash;
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"adresaCile string "<<addressComplete;
                QUrl subscriptionDestination=QUrl(addressComplete);
                isCandidateSelected=true;
                subscribeServiceCandidate=zcs;            

                deviceAddress=selectNonLoopbackAddressInSubnet(publisherStruct.hostAddress,mSubnetMask);

                if(deviceAddress.toString().isEmpty())
                {
                    qWarning()<<"couldn't find device address!";
                    emit signalError("couldn't find device address!");
                }
                else
                {
                    postSubscribe(subscriptionDestination,xmlGeneratorSubscriber.createSubscribeRequest(deviceAddress,httpServerSubscriber.portNumber(),mReplyPath));
                }




            }
            else
            {
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"isSubscriptionActive "<<isSubscriptionActive;
            }
        }
        else
        {
            qCDebug(IbisIpSubscriberOnePublisherLog)<<"isCandidateSelected "<<isCandidateSelected;
        }
    }
    else
    {
        qCDebug(IbisIpSubscriberOnePublisherLog)<<"service is not the requested one";
    }
}



void IbisIpSubscriberOnePublisher::slotAddServiceManual(QString serviceName, QString version, QString ipAddress, int portNumber)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;



    subscribeServiceCandidate.serviceName=serviceName;
    subscribeServiceCandidate.ibisIpVersion=version;
    subscribeServiceCandidate.hostAddress=QHostAddress(ipAddress);
    subscribeServiceCandidate.portNumber=portNumber;

    qCDebug(IbisIpSubscriberOnePublisherLog) <<"service name "<<subscribeServiceCandidate.serviceName<<" ip address "<<subscribeServiceCandidate.hostAddress<<" portNumber "<<QString::number(subscribeServiceCandidate.portNumber)<<" data" <<subscribeServiceCandidate.ibisIpVersion;

    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,serviceName,version))
    {
        if(this->isCandidateSelected==false)
        {
            if(this->isSubscriptionActive==false)
            {
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"sending subscribe request to  "<<subscribeServiceCandidate.hostAddress<<":"<<QString::number(subscribeServiceCandidate.portNumber)<<" service "<<subscribeServiceCandidate.serviceName;

                QString addressAfterBackslash="/"+mServiceName+"/Subscribe"+mStructureName;
                QString addressComplete="http://"+subscribeServiceCandidate.hostAddress.toString()+":"+QString::number( subscribeServiceCandidate.portNumber)+addressAfterBackslash;
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"adresaCile string "<<addressComplete;
                QUrl subscriptionDestination=QUrl(addressComplete);
                isCandidateSelected=true;


                deviceAddress=selectNonLoopbackAddressInSubnet(QHostAddress(ipAddress),mSubnetMask);
                postSubscribe(subscriptionDestination,xmlGeneratorSubscriber.createSubscribeRequest(deviceAddress,httpServerSubscriber.portNumber(),mReplyPath));

            }
            else
            {
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"isSubscriptionActive "<<isSubscriptionActive;
            }
        }
        else
        {
            qCDebug(IbisIpSubscriberOnePublisherLog)<<"isCandidateSelected "<<isCandidateSelected;
        }

    }
    else
    {
        qCDebug(IbisIpSubscriberOnePublisherLog)<<"service is not the requested one";
    }

}


void IbisIpSubscriberOnePublisher::slotAddServiceManualForce(QString serviceName, QString version, QString ipAddress,  int portNumber)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;

    qCDebug(IbisIpSubscriberOnePublisherLog) <<"service name "<<serviceName<<" ip address "<<ipAddress<<" portNumber "<<QString::number(portNumber)<<" data" <<version;

    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,serviceName,version))
    {
        if(this->isCandidateSelected==false)
        {
            if(this->isSubscriptionActive==false)
            {
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"sending subscribe request to  "<<ipAddress<<":"<<QString::number(portNumber)<<" service "<<serviceName;

                QString addressAfterBackslash="/"+mServiceName+"/Subscribe"+mStructureName;
                QString addressComplete="http://"+ipAddress+":"+QString::number( portNumber)+addressAfterBackslash;
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"adresaCile string "<<addressComplete;
                QUrl subscriptionDestination=QUrl(addressComplete);
                isCandidateSelected=true;


                deviceAddress=selectNonLoopbackAddressInSubnet(QHostAddress(ipAddress),mSubnetMask);

                subscribedService=subscribeServiceCandidate;
                this->isSubscriptionActive=true;
                emit signalIsSubscriptionSuccesful(true);
                emit signalSubscriptionSuccessful(subscribedService);


            }
            else
            {
                qCDebug(IbisIpSubscriberOnePublisherLog)<<"isSubscriptionActive "<<isSubscriptionActive;
            }
        }
        else
        {
            qCDebug(IbisIpSubscriberOnePublisherLog)<<"isCandidateSelected "<<isCandidateSelected;
        }

    }
    else
    {
        qCDebug(IbisIpSubscriberOnePublisherLog)<<"service is not the requested one";
    }
}

void IbisIpSubscriberOnePublisher::slotHeartbeatTimeout()
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;
    if(!isIpSet() )
    {
        deviceAddress=selectNonLoopbackAddress();
    }
    emit signalSubscriptionLost();

    newSubscribeRequest();
}






void IbisIpSubscriberOnePublisher::slotHandleResponseContent(QString responseContent)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) << "data response:" << responseContent;

    QDomDocument doc;
    QString parseErrorMsg;
    int parseErrorLine = 0;
    int parseErrorCol = 0;

    if (!doc.setContent(responseContent, &parseErrorMsg, &parseErrorLine, &parseErrorCol))
    {
        qCDebug(IbisIpSubscriberOnePublisherLog)
        << "XML parse error:" << parseErrorMsg << "at" << parseErrorLine << ":" << parseErrorCol;
        emit signalIsSubscriptionSuccesful(false); //check
        return;
    }

    QString rootTag=doc.firstChild().nodeName();
    if(rootTag=="xml")
    {
        rootTag=doc.firstChild().nextSibling().nodeName();
    }
    qDebug()<<"rootTag "<<rootTag;

    if(rootTag=="SubscribeResponse")
    {
        QDomNodeList activeNodes = doc.elementsByTagName("Active");
        if (activeNodes.isEmpty())
        {
            qCDebug(IbisIpSubscriberOnePublisherLog) << "Missing <Active> element";
            emit signalIsSubscriptionSuccesful(false);
            emit signalError(doc.toString());
            return;
        }

        const QString subscriptionResult = activeNodes.at(0).firstChildElement("Value").firstChild().nodeValue();

        qCDebug(IbisIpSubscriberOnePublisherLog) << "subscription result:" << subscriptionResult;

        if (subscriptionResult.compare("true", Qt::CaseInsensitive) == 0)
        {
            subscribedService = subscribeServiceCandidate;
            this->isSubscriptionActive = true;
            emit signalIsSubscriptionSuccesful(true);
            emit signalSubscriptionSuccessful(subscribedService);
        }
        else
        {
            qCDebug(IbisIpSubscriberOnePublisherLog) << "subscription failed";
            emit signalIsSubscriptionSuccesful(false);
            emit signalError(doc.toString());
        }
    }
    else if(rootTag=="UnsubscribeResponse")
    {
        const QDomNodeList activeNodes = doc.elementsByTagName("Active");
        if (activeNodes.isEmpty())
        {
            qCDebug(IbisIpSubscriberOnePublisherLog) << "Missing <Active> element in unsubscription response";
            emit signalIsUnsubscriptionSuccesful(false);
            emit signalError("Missing <Active> element");
            return;
        }

        const QString unsubscriptionResult = activeNodes.at(0).firstChildElement("Value").firstChild().nodeValue();

        qCDebug(IbisIpSubscriberOnePublisherLog) << "unsubscription result:" << unsubscriptionResult;

        if (unsubscriptionResult.compare("false", Qt::CaseInsensitive) == 0)
        {
            this->isSubscriptionActive = false;
            emit signalIsUnsubscriptionSuccesful(true);
            emit signalUnsubscriptionSuccessful(subscribedService);
        }
        else
        {
            qCDebug(IbisIpSubscriberOnePublisherLog) << "unsubscription failed";
            emit signalIsUnsubscriptionSuccesful(false);
            emit signalError("unsubscription failed");
        }
    }
    else
    {
        qCDebug(IbisIpSubscriberOnePublisherLog)<<" unexpected tag "<<rootTag;
    }


}





void IbisIpSubscriberOnePublisher::slotServiceRemoved(QZeroConfService zcs)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;
    deleteServiceFromList(serviceList,zcs);
    if(PublisherStruct(zcs)==subscribedService)
    {
        emit signalSubscriptionLost();
        newSubscribeRequest();
    }
}




void IbisIpSubscriberOnePublisher::slotHandleReceivedData(QString receivedData)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;
    // QByteArray posledniRequest=InstanceNovehoServeru.bodyPozadavku;
    QDomDocument xmlrequest;
    xmlrequest.setContent(receivedData);
    timerHeartbeatCheck.start(heartbeatCheckInterval);
    emit signalDataReceived(receivedData);
}


void IbisIpSubscriberOnePublisher::unsubscribe()
{

    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;
    if(!subscribedService.serviceName.isNull())
    {
        QString addressAfterBackslash="/"+mServiceName+"/Unsubscribe"+mStructureName;
        QString addressComplete="http://"+subscribedService.hostAddress.toString()+":"+QString::number(subscribedService.portNumber)+addressAfterBackslash;
        qCDebug(IbisIpSubscriberOnePublisherLog)<<"adresaCile string "<<addressComplete;
        QUrl subscriptionDestination=QUrl(addressComplete);


        postUnsubscribe(subscriptionDestination,xmlGeneratorSubscriber.createUnsubscribeRequest(deviceAddress,httpServerSubscriber.portNumber(),mReplyPath));

    }
    else
    {
        qCDebug(IbisIpSubscriberOnePublisherLog)<<"subscribed service name is empty";
        emit signalError("subscribed service name is empty");
    }
    isSubscriptionActive=false;
    isCandidateSelected=false;

}




