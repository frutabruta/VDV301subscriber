#include "ibisipsubscriber.h"


IbisIpSubscriber::IbisIpSubscriber(QString serviceName,QString structureName,QString version,QString serviceType, int portNumber, QString replyPath) : httpServerSubscriber (portNumber, replyPath)
{
    qDebug() <<  Q_FUNC_INFO;

    mReplyPath=replyPath;
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

QString IbisIpSubscriber::replyPath() const
{
    return mReplyPath;
}

void IbisIpSubscriber::setReplyPath(const QString &newReplyPath)
{
    mReplyPath = newReplyPath;
}

QString IbisIpSubscriber::structureName() const
{
    return mStructureName;
}

void IbisIpSubscriber::setStructureName(const QString &newStructureName)
{
    mStructureName = newStructureName;
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



QUrl IbisIpSubscriber::createSubscribeDestination(PublisherStruct publisherStruct)
{

    QString addressAfterBackslash="/"+mServiceName+"/Subscribe"+mStructureName;
    QString addressComplete="http://"+publisherStruct.hostAddress.toString()+":"+QString::number(publisherStruct.portNumber)+addressAfterBackslash;
    qDebug()<<"adresaCile string "<<addressComplete;
    return QUrl(addressComplete);
}


void IbisIpSubscriber::findServices(QString serviceType, int start)
{
    qDebug() <<  Q_FUNC_INFO;
    if(!blockBonjour)
    {
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
    else
    {
        qDebug()<<"bonjour blocked!";
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


int IbisIpSubscriber::isTheServiceRequestedOne(QString selectedServiceName,QString selectedVersion,QString testedServiceName, QString testedVersion)
{
    qDebug() <<  Q_FUNC_INFO;

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

    if(mIsIpSet)
    {
        qDebug()<<"address found";
    }
    else
    {
        qDebug()<<"non-loopback address not found";
        if(allowLoopback)
        {
            output=QHostAddress::LocalHost;
            qDebug()<<"non-loopback address allowed, setting localhost";
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

    if(addressOfPublisher.isLoopback())
    {
        qDebug()<<"address of publisher is loopback, setting local host";
        output=QHostAddress::LocalHost;
        isIpSet=true;
    }
    else
    {
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
    }


    return output;
}



void IbisIpSubscriber::postGenericRequest(QUrl subscriberAddress, QString postRequestContent)
{
    qDebug() <<  Q_FUNC_INFO;
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
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriber::slotHttpRequestGenericFinished);

}

void IbisIpSubscriber::slotHttpRequestGenericFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"unsusbscription response:";
    qDebug().noquote()<<str;

    if(reply->error()!=QNetworkReply::NoError)
    {
        qDebug()<<reply->errorString();

        reply->deleteLater();
        return;
    }

    // subscribedService=subscribeServiceCandidate;


    QDomDocument qDomResponse;
    //bool setContentResult=false;

    emit signalError(bts);

    reply->deleteLater();
}


