#include "ibisipsubscriber.h"


IbisIpSubscriber::IbisIpSubscriber(QString serviceName,QString structureName,QString version,QString serviceType, int portNumber) : httpServerSubscriber (portNumber)
{
    qDebug() <<  Q_FUNC_INFO;

    mServiceName=serviceName;
    mServiceType=serviceType;
    mStructureName=structureName;
    mVersion=version;
    deviceAddress=selectNonLoopbackAddress();
}


int IbisIpSubscriber::portNumber() const
{

    return httpServerSubscriber.portNumber();
}

void IbisIpSubscriber::setPortNumber(int newPortNumber)
{
    httpServerSubscriber.setPortNumber(newPortNumber);
}


void IbisIpSubscriber::start()
{
    qDebug()<<Q_FUNC_INFO;

    httpServerSubscriber.start();
    allConnects();
}


void IbisIpSubscriber::allConnects()
{

}

int IbisIpSubscriber::subnetMask() const
{
    return mSubnetMask;
}

void IbisIpSubscriber::setSubnetMask(int newSubnetMask)
{
    mSubnetMask = newSubnetMask;
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

/*
int IbisIpSubscriber::portNumber() const
{
    return mPortNumber;
}
*/
/*
void IbisIpSubscriber::setPortNumber(int newPortNumber)
{
    qDebug() <<  Q_FUNC_INFO << QString::number(newPortNumber);
    mPortNumber = newPortNumber;

}

*/
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

QHostAddress IbisIpSubscriber::selectNonLoopbackAddressInSubnet(QHostAddress addressOfPublisher,int mask)
{
    qDebug() <<  Q_FUNC_INFO;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QHostAddress output;


    bool isIpSet=false;

    foreach(QHostAddress selectedAddress, list)
    {
        qDebug() <<" "<<selectedAddress.toString();
        if(!selectedAddress.isLoopback())
        {
            if (selectedAddress.protocol() == QAbstractSocket::IPv4Protocol )
            {
                qDebug() <<" not loopback"<< selectedAddress.toString();
                if(isIpSet==false)
                {
                    if(selectedAddress.isInSubnet(addressOfPublisher,mask))
                    {
                        qDebug()<<" address "<<selectedAddress<<" is in subnet of "<<addressOfPublisher;
                        output=selectedAddress;
                        isIpSet=true;
                    }
                    else
                    {
                        qDebug()<<" address "<<selectedAddress<<" is NOT in subnet of "<<addressOfPublisher;
                    }


                }





            }
        }
    }
    return output;
}
