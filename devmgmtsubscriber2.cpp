#include "devmgmtsubscriber2.h"



//DevMgmtSubscriber::DevMgmtSubscriber(QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber) : IbisIpSubscriber(serviceName,  structureName,  version,  serviceType, portNumber)


DevMgmtSubscriber2::DevMgmtSubscriber2(QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber) : DevMgmtSubscriber(serviceName,structureName, version,serviceType, portNumber)
{
    qDebug()<<Q_FUNC_INFO;
    allConnects();
}

void DevMgmtSubscriber2::allConnects()
{
    qDebug()<<Q_FUNC_INFO;
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &DevMgmtSubscriber2::slotNewDnsSd);
    // connect(&zeroConf, &QZeroConf::serviceUpdated, this, &DevMgmtSubscriber::slotNewDnsSd);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &DevMgmtSubscriber::slotRemoveDnsSd);
    connect(&manager,&QNetworkAccessManager::finished,this,&DevMgmtSubscriber::slotRequestReceived);
    connect(this,&DevMgmtSubscriber::downloadFinished,this,&DevMgmtSubscriber::slotHandleData);
    connect(&httpServerSubscriber ,&HttpServerSubscriber::signalDataReceived,this,&DevMgmtSubscriber2::slotHandleReceivedData) ;
    connect(&httpServerSubscriber ,&HttpServerSubscriber::signalWholeRequest,this,&DevMgmtSubscriber2::slotHandleWholeRequest) ;

}

void DevMgmtSubscriber2::slotNewDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    DevMgmtPublisherStruct newDevice;
    newDevice.serviceName=zcs->name();

    //   if(newDevice.serviceName.contains("DeviceManagementService"))
    if(isTheServiceRequestedOne(mServiceName,mVersion,zcs))
    {
        qDebug()<<"DP1";
        newDevice.hostAddress=zcs->ip();
        newDevice.portNumber=zcs->port();
        newDevice.deviceClass="";
        newDevice.deviceId="";
        newDevice.hostname=zcs->host();
        newDevice.ibisIpVersion=zcs.data()->txt().value("ver");


        if(!deviceListDetected.contains(newDevice))
        {
            qDebug()<<"DP2";
            //subscribe here
            getDeviceConfiguration(newDevice);
            getDeviceInformation(newDevice);

            qDebug()<<"DP5";

            qDebug()<<"sending subscribe request to  "<<newDevice.hostAddress<<":"<<QString::number(newDevice.portNumber)<<" service "<<zcs->name();

            QString addressAfterBackslash="/"+mServiceName+"/Subscribe"+mStructureName;
            QString addressComplete="http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+addressAfterBackslash;
            qDebug()<<"adresaCile string "<<addressComplete;
            QUrl subscriptionDestination=QUrl(addressComplete);
            if(!isIpSet() )
            {
                deviceAddress=selectNonLoopbackAddress();
            }
            postSubscribe(subscriptionDestination,xmlGeneratorSubscriber.createSubscribeRequest(deviceAddress,httpServerSubscriber.portNumber()));
            deviceListDetected.push_back(newDevice);

        }
        else
        {
            qDebug()<<"zarizeni uz je na seznamu";
        }
    }
    else
    {
        qDebug()<<"jina sluzba";
    }

    emit signalUpdateDeviceList();
}


void DevMgmtSubscriber2::postSubscribe(QUrl subscriberAddress, QString postRequestContent)
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
    connect(reply, &QNetworkReply::finished, this, &DevMgmtSubscriber2::slotHttpRequestSubscriptionFinished);

}


void DevMgmtSubscriber2::postUnsubscribe(QUrl subscriberAddress, QString postRequestContent)
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
    connect(reply, &QNetworkReply::finished, this, &DevMgmtSubscriber2::slotHttpRequestUnsubscriptionFinished);

}


//void IbisIpSubscriberOnePublisher::slotHttpRequestSubscriptionFinished()
void DevMgmtSubscriber2::slotHttpRequestSubscriptionFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"subscribe response:"<<str;

    if(reply->error()!=QNetworkReply::NoError)
    {
        qDebug()<<reply->errorString();
        emit signalIsSubscriptionSuccessful2(false);
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
            // subscribedService=subscribeServiceCandidate;
            //   this->isSubscriptionActive=true;
            emit signalIsSubscriptionSuccessful2(true);
            //  emit signalSubscriptionSuccessful(subscribedService);
        }
        else
        {
            qDebug()<<"unsubscription failed";
            emit signalIsSubscriptionSuccessful2(false);
        }
    }
    else
    {
        emit signalIsSubscriptionSuccessful2(false);
    }


    reply->deleteLater();
    //reply = nullptr;
}


// void IbisIpSubscriberOnePublisher::slotHttpRequestUnsubscriptionFinished()
void DevMgmtSubscriber2::slotHttpRequestUnsubscriptionFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"unsusbscription response:";
    qDebug().noquote()<<str;

    if(reply->error()!=QNetworkReply::NoError)
    {
        qDebug()<<reply->errorString();
        emit signalIsUnsubscriptionSuccessful(false);
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
            emit signalIsUnsubscriptionSuccessful(true);
            //    emit signalUnsubscriptionSuccessful(subscribedService);
        }
        else
        {
            qDebug()<<"unsubscription failed";
            emit signalIsUnsubscriptionSuccessful(false);
        }
    }
    else
    {
        emit signalIsUnsubscriptionSuccessful(false);
    }

    reply->deleteLater();
}

void DevMgmtSubscriber2::slotHandleReceivedData(QString receivedData)
{
    qDebug() <<  Q_FUNC_INFO;
    // QByteArray posledniRequest=InstanceNovehoServeru.bodyPozadavku;
    QDomDocument xmlrequest;
    xmlrequest.setContent(receivedData);
    //   timerHeartbeatCheck.start(heartbeatCheckInterval);

    emit signalDataReceived(receivedData);
}


void DevMgmtSubscriber2::slotHandleWholeRequest(HttpServerRequest receivedData)
{
    qDebug() <<  Q_FUNC_INFO;
    qDebug()<<"received from: "<<receivedData.hostAddress<<" "<<receivedData.port;
    qDebug()<<"received data:\n"<<receivedData.body;

    DevMgmtPublisherStruct devicePlaceholder;
    devicePlaceholder.hostAddress=QHostAddress(receivedData.hostAddress.toIPv4Address());
    devicePlaceholder.portNumber=receivedData.port;

   // int protocol=receivedData.hostAddress.protocol();

    QDomDocument domDocument;
    domDocument.setContent(receivedData.body);


   // int index=deviceListDetected.indexOf(devicePlaceholder);
    int index=indexOfSubscriberOnListByIp(deviceListDetected,devicePlaceholder.hostAddress);

    if((index>=0)&&(index<deviceListDetected.count()))
    {
        devicePlaceholder=deviceListDetected[index];

        QString firstNodeName=domDocument.firstChildElement().nodeName();
        if(firstNodeName=="DeviceManagementService.GetDeviceConfigurationResponse")
        {
            xmlParserSubscriber.deviceConfigurationToDevice(domDocument,devicePlaceholder);
        }

        else if(firstNodeName=="DeviceManagementService.GetDeviceInformationResponse")
        {
            xmlParserSubscriber.deviceInformationToDevice(domDocument,devicePlaceholder);

        }
        else if(firstNodeName=="DeviceManagementService.GetDeviceStatusResponse")
        {
            xmlParserSubscriber.deviceStatusToDevice(domDocument,devicePlaceholder);
        }

        deviceListDetected[index]=devicePlaceholder;
    }



     emit signalUpdateDeviceList();
}


int DevMgmtSubscriber2::indexOfSubscriberOnListByIp(QVector<DevMgmtPublisherStruct> deviceList, QHostAddress hostAddress)
{

    for(int i=0;i<deviceList.length();i++)
    {
        DevMgmtPublisherStruct selectedDevice=deviceList.at(i);
        qDebug()<<"list "<<selectedDevice.hostAddress<<" tested:"<<hostAddress;
        if(selectedDevice.hostAddress==hostAddress)
        {

            return i;
        }
    }
    return -1;
}
