#include "ibisipsubscriberonepublisher.h"

Q_LOGGING_CATEGORY(IbisIpSubscriberOnePublisherLog, "IbisIpSubscriberOnePublisher")

IbisIpSubscriberOnePublisher::IbisIpSubscriberOnePublisher(QZeroConf& zeroConf, QString serviceName,QString structureName,QString version,QString serviceType, int portName, QString replyPath)
    : IbisIpSubscriber(zeroConf, serviceName, structureName, version, serviceType, portName, replyPath)
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

    connect(&zeroConf, &QZeroConf::serviceAdded, this, qOverload<QZeroConfService>(&IbisIpSubscriberOnePublisher::slotAddService));
    connect(&zeroConf, &QZeroConf::serviceUpdated, this, &IbisIpSubscriberOnePublisher::slotUpdateService);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &IbisIpSubscriberOnePublisher::slotServiceRemoved);

    connect(this,&IbisIpSubscriber::signalIsSubscriptionSuccesful,this,&IbisIpSubscriberOnePublisher::slotSubscriptionFailed);

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
    qCInfo(IbisIpSubscriberOnePublisherLog).noquote()<<"posting subscribe request to: "<<subscriberAddress;
    qCDebug(IbisIpSubscriberOnePublisherLog).noquote()<<"request content: "<<postRequestContent;
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
    // connect(reply, &QNetworkReply::errorOccurred, this, &IbisIpSubscriber::slotHttpRequestErrorHappened);
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

    qCInfo(IbisIpSubscriberOnePublisherLog) << "updating service "<<PublisherStruct(zcs).dumpToQString();

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

    qCInfo(IbisIpSubscriberOnePublisherLog) <<"new service detected zcs"<<publisherStruct.serviceName<<" ip address "<<publisherStruct.hostAddress<<" portNumber "<<QString::number(publisherStruct.portNumber)<<" data" <<publisherStruct.ibisIpVersion;

    if(!serviceList.contains(zcs))
    {
        serviceList.append(zcs);
    }

    slotAddService(PublisherStruct(zcs));
}


void IbisIpSubscriberOnePublisher::slotAddService(PublisherStruct publisherStruct)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;

    //  QString serviceName=zcs->name();
    //   QString ipAddress=zcs->ip().toString();
    //   QString version=zcs.data()->txt().value("ver");
    //  int portNumber=zcs->port();

    qCInfo(IbisIpSubscriberOnePublisherLog) <<"new service detected zcs"<<publisherStruct.serviceName<<" ip address "<<publisherStruct.hostAddress<<" portNumber "<<QString::number(publisherStruct.portNumber)<<" data" <<publisherStruct.ibisIpVersion;



    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,publisherStruct))
    {
        if(!isCandidateSelected)
        {
            if(this->isSubscriptionActive==false)
            {
                qCInfo(IbisIpSubscriberOnePublisherLog)<<"sending subscribe request to  "<<publisherStruct.hostAddress<<":"<<QString::number(publisherStruct.portNumber)<<" service "<<publisherStruct.serviceName;

                QUrl subscriptionDestination=createSubscribeDestination(publisherStruct);  //QUrl(addressComplete);
                isCandidateSelected=true;
                subscribeServiceCandidate=publisherStruct;

                deviceAddress=selectNonLoopbackAddressInSubnet(publisherStruct.hostAddress,mSubnetMask);

                if(deviceAddress.toString().isEmpty())
                {
                    qCWarning(IbisIpSubscriberOnePublisherLog)<<"couldn't find device address!";
                    isCandidateSelected=false;
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
            qCInfo(IbisIpSubscriberOnePublisherLog)<<"testing other candidate, putting to queue: "<<publisherStruct.dumpToQString();
            subscribeCandidateList.push_back(publisherStruct);
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

    IbisIpSubscriberOnePublisher::slotAddServiceManual(subscribeServiceCandidate);

}

void IbisIpSubscriberOnePublisher::slotAddServiceManual(PublisherStruct addedPublisher)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;

    subscribeServiceCandidate=addedPublisher;



    qCInfo(IbisIpSubscriberOnePublisherLog) <<"service added manually "<<subscribeServiceCandidate.serviceName<<" ip address "<<subscribeServiceCandidate.hostAddress<<" portNumber "<<QString::number(subscribeServiceCandidate.portNumber)<<" data" <<subscribeServiceCandidate.ibisIpVersion;

    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,subscribeServiceCandidate))
    {
        if(this->isCandidateSelected==false)
        {
            if(this->isSubscriptionActive==false)
            {
                qCInfo(IbisIpSubscriberOnePublisherLog)<<"sending subscribe request to  "<<subscribeServiceCandidate.hostAddress<<":"<<QString::number(subscribeServiceCandidate.portNumber)<<" service "<<subscribeServiceCandidate.serviceName;

                QUrl subscriptionDestination=createSubscribeDestination(addedPublisher);

                isCandidateSelected=true;


                deviceAddress=selectNonLoopbackAddressInSubnet(subscribeServiceCandidate.hostAddress,mSubnetMask);
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

//unused
void IbisIpSubscriberOnePublisher::slotAddServiceManualForce(QString serviceName, QString version, QString ipAddress,  int portNumber)
{
    qCDebug(IbisIpSubscriberOnePublisherLog) <<  Q_FUNC_INFO;


    qCInfo(IbisIpSubscriberOnePublisherLog) <<"manually added service "<<serviceName<<" ip address "<<ipAddress<<" portNumber "<<QString::number(portNumber)<<" data" <<version;

    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,serviceName,version))
    {
        if(this->isCandidateSelected==false)
        {
            if(this->isSubscriptionActive==false)
            {
                qCInfo(IbisIpSubscriberOnePublisherLog)<<"sending subscribe request to  "<<ipAddress<<":"<<QString::number(portNumber)<<" service "<<serviceName;

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
        qCWarning(IbisIpSubscriberOnePublisherLog)<< "XML parse error:" << parseErrorMsg << "at" << parseErrorLine << ":" << parseErrorCol;
        emit signalIsSubscriptionSuccesful(false); //check
        return;
    }

    QString rootTag=doc.firstChild().nodeName();
    if(rootTag=="xml")
    {
        rootTag=doc.firstChild().nextSibling().nodeName();
    }
    qCDebug(IbisIpSubscriberOnePublisherLog)<<"rootTag "<<rootTag;

    if(rootTag=="SubscribeResponse")
    {
        QDomNodeList activeNodes = doc.elementsByTagName("Active");
        if (activeNodes.isEmpty())
        {
            qCWarning(IbisIpSubscriberOnePublisherLog) << "Missing <Active> element";
            emit signalIsSubscriptionSuccesful(false);
            emit signalError(doc.toString());
            return;
        }

        const QString subscriptionResult = activeNodes.at(0).firstChildElement("Value").firstChild().nodeValue();

        qCDebug(IbisIpSubscriberOnePublisherLog) << "subscription result:" << subscriptionResult;

        if (subscriptionResult.compare("true", Qt::CaseInsensitive) == 0)
        {
            subscribedService = subscribeServiceCandidate;
            qCInfo(IbisIpSubscriberOnePublisherLog)<<"successful subscribed to "<<subscribedService.dumpToQString();
            this->isSubscriptionActive = true;
            emit signalIsSubscriptionSuccesful(true);
            emit signalSubscriptionSuccessful(subscribedService);
            return;
        }
        else
        {
            qCWarning(IbisIpSubscriberOnePublisherLog)<<"subscription failed "<<subscribedService.dumpToQString();
            emit signalIsSubscriptionSuccesful(false);
            emit signalError(doc.toString());
            return;
        }
    }
    else if(rootTag=="UnsubscribeResponse")
    {
        const QDomNodeList activeNodes = doc.elementsByTagName("Active");
        if (activeNodes.isEmpty())
        {
            qCWarning(IbisIpSubscriberOnePublisherLog) << "Missing <Active> element in unsubscription response";
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
            return;
        }
        else
        {
            qCWarning(IbisIpSubscriberOnePublisherLog) << "unsubscription failed";
            emit signalIsUnsubscriptionSuccesful(false);
            emit signalError("unsubscription failed");
            return;
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


void IbisIpSubscriberOnePublisher::slotSubscriptionFailed(bool subscriptionSuccesful)
{
    qDebug()<<Q_FUNC_INFO;

    if(!subscriptionSuccesful)
    {
        isCandidateSelected=false;

        qCWarning(IbisIpSubscriberOnePublisherLog)<<" subscribe request to "<<subscribeServiceCandidate.dumpToQString()<<" FAILED";
        if(!subscribeCandidateList.isEmpty())
        {
            qCInfo(IbisIpSubscriberOnePublisherLog)<<" trying another candidate "<<subscribeCandidateList.first().dumpToQString();
            slotAddService(subscribeCandidateList.first());
            subscribeCandidateList.pop_front();
        }
        else
        {
            qCInfo(IbisIpSubscriberOnePublisherLog)<<" no more candidates in list";
        }
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




