#include "ibisipsubscriberonepublisher.h"

IbisIpSubscriberOnePublisher::IbisIpSubscriberOnePublisher(QString serviceName,QString structureName,QString version,QString serviceType, int portName) : IbisIpSubscriber(serviceName,structureName, version, serviceType, portName)
{
    qDebug()<<Q_FUNC_INFO;

   // vsechnyConnecty();


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
}



void IbisIpSubscriberOnePublisher::postSubscribe(QUrl subscriberAddress, QString postRequestContent)
{

    qDebug() <<  Q_FUNC_INFO;
    qDebug()<<"posting to address: "<<subscriberAddress<<" "<<postRequestContent;

    QNetworkRequest postRequest(subscriberAddress);
    qDebug()<<"A";

    // https://stackoverflow.com/a/53556560

    postRequest.setTransferTimeout(30000);
    postRequest.setRawHeader("Content-Type", "text/xml");
    //postRequest.setRawHeader("Expect", "100-continue");
    //postRequest.setRawHeader("Connection", "keep-Alive");


    //postRequest.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray postRequestContentQByteArray=postRequestContent.toUtf8() ;

    qDebug()<<"B";

    reply=postManager.post(postRequest,postRequestContentQByteArray);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberOnePublisher::slotHttpFinished);


    qDebug()<<"C";

}


void IbisIpSubscriberOnePublisher::slotUpdateService(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;
    if(!serviceList.contains(zcs))
    {
        slotAddService(zcs);
    }
    else
    {
        qDebug()<<"service is already on the list";
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

    serviceList.append(zcs);
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
                if(!isIpSet() )
                {
                    deviceAddress=selectNonLoopbackAddress();
                }
                postSubscribe(subscriptionDestination,this->createSubscribeRequest(deviceAddress,mPortNumber));

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

void IbisIpSubscriberOnePublisher::slotHttpFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"odpoved na subscribe:"<<str;

    subscribedService=subscribeServiceCandidate;

    this->isSubscriptionActive=true;
    reply->deleteLater();
    //reply = nullptr;
    emit signalSubscriptionSuccessful(subscribedService);
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



