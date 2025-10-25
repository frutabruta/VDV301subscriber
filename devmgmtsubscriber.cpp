#include "devmgmtsubscriber.h"


DevMgmtSubscriber::DevMgmtSubscriber(QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber) : IbisIpSubscriberMultiplePublishers(serviceName,  structureName,  version,  serviceType, portNumber)
{
    qDebug()<<Q_FUNC_INFO;

    //allConnects();

}


DevMgmtSubscriber::~DevMgmtSubscriber()
{
    qDebug()<<Q_FUNC_INFO;
    zeroConf.disconnect();//reseni erroru ASSERT failure in DevMgmtSubscriber: "Called object is not of the correct type
    //  this->disconnect();
}

void DevMgmtSubscriber::start()
{
    findServices(mServiceType,1);
    IbisIpSubscriber::start();
}


bool DevMgmtSubscriber::getDeviceInformation(DevMgmtPublisherStruct &device)
{

    qDebug()<<Q_FUNC_INFO;

    //   QString adresa=zarizeni.adresa.toString()+":"+QString::number(zarizeni.port)+
    QString adresa = "http://"+device.hostAddress.toString()+":"+QString::number(device.portNumber)+"/DeviceManagementService/GetDeviceInformation";
    qDebug()<<adresa;
    QNetworkRequest pozadavek;
    pozadavek.setUrl(QUrl(adresa));

    manager.get(pozadavek);

    return true;
}


bool DevMgmtSubscriber::getDeviceConfiguration(DevMgmtPublisherStruct &device)
{
    qDebug()<<Q_FUNC_INFO;

    //   QString adresa=zarizeni.adresa.toString()+":"+QString::number(zarizeni.port)+
    QString adresa = "http://"+device.hostAddress.toString()+":"+QString::number(device.portNumber)+"/DeviceManagementService/GetDeviceConfiguration";
    qDebug()<<adresa;
    manager.get(QNetworkRequest(QUrl(adresa)));

    return true;
}

QByteArray DevMgmtSubscriber::slotRequestReceived(QNetworkReply* reply)
{
    qDebug() <<  Q_FUNC_INFO;
    QByteArray rawData = reply->readAll();
    qDebug()<<"Tohle je reply Url: "<<reply->request().url();


    QString textData(rawData);
    //  qDebug() << textData;

    emit downloadFinished(textData);


    DevMgmtPublisherStruct devicePlaceholder;

    QDomDocument domDocument;
    domDocument.setContent(textData);

    //   zarizeni.adresa=QHostAddress("192.168.0.131");//reply->request().url()
    devicePlaceholder.hostAddress=QHostAddress( reply->request().url().host());
    devicePlaceholder.portNumber=reply->request().url().port();
    reply->deleteLater();

    int index=deviceListDetected.indexOf(devicePlaceholder);

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
        //  if(seznamZarizeniDetekce[index])
        if((!deviceListDetected[index].deviceId.isEmpty())&&(!deviceListDetected[index].deviceClass.isEmpty()))
        {
            DevMgmtPublisherStruct* noveZarizeni=&deviceListDetected[index];

            if(noveZarizeni->isInListByIdClass(deviceListConfigured))
            {
                noveZarizeni->hwConfig=true;
            }

        }


    }
    else
    {
        qDebug()<<"index je mimo rozsah";
    }

    emit signalUpdateDeviceList();


    return rawData;
}




void DevMgmtSubscriber::slotHandleData(QString input)
{
    qDebug()<<Q_FUNC_INFO;
    qDebug().noquote()<<input;
}



void DevMgmtSubscriber::slotNewDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    DevMgmtPublisherStruct newDevice(zcs);


    if(newDevice.serviceName.contains(mServiceName))
    {

        if(!deviceListDetected.contains(newDevice))
        {     
            getDeviceConfiguration(newDevice);
            getDeviceInformation(newDevice);

            deviceListDetected.push_back(newDevice);

        }
        else
        {
            qDebug()<<"device is already on the list";
        }
    }
    else
    {
        qDebug()<<"jina sluzba";
    }

    emit signalUpdateDeviceList();
}



void DevMgmtSubscriber::slotUpdateDeviceInfo()
{
    qDebug() <<  Q_FUNC_INFO;
    foreach(DevMgmtPublisherStruct polozka, deviceListDetected)
    {
        getDeviceConfiguration(polozka);
        getDeviceInformation(polozka);

    }
    emit signalUpdateDeviceList();
}

void DevMgmtSubscriber::slotRemoveDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    DevMgmtPublisherStruct selectedDevice(zcs);
    selectedDevice.serviceName=zcs->name();

    if(selectedDevice.serviceName.contains(mServiceName))
    {

        if(deviceListDetected.contains(selectedDevice))
        {
            deviceListDetected.remove(deviceListDetected.indexOf(selectedDevice) );
        }
        else
        {
            qDebug()<<"device was not present on the list";
        }
    }




    emit signalUpdateDeviceList();
}


void DevMgmtSubscriber::postSetDeviceConfiguration(QUrl subscriberAddress, QString deviceId)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomDocument xmlDocument;
    QString postRequestContent=xmlGeneratorSubscriber.setDeviceConfigurationRequest(xmlDocument,deviceId).toString();
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
    connect(reply, &QNetworkReply::finished, this, &DevMgmtSubscriber::slotSetSetDeviceConfigurationFinished);

}

void DevMgmtSubscriber::slotSetSetDeviceConfigurationFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"subscribe response:"<<str;

    if(reply->error()!=QNetworkReply::NoError)
    {
        qDebug()<<reply->errorString();
      //  emit signalIsSubscriptionSuccesful2(false);
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

      //      emit signalIsSubscriptionSuccesful2(true);

        }
        else
        {
            qDebug()<<"unsubscription failed";
      //      emit signalIsSubscriptionSuccesful2(false);
        }
    }
    else
    {
     //   emit signalIsSubscriptionSuccesful2(false);
    }


    reply->deleteLater();
    //reply = nullptr;
}
