#include "ibisipsubscriberonepublisher.h"

IbisIpSubscriberOnePublisher::IbisIpSubscriberOnePublisher(QString serviceName,QString structureName,QString version,QString serviceType, int portName) : IbisIpSubscriber(serviceName,structureName, version, serviceType, portName)
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
    //   connect(&zeroConf, &QZeroConf::serviceAdded, this, &IbisIpSubscriberOnePublisher::slotAddService);
    //   connect(&zeroConf, &QZeroConf::serviceRemoved, this, &IbisIpSubscriberOnePublisher::slotServiceRemoved);
}


void IbisIpSubscriberOnePublisher::allConnects2()
{
    qDebug()<<Q_FUNC_INFO;

    //  connect(&timerHeartbeatCheck, &QTimer::timeout, this, &IbisIpSubscriberOnePublisher::slotHeartbeatTimeout);

    // connect(&httpServerSubscriber ,&HttpServerSubscriber::signalDataReceived,this,&IbisIpSubscriberOnePublisher::slotHandleReceivedData) ;
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

    /*

    QString serviceName=zcs->name();
    QString ipAddress=zcs->ip().toString();
    QString version=zcs.data()->txt().value("ver");
    int portNumber=zcs->port();


    if(!serviceList.contains(zcs))
    {
        slotAddService(zcs);
    }
    else
    {
        qDebug()<<"service "<<serviceName<<" "<<version<<" "<<ipAddress<<":"<<QString::number(portNumber)<<" is already on the list";

        QZeroConfService zcs2=serviceList.at(serviceList.indexOf(zcs));
        QString serviceName2=zcs2->name();
        QString ipAddress2=zcs2->ip().toString();
        QString version2=zcs2.data()->txt().value("ver");
        int portNumber2=zcs2->port();
        qDebug()<<"service on list: "<<serviceName2<<" "<<version2<<" "<<ipAddress2<<":"<<QString::number(portNumber2)<<" is already on the list";

    }
*/
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
                /*
                if(!isIpSet() )
                {
                    //deviceAddress=selectNonLoopbackAddress();
                    deviceAddress=selectNonLoopbackAddressInSubnet(zcs->ip());
                }*/

                deviceAddress=selectNonLoopbackAddressInSubnet(zcs->ip(),mSubnetMask);
                postSubscribe(subscriptionDestination,xmlGeneratorSubscriber.createSubscribeRequest(deviceAddress,httpServerSubscriber.portNumber()));

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

    // emit nalezenaSluzba( zcs);

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
            qDebug()<<"unsubscription failed";
            emit signalIsSubscriptionSuccesful(false);
        }
    }
    else
    {
        emit signalIsSubscriptionSuccesful(false);
    }


    reply->deleteLater();
    //reply = nullptr;
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
        }
    }
    else
    {
        emit signalIsUnsubscriptionSuccesful(false);
    }



    reply->deleteLater();
    //reply = nullptr;


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
    QString addressAfterBackslash="/"+mServiceName+"/Unsubscribe"+mStructureName;
    QString addressComplete="http://"+subscribedService->ip().toString()+":"+QString::number(subscribedService->port())+addressAfterBackslash;
    qDebug()<<"adresaCile string "<<addressComplete;
    QUrl subscriptionDestination=QUrl(addressComplete);


    postUnsubscribe(subscriptionDestination,xmlGeneratorSubscriber.createUnsubscribeRequest(deviceAddress,httpServerSubscriber.portNumber()));
    isSubscriptionActive=false;
    isCandidateSelected=false;
}

