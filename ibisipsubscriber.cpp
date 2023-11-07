#include "ibisipsubscriber.h"


IbisIpSubscriber::IbisIpSubscriber(QString serviceName,QString structureName,QString version,QString serviceType, int portNumber) : httpServerSubscriber (portNumber)
{
    qDebug() <<  Q_FUNC_INFO;
    mPortNumber=portNumber;
    mServiceName=serviceName;
    mServiceType=serviceType;
    mStructureName=structureName;
    mVersion=version;
    deviceAddress=selectNonLoopbackAddress();

    allConnects();
       // this->projedAdresy();

}

void IbisIpSubscriber::allConnects()
{

}

bool IbisIpSubscriber::isIpSet() const
{
    return mIsIpSet;
}

void IbisIpSubscriber::setIsIpSet(bool newIsIpSet)
{
    mIsIpSet = newIsIpSet;
}

QString IbisIpSubscriber::version() const
{
    return mVersion;
}

void IbisIpSubscriber::setVersion(const QString &newVersion)
{
    mVersion = newVersion;
}

int IbisIpSubscriber::portNumber() const
{
    return mPortNumber;
}

void IbisIpSubscriber::setPortNumber(int newPortNumber)
{
    qDebug() <<  Q_FUNC_INFO << QString::number(newPortNumber);
    mPortNumber = newPortNumber;
}


QByteArray IbisIpSubscriber::createOkResponse()
{
    qDebug() <<  Q_FUNC_INFO;
    QByteArray okResponse;
    this->mHeader="";
    okResponse+=("HTTP/1.1 200 OK\r\n");       // \r needs to be before \n
    okResponse+=("Content-Type: application/xml\r\n");
    okResponse+=("Connection: close\r\n");
    okResponse+=("Pragma: no-cache\r\n");
    okResponse+=("\r\n");
    return okResponse;
}


void IbisIpSubscriber::findServices(QString serviceType, int start)
{
    qDebug() <<  Q_FUNC_INFO;
    if (start == 0 ) //stops service browser
    {
        zeroConf.stopBrowser();
    }
    else if (start == 1) //starts service browser
    {
        if (!zeroConf.browserExists())
        {
            qDebug()<<"searching for services";
            zeroConf.startBrowser(serviceType);
        }
    }
}


int IbisIpSubscriber::deleteServiceFromList(QVector<QZeroConfService> &serviceList, QZeroConfService selectedService)
{
    qDebug() <<  Q_FUNC_INFO;
    if(    serviceList.removeOne(selectedService))
    {
        qDebug()<<"couldn't remove service";
        emit signalUpdateDeviceList();
        return 1;
    }
    return 0;
}


int IbisIpSubscriber::isTheServiceRequestedOne(QString selectedServiceName,QString selectedVersion, QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;
    QString testedVersion=zcs.data()->txt().value("ver");
    QString testedServiceName=zcs->name();
    qDebug()<<"tested service: "<<testedServiceName<<" "<<testedVersion;
    if (testedServiceName.startsWith(selectedServiceName))
    {
        qDebug()<<"requested service found "<<testedServiceName;
        if(testedVersion==selectedVersion)
        {
            qDebug()<<"1 requested version:"<<selectedVersion<<" found version:"<<testedVersion;
            //this->vytvorSubscribeRequest(projedAdresy(),cisloPortuInterni);
            return 1;
        }
        else
        {
             qDebug()<<"1 requested version:"<<selectedVersion<<" found version:"<<testedVersion;
            return 0;
        }
    }

    return 0;
}

QString IbisIpSubscriber::createSubscribeRequest(QHostAddress clientIpAddress, int port)
{
    QDomDocument xmlDocument;
    QDomProcessingInstruction dProcessingInformation=xmlDocument.createProcessingInstruction("xml","version=\"1.0\" encoding=\"utf-8\" ");
    xmlDocument.appendChild(dProcessingInformation);
    QDomElement dSubscribeRequest =xmlDocument.createElement("SubscribeRequest");
    xmlDocument.appendChild(dSubscribeRequest);
    QDomElement dClientIPAddress=xmlDocument.createElement("Client-IP-Address");
    QDomElement dIpValue=xmlDocument.createElement("Value");
    dIpValue.appendChild(xmlDocument.createTextNode(clientIpAddress.toString()));
    dClientIPAddress.appendChild(dIpValue);
    dSubscribeRequest.appendChild(dClientIPAddress);
    QDomElement dReplyPort=xmlDocument.createElement("ReplyPort");
    QDomElement dPortValue=xmlDocument.createElement("Value");
    dPortValue.appendChild(xmlDocument.createTextNode(QString::number(port)));
    dReplyPort.appendChild(dPortValue);
    dSubscribeRequest.appendChild(dReplyPort);

    return xmlDocument.toString();
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



QHostAddress IbisIpSubscriber::selectNonLoopbackAddress()
{
    qDebug() <<  Q_FUNC_INFO;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QHostAddress output;




    /* int ipIndex=0;



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

*/

    foreach(QHostAddress selectedAddress, list)
    {
        qDebug() <<" "<<selectedAddress.toString();
        if(!selectedAddress.isLoopback())
        {
            if (selectedAddress.protocol() == QAbstractSocket::IPv4Protocol )
            {
                qDebug() <<" not loopback"<< selectedAddress.toString();
                if(mIsIpSet==false)
                {

                    output=selectedAddress;
                    mIsIpSet=true;
                }
            }
        }
    }




    return output;
}
