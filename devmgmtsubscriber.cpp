#include "devmgmtsubscriber.h"


DevMgmtSubscriber::DevMgmtSubscriber(QString nazevSluzby, QString struktura, QString  verze, QString  typSluzby, int cisloPortu) : IbisIpSubscriber(nazevSluzby,  struktura,  verze,  typSluzby, cisloPortu)
{
    qDebug()<<Q_FUNC_INFO;
    vsechnyConnecty();

    hledejSluzby(typSluzbyInterni,1);
}

void DevMgmtSubscriber::vsechnyConnecty()
{
    qDebug()<<Q_FUNC_INFO;
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &DevMgmtSubscriber::slotNewDnsSd);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &DevMgmtSubscriber::slotRemoveDnsSd);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(slotRequestReceived(QNetworkReply*)));
    connect(this,&DevMgmtSubscriber::stazeniHotovo,this,&DevMgmtSubscriber::slotZpracujData);

}


bool DevMgmtSubscriber::getDeviceInformation(DevMgmtPublisherStruct &zarizeni, QZeroConfService zcs)
{

    qDebug()<<Q_FUNC_INFO;

    //   QString adresa=zarizeni.adresa.toString()+":"+QString::number(zarizeni.port)+
    QString adresa = "http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+"/DeviceManagementService/GetDeviceInformation";
    qDebug()<<adresa;
    QNetworkRequest pozadavek;
    pozadavek.setUrl(QUrl(adresa));

    manager->get(pozadavek);

    return true;
}

bool DevMgmtSubscriber::getDeviceConfiguration(DevMgmtPublisherStruct &zarizeni, QZeroConfService zcs)
{
    qDebug()<<Q_FUNC_INFO;

    //   QString adresa=zarizeni.adresa.toString()+":"+QString::number(zarizeni.port)+
    QString adresa = "http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+"/DeviceManagementService/GetDeviceConfiguration";
    qDebug()<<adresa;
    manager->get(QNetworkRequest(QUrl(adresa)));

    return true;
}

QByteArray DevMgmtSubscriber::slotRequestReceived(QNetworkReply* reply)
{
    qDebug() <<  Q_FUNC_INFO;
    QByteArray rawData = reply->readAll();
    qDebug()<<"Tohle je reply Url: "<<reply->request().url();


    QString textData(rawData);
    //  qDebug() << textData;

    emit stazeniHotovo(textData);


    DevMgmtPublisherStruct zarizeni;

    QDomDocument dokument;
    dokument.setContent(textData);

    //   zarizeni.adresa=QHostAddress("192.168.0.131");//reply->request().url()
    zarizeni.adresa=QHostAddress( reply->request().url().host());
    zarizeni.port=reply->request().url().port();




    int index=seznamZarizeni.indexOf(zarizeni);
    if((index>=0)&&(index<seznamZarizeni.count()))
    {
        //  zarizeni.hostname=seznamZarizeni.at(index).hostname;
        //   zarizeni.serviceName=seznamZarizeni.at(index).serviceName;
        //   zarizeni.ibisIpVersion=seznamZarizeni.at(index).ibisIpVersion;
        //   seznamZarizeni.replace(index, zarizeni);

        if(dokument.firstChildElement().nodeName()=="DeviceManagementService.GetDeviceConfigurationResponse")
        {
            qDebug()<<"ano, hledam Configuration";
            zarizeni.deviceId=dokument.elementsByTagName("DeviceID").at(0).firstChildElement("Value").firstChild().nodeValue();
            seznamZarizeni[index].deviceId=zarizeni.deviceId;
            qDebug()<<"device id je: "<< zarizeni.deviceId<<" index na seznamu"<<index<<" adresa:"<<zarizeni.adresa<<" port:"<<zarizeni.port;

        }

        if(dokument.firstChildElement().nodeName()=="DeviceManagementService.GetDeviceInformationResponse")
        {
            qDebug()<<"ano, hledam Information ";

            zarizeni.deviceClass=dokument.elementsByTagName("DeviceClass").at(0).firstChildElement("Value").firstChild().nodeValue();
            zarizeni.deviceName=dokument.elementsByTagName("DeviceName").at(0).firstChildElement("Value").firstChild().nodeValue();
            zarizeni.manufacturer=dokument.elementsByTagName("Manufacturer").at(0).firstChildElement("Value").firstChild().nodeValue();
            zarizeni.serialNumber=dokument.elementsByTagName("SerialNumber").at(0).firstChildElement("Value").firstChild().nodeValue();
            zarizeni.swVersion=getVersion(dokument,"SwVersion");

            seznamZarizeni[index].deviceClass=zarizeni.deviceClass;
            seznamZarizeni[index].deviceName=zarizeni.deviceName;
            seznamZarizeni[index].manufacturer=zarizeni.manufacturer;
            seznamZarizeni[index].serialNumber=zarizeni.serialNumber;
            seznamZarizeni[index].swVersion=zarizeni.swVersion;
            qDebug()<<"device class je: "<< zarizeni.deviceClass<<" verze: "<<zarizeni.swVersion<<" index na seznamu"<<index<<" adresa:"<<zarizeni.adresa<<" port:"<<zarizeni.port;

        }


    }
    else
    {
        qDebug()<<"index je mimo rozsah";
    }

    emit signalAktualizaceSeznamu();


    return rawData;
}

void DevMgmtSubscriber::slotZpracujData(QString vstup)
{
    qDebug()<<Q_FUNC_INFO;
    qDebug().noquote()<<vstup;
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






/*
void IbisIpSubscriberOnePublisher::slotAddService(QZeroConfService zcs)
{

    qDebug() <<  Q_FUNC_INFO;
    // QTableWidgetItem *cell;
    // qDebug() << "Added service: " << zcs;
    QString nazev=zcs->name();
    QString ipadresa=zcs->ip().toString();
    QString verze=zcs.data()->txt().value("ver");
    int port=zcs->port();
    qDebug() <<"nazev sluzby "<<nazev<<" ip adresa "<<ipadresa<<" port "<<QString::number(port)<<" data" <<verze;

    seznamSluzeb.append(zcs);
    emit aktualizaceSeznamu();

    if (jeSluzbaHledanaVerze(nazevSluzbyInterni,verzeInterni,zcs)&&(this->existujeKandidat==false)&&(this->odebirano==false))
    {
        qDebug()<<"odesilam subscribe na "<<ipadresa<<":"<<QString::number(port)<<" sluzba "<<nazev;

        QString adresaZaLomitkem="/"+nazevSluzbyInterni+"/Subscribe"+strukturaInterni;
        QString adresaCileString="http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+adresaZaLomitkem;
        qDebug()<<"adresaCile string "<<adresaCileString;
        QUrl adresaKamPostovatSubscribe=QUrl(adresaCileString);
        existujeKandidat=true;
        kandidatSluzbaMdns=zcs;
        PostSubscribe(adresaKamPostovatSubscribe,this->vytvorSubscribeRequest(adresaZarizeni,cisloPortuInterni));

    }

   // emit nalezenaSluzba( zcs);

}

*/

void DevMgmtSubscriber::slotNewDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    DevMgmtPublisherStruct noveZarizeni;
    noveZarizeni.serviceName=zcs->name();


    if(noveZarizeni.serviceName.contains("DeviceManagementService"))
    {
        noveZarizeni.adresa=zcs->ip();
        noveZarizeni.port=zcs->port();
        noveZarizeni.deviceClass="";
        noveZarizeni.deviceId="";
        noveZarizeni.hostname=zcs->host();
        noveZarizeni.ibisIpVersion=zcs.data()->txt().value("ver");

        if(!seznamZarizeni.contains(noveZarizeni))
        {
            getDeviceConfiguration(noveZarizeni,zcs);
            getDeviceInformation(noveZarizeni,zcs);
            seznamZarizeni.push_back(noveZarizeni);
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





    emit signalAktualizaceSeznamu();
}

void DevMgmtSubscriber::slotRemoveDnsSd(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;

    DevMgmtPublisherStruct noveZarizeni;
    noveZarizeni.serviceName=zcs->name();

    if(noveZarizeni.serviceName.contains("DeviceManagementService"))
    {
        noveZarizeni.adresa=zcs->ip();
        noveZarizeni.port=zcs->port();
        noveZarizeni.deviceClass="";
        noveZarizeni.deviceId="";
        // noveZarizeni.deviceName=zcs->host();

        noveZarizeni.hostname=zcs->host();
        noveZarizeni.ibisIpVersion=zcs.data()->txt().value("ver");

        if(seznamZarizeni.contains(noveZarizeni))
        {
            seznamZarizeni.remove(seznamZarizeni.indexOf(noveZarizeni) );
        }
        else
        {
            qDebug()<<"zarizeni nebylo na seznamu";
        }
    }




    emit signalAktualizaceSeznamu();
}
