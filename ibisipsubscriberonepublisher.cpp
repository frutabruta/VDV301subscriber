#include "ibisipsubscriberonepublisher.h"

IbisIpSubscriberOnePublisher::IbisIpSubscriberOnePublisher(QString serviceName,QString structureName,QString version,QString serviceType, int portName, QString replyPath) : IbisIpSubscriber(serviceName,structureName, version, serviceType, portName, replyPath)
{
    qDebug()<<Q_FUNC_INFO;

    // vsechnyConnecty();    
}



IbisIpSubscriberOnePublisher::~IbisIpSubscriberOnePublisher()
{
    qDebug()<<Q_FUNC_INFO;
    //  unsubscribe();
}


void IbisIpSubscriberOnePublisher::start()
{
    qDebug()<<Q_FUNC_INFO;
    timerHeartbeatCheck.start(heartbeatCheckInterval);
    httpServerSubscriber.start();
    allConnects();
    allConnects2();
}



void IbisIpSubscriberOnePublisher::allConnects()
{
    qDebug()<<Q_FUNC_INFO;

    connect(&timerHeartbeatCheck, &QTimer::timeout, this, &IbisIpSubscriberOnePublisher::slotHeartbeatTimeout);
    connect(&httpServerSubscriber ,&HttpServerSubscriber::signalDataReceived,this,&IbisIpSubscriberOnePublisher::slotHandleReceivedData) ;
}


void IbisIpSubscriberOnePublisher::allConnects2()
{
    qDebug()<<Q_FUNC_INFO;

    connect(&zeroConf, &QZeroConf::serviceAdded, this, &IbisIpSubscriberOnePublisher::slotAddService);
    connect(&zeroConf, &QZeroConf::serviceUpdated, this, &IbisIpSubscriberOnePublisher::slotUpdateService);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &IbisIpSubscriberOnePublisher::slotServiceRemoved);
}

void IbisIpSubscriberOnePublisher::newSubscribeRequest()
{
    qDebug() <<  Q_FUNC_INFO;
    isSubscriptionActive=false;
    isCandidateSelected=false;


    findServices(mServiceType,1);
    checkExistingServices();
}

void IbisIpSubscriberOnePublisher::postSubscribe(QUrl subscriberAddress, QString postRequestContent)
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
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberOnePublisher::slotHttpRequestSubscriptionFinished);

}


void IbisIpSubscriberOnePublisher::postUnsubscribe(QUrl subscriberAddress, QString postRequestContent)
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
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberOnePublisher::slotHttpRequestUnsubscriptionFinished);

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
    qDebug() <<  Q_FUNC_INFO;

    slotAddService(zcs);
}

void IbisIpSubscriberOnePublisher::checkExistingServices()
{
    qDebug() <<  Q_FUNC_INFO;
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
    qDebug() <<  Q_FUNC_INFO;

    QString serviceName=zcs->name();
    QString ipAddress=zcs->ip().toString();
    QString version=zcs.data()->txt().value("ver");
    int portNumber=zcs->port();
    qDebug() <<"service name "<<serviceName<<" ip address "<<ipAddress<<" portNumber "<<QString::number(portNumber)<<" data" <<version;

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
                qDebug()<<"sending subscribe request to  "<<ipAddress<<":"<<QString::number(portNumber)<<" service "<<serviceName;

                QString addressAfterBackslash="/"+mServiceName+"/Subscribe"+mStructureName;
                QString addressComplete="http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+addressAfterBackslash;
                qDebug()<<"adresaCile string "<<addressComplete;
                QUrl subscriptionDestination=QUrl(addressComplete);
                isCandidateSelected=true;
                subscribeServiceCandidate=zcs;            

                deviceAddress=selectNonLoopbackAddressInSubnet(zcs->ip(),mSubnetMask);

                if(deviceAddress.toString().isEmpty())
                {
                    emit signalError("couldn't find device address!");
                }



                postSubscribe(subscriptionDestination,xmlGeneratorSubscriber.createSubscribeRequest(deviceAddress,httpServerSubscriber.portNumber(),mReplyPath));

            }
            else
            {
                qDebug()<<"isSubscriptionActive "<<isSubscriptionActive;
            }
        }
        else
        {
            qDebug()<<"isCandidateSelected "<<isCandidateSelected;
        }
    }
    else
    {
        qDebug()<<"service is not the requested one";
    }
}



void IbisIpSubscriberOnePublisher::slotAddServiceManual(QString serviceName, QString version, QString ipAddress, int portNumber)
{
    qDebug() <<  Q_FUNC_INFO;

    qDebug() <<"service name "<<serviceName<<" ip address "<<ipAddress<<" portNumber "<<QString::number(portNumber)<<" data" <<version;


    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,serviceName,version))
    {
        if(this->isCandidateSelected==false)
        {
            if(this->isSubscriptionActive==false)
            {
                qDebug()<<"sending subscribe request to  "<<ipAddress<<":"<<QString::number(portNumber)<<" service "<<serviceName;

                QString addressAfterBackslash="/"+mServiceName+"/Subscribe"+mStructureName;
                QString addressComplete="http://"+ipAddress+":"+QString::number( portNumber)+addressAfterBackslash;
                qDebug()<<"adresaCile string "<<addressComplete;
                QUrl subscriptionDestination=QUrl(addressComplete);
                isCandidateSelected=true;


                deviceAddress=selectNonLoopbackAddressInSubnet(QHostAddress(ipAddress),mSubnetMask);
                postSubscribe(subscriptionDestination,xmlGeneratorSubscriber.createSubscribeRequest(deviceAddress,httpServerSubscriber.portNumber(),mReplyPath));

            }
            else
            {
                qDebug()<<"isSubscriptionActive "<<isSubscriptionActive;
            }
        }
        else
        {
            qDebug()<<"isCandidateSelected "<<isCandidateSelected;
        }

    }
    else
    {
        qDebug()<<"service is not the requested one";
    }

}


void IbisIpSubscriberOnePublisher::slotAddServiceManualForce(QString serviceName, QString version, QString ipAddress,  int portNumber)
{
    qDebug() <<  Q_FUNC_INFO;

    qDebug() <<"service name "<<serviceName<<" ip address "<<ipAddress<<" portNumber "<<QString::number(portNumber)<<" data" <<version;

    emit signalUpdateDeviceList();

    if (isTheServiceRequestedOne(mServiceName,mVersion,serviceName,version))
    {
        if(this->isCandidateSelected==false)
        {
            if(this->isSubscriptionActive==false)
            {
                qDebug()<<"sending subscribe request to  "<<ipAddress<<":"<<QString::number(portNumber)<<" service "<<serviceName;

                QString addressAfterBackslash="/"+mServiceName+"/Subscribe"+mStructureName;
                QString addressComplete="http://"+ipAddress+":"+QString::number( portNumber)+addressAfterBackslash;
                qDebug()<<"adresaCile string "<<addressComplete;
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
                qDebug()<<"isSubscriptionActive "<<isSubscriptionActive;
            }
        }
        else
        {
            qDebug()<<"isCandidateSelected "<<isCandidateSelected;
        }

    }
    else
    {
        qDebug()<<"service is not the requested one";
    }
}

void IbisIpSubscriberOnePublisher::slotHeartbeatTimeout()
{
    qDebug() <<  Q_FUNC_INFO;
    if(!isIpSet() )
    {
        deviceAddress=selectNonLoopbackAddress();
    }
    emit signalSubscriptionLost();

    newSubscribeRequest();
}

void IbisIpSubscriberOnePublisher::slotHttpRequestSubscriptionFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"subscribe response:"<<str;

    if(reply->error()!=QNetworkReply::NoError)
    {
        qDebug()<<reply->errorString();
        emit signalIsSubscriptionSuccesful(false);
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
        if((subscriptionResult=="true")||(subscriptionResult=="True"))
        {
            subscribedService=subscribeServiceCandidate;
            this->isSubscriptionActive=true;
            emit signalIsSubscriptionSuccesful(true);
            emit signalSubscriptionSuccessful(subscribedService);
        }
        else
        {
            qDebug()<<"subscription failed";
            emit signalIsSubscriptionSuccesful(false);
            emit signalError(qDomResponse.toString());
        }
    }
    else
    {
        emit signalIsSubscriptionSuccesful(false);
    }

    reply->deleteLater();
}


void IbisIpSubscriberOnePublisher::slotHttpRequestUnsubscriptionFinished()
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
            this->isSubscriptionActive=false;
            emit signalIsUnsubscriptionSuccesful(true);
            emit signalUnsubscriptionSuccessful(subscribedService);
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
}


void IbisIpSubscriberOnePublisher::slotSubscribeSent(QNetworkReply *subscriptionReply)
{
    qDebug() <<  Q_FUNC_INFO;
    QByteArray replyContent = subscriptionReply->readAll();
    QString replyString(replyContent);
    qDebug()<<"odpoved na subscribe:"<<replyString;

    subscribedService=subscribeServiceCandidate;

    //check if subscribe is successful should be here!

    this->isSubscriptionActive=true;

}

void IbisIpSubscriberOnePublisher::slotServiceRemoved(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;
    deleteServiceFromList(serviceList,zcs);
    if(zcs==subscribedService)
    {
        emit signalSubscriptionLost();
        newSubscribeRequest();
    }
}




void IbisIpSubscriberOnePublisher::slotHandleReceivedData(QString receivedData)
{
    qDebug() <<  Q_FUNC_INFO;
    // QByteArray posledniRequest=InstanceNovehoServeru.bodyPozadavku;
    QDomDocument xmlrequest;
    xmlrequest.setContent(receivedData);
    timerHeartbeatCheck.start(heartbeatCheckInterval);
    emit signalDataReceived(receivedData);
}


void IbisIpSubscriberOnePublisher::unsubscribe()
{

    qDebug() <<  Q_FUNC_INFO;
    if(!subscribedService.isNull())
    {
        QString addressAfterBackslash="/"+mServiceName+"/Unsubscribe"+mStructureName;
        QString addressComplete="http://"+subscribedService->ip().toString()+":"+QString::number(subscribedService->port())+addressAfterBackslash;
        qDebug()<<"adresaCile string "<<addressComplete;
        QUrl subscriptionDestination=QUrl(addressComplete);


        postUnsubscribe(subscriptionDestination,xmlGeneratorSubscriber.createUnsubscribeRequest(deviceAddress,httpServerSubscriber.portNumber(),mReplyPath));

    }
    else
    {
        qDebug()<<"no subscribed service";
        emit signalError("subscribed service is null");
    }
    isSubscriptionActive=false;
    isCandidateSelected=false;

}




