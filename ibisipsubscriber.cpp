#include "ibisipsubscriber.h"


IbisIpSubscriber::IbisIpSubscriber(QString nazevSluzby,QString struktura,QString verze,QString typSluzby, int cisloPortu) : InstanceNovehoServeru(cisloPortu)
{
    qDebug() <<  Q_FUNC_INFO;
    cisloPortuInterni=cisloPortu;
    nazevSluzbyInterni=nazevSluzby;
    typSluzbyInterni=typSluzby;
    strukturaInterni=struktura;
    verzeInterni=verze;
    adresaZarizeni=projedAdresy();

    vsechnyConnecty();
       // this->projedAdresy();


}

void IbisIpSubscriber::vsechnyConnecty()
{


}


QByteArray IbisIpSubscriber::vyrobHlavickuOk()
{
    qDebug() <<  Q_FUNC_INFO;
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
    qDebug() <<  Q_FUNC_INFO;
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




int IbisIpSubscriber::vymazSluzbuZeSeznamu(QVector<QZeroConfService> &intSeznamSluzeb, QZeroConfService sluzba)
{
    qDebug() <<  Q_FUNC_INFO;
    if(    intSeznamSluzeb.removeOne(sluzba))
    {
        qDebug()<<"sluzbu se podarilo odstranit";
        emit signalAktualizaceSeznamu();
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
    qDebug() <<  Q_FUNC_INFO;
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
    qDebug() <<  Q_FUNC_INFO;
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






