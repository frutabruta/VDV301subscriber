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
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &IbisIpSubscriber::slotAddService);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &IbisIpSubscriber::slotOdstranenaSluzba);
    connect(timer, &QTimer::timeout, this, &IbisIpSubscriber::slotCasovacVyprsel);
   // this->projedAdresy();


    timer->start(defaultniCasovac);
}

void IbisIpSubscriber::slotCasovacVyprsel()
{
    qDebug()<<"IbisIpSubscriber::casovacVyprsel";
    emit signalZtrataOdberu();
    novePrihlaseniOdberu();

}

void IbisIpSubscriber::novePrihlaseniOdberu()
{
    qDebug()<<"IbisIpSubscriber::novePrihlaseniOdberu()";
    odebirano=false;
    existujeKandidat=false;
    hledejSluzby(typSluzbyInterni,1);
}

void IbisIpSubscriber::vypisObsahRequestu(QString vysledek)
{

    qDebug()<<"IbisIpSubscriber::vypisObsahRequestu";
    QByteArray posledniRequest=InstanceNovehoServeru.bodyPozadavku;
    QDomDocument xmlrequest;
    xmlrequest.setContent(vysledek);
    timer->start(defaultniCasovac);
    emit dataNahrana(vysledek);
}




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


void IbisIpSubscriber::hledejSluzby(QString typSluzby, int start)
{
    qDebug()<<"IbisIpSubscriber::hledejSluzby";
    if (start == 0 )
    {
        zeroConf.stopBrowser();
    }
    else if (start == 1)
    {
        if (!zeroConf.browserExists())
        {
            qDebug()<<"prohledavam";
            zeroConf.startBrowser(typSluzby);
        }
    }
}

void IbisIpSubscriber::slotAddService(QZeroConfService zcs)
{

    qDebug()<<"IbisIpSubscriber::addService";
    // QTableWidgetItem *cell;
    // qDebug() << "Added service: " << zcs;
    QString nazev=zcs->name();
    QString ipadresa=zcs->ip().toString();
    QString verze=zcs.data()->txt().value("ver");
    int port=zcs->port();
    qDebug() <<"nazev sluzby "<<nazev<<" ip adresa "<<ipadresa<<" port "<<QString::number(port)<<" data" <<verze ;

    //QString strukturaKodberu="/CustomerInformationService/SubscribeAllData";
    QString adresaZaLomitkem="/"+nazevSluzbyInterni+"/Subscribe"+strukturaInterni;
    QString adresaCileString="http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+adresaZaLomitkem;
    qDebug()<<"adresaCile string "<<adresaCileString;
    QUrl adresaKamPostovatSubscribe=QUrl(adresaCileString);

    if (jeSluzbaHledanaVerze(nazevSluzbyInterni,verzeInterni,zcs)&&(this->existujeKandidat==false)&&(this->odebirano==false))
    {
        qDebug()<<"odesilam subscribe na "<<ipadresa<<":"<<QString::number(port)<<" sluzba "<<nazev;
        existujeKandidat=true;
        kandidatSluzbaMdns=zcs;
        PostSubscribe(adresaKamPostovatSubscribe,this->vytvorSubscribeRequest(projedAdresy(),cisloPortuInterni));
    }
    seznamSluzeb.append(zcs);
    emit nalezenaSluzba( zcs);
    emit aktualizaceSeznamu();
}



int IbisIpSubscriber::vymazSluzbuZeSeznamu(QVector<QZeroConfService> &intSeznamSluzeb, QZeroConfService sluzba)
{
    qDebug()<<"";
    if(    intSeznamSluzeb.removeOne(sluzba))
    {
        qDebug()<<"sluzbu se podarilo odstranit";
        emit aktualizaceSeznamu();
        return 1;
    }
    else
    {
         qDebug()<<"sluzbu se nepodarilo odstranit";
    }
    return 0;
}




int IbisIpSubscriber::jeSluzbaHledanaVerze(QString hledanaSluzba,QString hledanaVerze, QZeroConfService zcs)
{
    qDebug()<<"IbisIpSubscriber::jeSluzbaHledanaVerze";
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
    int ipIndex=0;
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
                  ipIndex=nIter;
                  ipSet=true;
                }
            }
        }



    }
    return list[ipIndex];

}

void IbisIpSubscriber::PostSubscribe(QUrl adresaDispleje, QString dataDoPostu)
{

    qDebug()<<"IbisIpSubscriber::PostSubscribe";
    qDebug()<<"postuju na adresu "<<adresaDispleje<<" "<<dataDoPostu;
 //   QByteArray postDataSize = QByteArray::number(dataDoPostu.size());
    QNetworkRequest pozadavekPOST(adresaDispleje);
    qDebug()<<"A";
    //pozadavekPOST.setRawHeader("Content-Length", postDataSize );

    QNetworkAccessManager *manager2 = new QNetworkAccessManager(); // https://stackoverflow.com/a/53556560
    connect(manager2,SIGNAL(finished(QNetworkReply*)),this,SLOT(slotSubscribeOdeslan(QNetworkReply*)));
    connect(manager2,SIGNAL(finished(QNetworkReply*)),manager2,SLOT(deleteLater()));


    pozadavekPOST.setTransferTimeout(30000);
    pozadavekPOST.setRawHeader("Content-Type", "text/xml");
    pozadavekPOST.setRawHeader("Expect", "100-continue");
    pozadavekPOST.setRawHeader("Connection", "keep-Alive");
    qDebug()<<"B";

    //pozadavekPOST.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray dataDoPostuQByte=dataDoPostu.toUtf8() ;
    qDebug()<<"C";
    manager2->post(pozadavekPOST,dataDoPostuQByte);

    qDebug()<<"D";

}

void IbisIpSubscriber::slotSubscribeOdeslan(QNetworkReply *rep)
{
qDebug()<<"IbisIpSubscriber::slotSubscribeOdeslan";
QByteArray bts = rep->readAll();
    QString str(bts);
 qDebug()<<"odpoved na subscribe:"<<str;

 aktualniSluzbaMdns=kandidatSluzbaMdns;
 this->odebirano=true;
}

void IbisIpSubscriber::slotOdstranenaSluzba(QZeroConfService zcs)
{
    qDebug()<<"IbisIpSubscriber::slotOdstranenaSluzba()";
    vymazSluzbuZeSeznamu(seznamSluzeb,zcs);
    if(zcs==aktualniSluzbaMdns)
    {
        emit signalZtrataOdberu();
        novePrihlaseniOdberu();
    }
}
