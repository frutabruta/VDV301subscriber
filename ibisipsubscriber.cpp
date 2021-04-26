#include "ibisipsubscriber.h"


IbisIpSubscriber::IbisIpSubscriber(QString nazevSluzby,QString struktura,QString verze,QString typSluzby, int cisloPortu) : InstanceNovehoServeru(cisloPortu)
{
    qDebug()<<"HttpSluzba::HttpSluzba";
    cisloPortuInterni=cisloPortu;
    nazevSluzbyInterni=nazevSluzby;
    typSluzbyInterni=typSluzby;
    strukturaInterni=struktura;
    verzeInterni=verze;
    connect(&InstanceNovehoServeru,&NewHttpServer::prijemDat,this,&IbisIpSubscriber::vypisObsahRequestu);
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &IbisIpSubscriber::addService);
    this->projedAdresy();

    connect(timer, &QTimer::timeout, this, &IbisIpSubscriber::casovacVyprsel);
    timer->start(defaultniCasovac);
}

void IbisIpSubscriber::casovacVyprsel()
{
    qDebug()<<"IbisIpSubscriber::casovacVyprsel";
    odebirano=false;
    hledejSluzby(typSluzbyInterni,0);
    hledejSluzby(typSluzbyInterni,1);
}

void IbisIpSubscriber::vypisObsahRequestu(QString vysledek)
{

    qDebug()<<"IbisIpSubscriber::vypisObsahRequestu";
    QByteArray posledniRequest=InstanceNovehoServeru.bodyPozadavku;
    QDomDocument xmlrequest;
    xmlrequest.setContent(vysledek);
    QString adresa= xmlrequest.elementsByTagName("Client-IP-Address").at(0).toElement().firstChildElement().text() ;
    QString port= xmlrequest.elementsByTagName("ReplyPort").at(0).toElement().firstChildElement().text() ;
    qDebug()<<"prvni element "<<adresa<<" "<<port;
    qDebug()<<"body pozadavku"<<posledniRequest;
    qDebug()<<"vysledek"<<vysledek;
    QString kompletadresa="http://"+adresa+":"+port;
    QUrl adresaurl=kompletadresa;
    //emit pridejSubscribera(adresaurl);
    //qDebug()<<obsahBody;
    timer->start(defaultniCasovac);
    emit dataNahrana(vysledek);
}
/*

HttpSluzba::HttpSluzba(QString nazevSluzby,QString typSluzby, int cisloPortu):InstanceNovehoServeru(cisloPortu)
{
    qDebug()<<"HttpSluzba::HttpSluzba";
    cisloPortuInterni=cisloPortu;
    nazevSluzbyInterni=nazevSluzby;
    typSluzbyInterni=typSluzby;
    bonjourStartKomplet();
    hlavickaInterni=vyrobHlavickuGet();
    //connect(&InstanceNovehoServeru,SIGNAL(zmenaObsahu()),this,SLOT(vypisObsahRequestu()));
    connect(&InstanceNovehoServeru,&NewHttpServer::zmenaObsahu,this,&HttpSluzba::vypisObsahRequestu);
}
*/



QByteArray IbisIpSubscriber::vyrobHlavickuOk()
{
    qDebug()<<"HttpSluzba::vyrobHlavicku()";
    QByteArray hlavicka;
    this->hlavickaInterni="";
    QByteArray argumentXMLserveru = "";
    hlavicka+=("HTTP/1.1 200 OK\r\n");       // \r needs to be before \n
    hlavicka+=("Content-Type: application/xml\r\n");
    hlavicka+=("Connection: close\r\n");
    hlavicka+=("Pragma: no-cache\r\n");
    hlavicka+=("\r\n");
    return hlavicka;
}


void IbisIpSubscriber::hledejSluzby(QString typsluzby, int start)
{
    qDebug()<<"IbisIpSubscriber::hledejSluzby";
    typsluzby="_ibisip_http._tcp.";
    if (start == 0 ) {
        //zeroConf.stopServicePublish();
        zeroConf.stopBrowser();
    }
    else if (start == 1)
    {

        if (!zeroConf.browserExists())
        {
            //	zeroConf.startBrowser("_qtzeroconf_test._tcp");
            qDebug()<<"prohledavam";
            zeroConf.startBrowser(typSluzbyInterni);
            //zeroConf.startBrowser("_ibisip_http._tcp.");
        }

    }
}

void IbisIpSubscriber::addService(QZeroConfService zcs)
{

    // QTableWidgetItem *cell;
    // qDebug() << "Added service: " << zcs;
    QString nazev=zcs->name();
    QString ipadresa=zcs->ip().toString();
    QString verze=zcs.data()->txt().value("ver");
    int port=zcs->port();

    qDebug() <<"nazev sluzby "<<nazev<<" ip adresa "<<ipadresa<<" port "<<QString::number(port)<<" data" <<verze ;


    //row = table.rowCount();
    // table.insertRow(row);
    // cell = new QTableWidgetItem(zcs->name());
    // table.setItem(row, 0, cell);
    // cell = new QTableWidgetItem(zcs->ip().toString());
    // table.setItem(row, 1, cell);
    // table.resizeColumnsToContents();
#if !(defined(Q_OS_IOS) || defined(Q_OS_ANDROID))
    // setFixedSize(table.horizontalHeader()->length() + 60, table.verticalHeader()->length() + 100);
#endif

    //QString strukturaKodberu="/CustomerInformationService/SubscribeAllData";
    QString adresaZaLomitkem="/"+nazevSluzbyInterni+"/Subscribe"+strukturaInterni;
    QString adresaCileString="http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+adresaZaLomitkem;
    qDebug()<<"adresaCile string "<<adresaCileString;
    QUrl adresaKamPostovatSubscirbe=QUrl(adresaCileString);

    if (najdiSluzbu(nazevSluzbyInterni,verzeInterni,zcs)&&(this->odebirano==false))
    {
        PostSubscribe(adresaKamPostovatSubscirbe,this->vytvorSubscribeRequest(projedAdresy(),cisloPortuInterni));
        qDebug()<<"odesilam subscribe na "<<ipadresa<<":"<<QString::number(port)<<" sluzba "<<nazev;
    }
    emit nalezenaSluzba( zcs);
}









int IbisIpSubscriber::najdiSluzbu(QString hledanaSluzba,QString hledanaVerze, QZeroConfService zcs)
{
    qDebug()<<"IbisIpSubscriber::najdiSluzbu";
    if (zcs->name().startsWith(hledanaSluzba))
    {
        qDebug()<<"sluzba "<<hledanaSluzba<<" Nalezena";
        if(zcs.data()->txt().value("ver")==hledanaVerze)
        {
            qDebug()<<"hledana verze "<<hledanaVerze<<" nalezena";
            //this->vytvorSubscribeRequest(projedAdresy(),cisloPortuInterni);
            return 1;
        }
        else
        {
            return 0;
        }

    }

    return 0;
}

QString IbisIpSubscriber::vytvorSubscribeRequest(QHostAddress ipadresa, int port)
{
    QDomDocument xmlko;
    QDomProcessingInstruction dHlavicka=xmlko.createProcessingInstruction("xml","version=\"1.0\" encoding=\"utf-8\" ");
    xmlko.appendChild(dHlavicka);
    QDomElement subscribeRequest =xmlko.createElement("SubscribeRequest");
    xmlko.appendChild(subscribeRequest);
    QDomElement clientIPAddress=xmlko.createElement("Client-IP-Address");
    QDomElement ipValue=xmlko.createElement("Value");
    ipValue.appendChild(xmlko.createTextNode(ipadresa.toString()));
    clientIPAddress.appendChild(ipValue);
    subscribeRequest.appendChild(clientIPAddress);
    QDomElement replyPort=xmlko.createElement("ReplyPort");
    QDomElement portValue=xmlko.createElement("Value");
    portValue.appendChild(xmlko.createTextNode(QString::number(port)));
    replyPort.appendChild(portValue);
    subscribeRequest.appendChild(replyPort);


    //stopProperty.appendChild(xmlko.createTextNode(hodnota));
    //qDebug()<<"obsah xml subscribe request"<<xmlko.toString();
    return xmlko.toString();
}

/*
POST /CustomerInformationService/SubscribeAllData HTTP/1.1
Content-Type: text/xml
Host: 192.168.1.100:8081
Content-Length: 284
Expect: 100-continue
Connection: Keep-Alive

<?xml version="1.0" encoding="utf-8"?>
<SubscribeRequest xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
<Client-IP-Address>
<Value>192.168.1.128</Value>
</Client-IP-Address>
<ReplyPort><Value>60011</Value>
</ReplyPort>
</SubscribeRequest>
*/



QHostAddress IbisIpSubscriber::projedAdresy()
{
    qDebug()<<"IbisIpSubscriber::projedAdresy()";
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    bool ipSet=false;
    int ipindex=0;
    for(int nIter=0; nIter<list.count(); nIter++)

    {
        qDebug() <<nIter<<" "<< list[nIter].toString();
        if(!list[nIter].isLoopback())
        {
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
            {
                qDebug() <<nIter<<" not loopback"<< list[nIter].toString();
                if(ipSet==false)
                {
                  ipindex=nIter;
                  ipSet=true;
                }
            }
        }



    }
    return list[ipSet];

}

void IbisIpSubscriber::PostSubscribe(QUrl adresaDispleje, QString dataDoPostu)
{

    qDebug()<<"IbisIpSubscriber::PostDoDispleje";
    qDebug()<<"postuju na adresu "<<adresaDispleje;
    QByteArray postDataSize = QByteArray::number(dataDoPostu.size());
    QNetworkRequest pozadavekPOST(adresaDispleje);

    //pozadavekPOST.setRawHeader("Content-Length", postDataSize );
    pozadavekPOST.setRawHeader("Content-Type", "text/xml");
    pozadavekPOST.setRawHeader("Expect", "100-continue");
    pozadavekPOST.setRawHeader("Connection", "keep-Alive");
    //pozadavekPOST.setRawHeader("Accept-Encoding", "gzip, deflate");
    QNetworkAccessManager *manager2 = new QNetworkAccessManager();
    QByteArray dataDoPostuQByte=dataDoPostu.toUtf8() ;
    manager2->post(pozadavekPOST,dataDoPostuQByte);

}
