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
    qDebug() << textData;

    emit stazeniHotovo(textData);


    DevMgmtPublisherStruct zarizeni;

    QDomDocument dokument;
    dokument.setContent(textData);

    zarizeni.adresa=QHostAddress("192.168.0.131");//reply->request().url()
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
            seznamZarizeni[index].deviceId=dokument.elementsByTagName("DeviceID").at(0).firstChildElement("Value").firstChild().nodeValue();
        }

        if(dokument.firstChildElement().nodeName()=="DeviceManagementService.GetDeviceInformationResponse")
        {
            qDebug()<<"ano, hledam Information";
            seznamZarizeni[index].deviceClass=dokument.elementsByTagName("DeviceClass").at(0).firstChildElement("Value").firstChild().nodeValue();
            seznamZarizeni[index].deviceName=dokument.elementsByTagName("DeviceName").at(0).firstChildElement("Value").firstChild().nodeValue();
            seznamZarizeni[index].manufacturer=dokument.elementsByTagName("Manufacturer").at(0).firstChildElement("Value").firstChild().nodeValue();
            seznamZarizeni[index].serialNumber=dokument.elementsByTagName("SerialNumber").at(0).firstChildElement("Value").firstChild().nodeValue();
            seznamZarizeni[index].swVersion=getVersion(dokument,"SwVersion");
        }

        qDebug()<<"device class je: "<< zarizeni.deviceClass<<" verze: "<<zarizeni.swVersion<<" index na seznamu"<<index<<" adresa:"<<zarizeni.adresa<<" port:"<<zarizeni.port;

    }
    else
    {
        qDebug()<<"index je mimo rozsah";
    }


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

    noveZarizeni.adresa=zcs->ip();
    noveZarizeni.port=zcs->port();
    noveZarizeni.deviceClass="";
    noveZarizeni.deviceId="";
    // noveZarizeni.deviceName=zcs->host();
    noveZarizeni.serviceName=zcs->name();
    noveZarizeni.hostname=zcs->host();
    noveZarizeni.ibisIpVersion=zcs.data()->txt().value("ver");

    getDeviceConfiguration(noveZarizeni,zcs);
    getDeviceInformation(noveZarizeni,zcs);
    seznamZarizeni.push_back(noveZarizeni);

    emit aktualizaceSeznamu();
}
