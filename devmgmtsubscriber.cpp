#include "devmgmtsubscriber.h"


DevMgmtSubscriber::DevMgmtSubscriber(QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber) : IbisIpSubscriber(serviceName,  structureName,  version,  serviceType, portNumber)
{
    qDebug()<<Q_FUNC_INFO;
    allConnects();
    findServices(mServiceType,1);
}

DevMgmtSubscriber::~DevMgmtSubscriber()
{
    qDebug()<<Q_FUNC_INFO;
    zeroConf.disconnect();//reseni erroru ASSERT failure in DevMgmtSubscriber: "Called object is not of the correct type
    //  this->disconnect();
}

void DevMgmtSubscriber::allConnects()
{
    qDebug()<<Q_FUNC_INFO;
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &DevMgmtSubscriber::slotNewDnsSd);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &DevMgmtSubscriber::slotRemoveDnsSd);
    connect(&manager,&QNetworkAccessManager::finished,this,&DevMgmtSubscriber::slotRequestReceived);
    connect(this,&DevMgmtSubscriber::downloadFinished,this,&DevMgmtSubscriber::slotHandleData);

}


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


    DevMgmtPublisherStruct zarizeni;

    QDomDocument dokument;
    dokument.setContent(textData);

    //   zarizeni.adresa=QHostAddress("192.168.0.131");//reply->request().url()
    zarizeni.hostAddress=QHostAddress( reply->request().url().host());
    zarizeni.portNumber=reply->request().url().port();


    int index=deviceListDetected.indexOf(zarizeni);
    if((index>=0)&&(index<deviceListDetected.count()))
    {

        if(dokument.firstChildElement().nodeName()=="DeviceManagementService.GetDeviceConfigurationResponse")
        {
            qDebug()<<"ano, hledam Configuration";
            zarizeni.deviceId=dokument.elementsByTagName("DeviceID").at(0).firstChildElement("Value").firstChild().nodeValue();
            deviceListDetected[index].deviceId=zarizeni.deviceId;
            qDebug()<<"device id je: "<< zarizeni.deviceId<<" index na seznamu"<<index<<" adresa:"<<zarizeni.hostAddress<<" port:"<<zarizeni.portNumber;
        }

        if(dokument.firstChildElement().nodeName()=="DeviceManagementService.GetDeviceInformationResponse")
        {
            qDebug()<<"ano, hledam Information ";

            zarizeni.deviceClass=dokument.elementsByTagName("DeviceClass").at(0).firstChild().nodeValue();
            zarizeni.deviceName=dokument.elementsByTagName("DeviceName").at(0).firstChildElement("Value").firstChild().nodeValue();
            zarizeni.manufacturer=dokument.elementsByTagName("Manufacturer").at(0).firstChildElement("Value").firstChild().nodeValue();
            zarizeni.serialNumber=dokument.elementsByTagName("SerialNumber").at(0).firstChildElement("Value").firstChild().nodeValue();
            zarizeni.swVersion=getVersion(dokument,"SwVersion");

            deviceListDetected[index].deviceClass=zarizeni.deviceClass;
            deviceListDetected[index].deviceName=zarizeni.deviceName;
            deviceListDetected[index].manufacturer=zarizeni.manufacturer;
            deviceListDetected[index].serialNumber=zarizeni.serialNumber;
            deviceListDetected[index].swVersion=zarizeni.swVersion;
            qDebug()<<"device class je: "<< zarizeni.deviceClass<<" verze: "<<zarizeni.swVersion<<" index na seznamu"<<index<<" adresa:"<<zarizeni.hostAddress<<" port:"<<zarizeni.portNumber;

        }


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
    foreach(auto polozka, deviceListDetected)
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
