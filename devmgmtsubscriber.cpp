#include "devmgmtsubscriber.h"


DevMgmtSubscriber::DevMgmtSubscriber(QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber) : IbisIpSubscriber(serviceName,  structureName,  version,  serviceType, portNumber)
{
    qDebug()<<Q_FUNC_INFO;

    //allConnects();
    findServices(mServiceType,1);
    start();
}

DevMgmtSubscriber::~DevMgmtSubscriber()
{
    qDebug()<<Q_FUNC_INFO;
    zeroConf.disconnect();//reseni erroru ASSERT failure in DevMgmtSubscriber: "Called object is not of the correct type
    //  this->disconnect();
}



//unused
bool DevMgmtSubscriber::getDeviceInformation(QZeroConfService zcs)
{

    qDebug()<<Q_FUNC_INFO;

    //   QString adresa=zarizeni.adresa.toString()+":"+QString::number(zarizeni.port)+
    QString adresa = "http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+"/DeviceManagementService/GetDeviceInformation";
    qDebug()<<adresa;
    QNetworkRequest pozadavek;
    pozadavek.setUrl(QUrl(adresa));

    manager.get(pozadavek);

    return true;
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


bool DevMgmtSubscriber::getDeviceConfiguration(QZeroConfService zcs)
{
    qDebug()<<Q_FUNC_INFO;

    //   QString adresa=zarizeni.adresa.toString()+":"+QString::number(zarizeni.port)+
    QString adresa = "http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+"/DeviceManagementService/GetDeviceConfiguration";
    qDebug()<<adresa;
    manager.get(QNetworkRequest(QUrl(adresa)));

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
            deviceConfigurationToDevice(domDocument,devicePlaceholder);
         }

        else if(firstNodeName=="DeviceManagementService.GetDeviceInformationResponse")
        {   
            deviceInformationToDevice(domDocument,devicePlaceholder);
        }

        else if(firstNodeName=="DeviceManagementService.GetDeviceStatusResponse")
        {
            deviceStatusToDevice(domDocument,devicePlaceholder);
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


void DevMgmtSubscriber::deviceInformationToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device)
{
    device.deviceClass=domDocument.elementsByTagName("DeviceClass").at(0).firstChild().nodeValue();
    device.deviceName=domDocument.elementsByTagName("DeviceName").at(0).firstChildElement("Value").firstChild().nodeValue();
    device.manufacturer=domDocument.elementsByTagName("Manufacturer").at(0).firstChildElement("Value").firstChild().nodeValue();
    device.serialNumber=domDocument.elementsByTagName("SerialNumber").at(0).firstChildElement("Value").firstChild().nodeValue();
    device.swVersion=getVersion(domDocument,"SwVersion");
}

void DevMgmtSubscriber::deviceConfigurationToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device)
{
     device.deviceId=domDocument.elementsByTagName("DeviceID").at(0).firstChildElement("Value").firstChild().nodeValue();
}


void DevMgmtSubscriber::deviceStatusToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device)
{
    device.status=domDocument.elementsByTagName("DeviceState").at(0).firstChild().nodeValue();

}

void DevMgmtSubscriber::slotHandleData(QString input)
{
    qDebug()<<Q_FUNC_INFO;
    qDebug().noquote()<<input;
}


QString DevMgmtSubscriber::getVersion(QDomDocument document, QString element)
{
    QString vysledek="";
    QDomNodeList seznamElementu = document.elementsByTagName("DataVersion");

    for (int i=0;i<seznamElementu.count();i++)
    {
        QString tagName=seznamElementu.at(i).toElement().firstChildElement("DataType").firstChildElement("Value").firstChild().nodeValue();
        if(tagName==element)
        {
            vysledek=seznamElementu.at(i).toElement().firstChildElement("VersionRef").firstChildElement("Value").firstChild().nodeValue();
            return vysledek;
        }


    }

    return vysledek;
}



void DevMgmtSubscriber::slotNewDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    DevMgmtPublisherStruct noveZarizeni;
    noveZarizeni.serviceName=zcs->name();



    if(noveZarizeni.serviceName.contains("DeviceManagementService"))
    {
        qDebug()<<"DP1";
        noveZarizeni.hostAddress=zcs->ip();
        noveZarizeni.portNumber=zcs->port();
        noveZarizeni.deviceClass="";
        noveZarizeni.deviceId="";
        noveZarizeni.hostname=zcs->host();
        noveZarizeni.ibisIpVersion=zcs.data()->txt().value("ver");



        if(!deviceListDetected.contains(noveZarizeni))
        {
            qDebug()<<"DP2";
            getDeviceConfiguration(noveZarizeni);
            getDeviceInformation(noveZarizeni);

            qDebug()<<"DP5";
            deviceListDetected.push_back(noveZarizeni);

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

    DevMgmtPublisherStruct selectedDevice;
    selectedDevice.serviceName=zcs->name();

    if(selectedDevice.serviceName.contains("DeviceManagementService"))
    {
        selectedDevice.hostAddress=zcs->ip();
        selectedDevice.portNumber=zcs->port();
        selectedDevice.deviceClass="";
        selectedDevice.deviceId="";
        // noveZarizeni.deviceName=zcs->host();

        selectedDevice.hostname=zcs->host();
        selectedDevice.ibisIpVersion=zcs.data()->txt().value("ver");

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
