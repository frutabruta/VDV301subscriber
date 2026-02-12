#include "ibisipsubscriber.h"

Q_LOGGING_CATEGORY(IbisIpSubscriberLog, "IbisIpSubscriber")

IbisIpSubscriber::IbisIpSubscriber(QString serviceName,QString structureName,QString version,QString serviceType, int portNumber, QString replyPath) : httpServerSubscriber (portNumber, replyPath)
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;

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
    qCDebug(IbisIpSubscriberLog)<<Q_FUNC_INFO;

    httpServerSubscriber.setReplyPath(mReplyPath);
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
    httpServerSubscriber.setReplyPath(mReplyPath);
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
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO << QString::number(newPortNumber);
    mPortNumber = newPortNumber;

}

*/
QByteArray IbisIpSubscriber::createOkResponse()
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;
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
    qCDebug(IbisIpSubscriberLog)<<"adresaCile string "<<addressComplete;
    return QUrl(addressComplete);
}


void IbisIpSubscriber::findServices(QString serviceType, int start)
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;
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
                qCDebug(IbisIpSubscriberLog)<<"searching for services";
                zeroConf.startBrowser(serviceType);
            }
        }
    }
    else
    {
        qCDebug(IbisIpSubscriberLog)<<"bonjour blocked!";
    }

}


int IbisIpSubscriber::deleteServiceFromList(QVector<QZeroConfService> &serviceList, QZeroConfService selectedService)
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;
    if(    serviceList.removeOne(selectedService))
    {
        qCDebug(IbisIpSubscriberLog)<<"couldn't remove service";
        emit signalUpdateDeviceList();
        return 1;
    }
    return 0;
}


int IbisIpSubscriber::isTheServiceRequestedOne(QString selectedServiceName,QString selectedVersion, QZeroConfService zcs)
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;

    return isTheServiceRequestedOne(selectedServiceName, selectedVersion, PublisherStruct(zcs));
}

int IbisIpSubscriber::isTheServiceRequestedOne(QString selectedServiceName,QString selectedVersion, PublisherStruct publisherStruct)
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;

    return isTheServiceRequestedOne(selectedServiceName,selectedVersion,publisherStruct.serviceName,publisherStruct.ibisIpVersion);
}


int IbisIpSubscriber::isTheServiceRequestedOne(QString selectedServiceName,QString selectedVersion,QString testedServiceName, QString testedVersion)
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;

    qCDebug(IbisIpSubscriberLog)<<"tested service: "<<testedServiceName<<" "<<testedVersion;
    if (testedServiceName.startsWith(selectedServiceName))
    {
        qCDebug(IbisIpSubscriberLog)<<"requested service found "<<testedServiceName;
        if(testedVersion==selectedVersion)
        {
            qCDebug(IbisIpSubscriberLog)<<"1 requested version:"<<selectedVersion<<" found version:"<<testedVersion;
            //this->vytvorSubscribeRequest(projedAdresy(),cisloPortuInterni);
            return 1;
        }
        else
        {
            qCDebug(IbisIpSubscriberLog)<<"1 requested version:"<<selectedVersion<<" found version:"<<testedVersion;
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
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QHostAddress output;

    foreach(QHostAddress selectedAddress, list)
    {
        qCDebug(IbisIpSubscriberLog) <<" "<<selectedAddress.toString();
        if(!selectedAddress.isLoopback())
        {
            if (selectedAddress.protocol() == QAbstractSocket::IPv4Protocol )
            {
                qCDebug(IbisIpSubscriberLog) <<" not loopback"<< selectedAddress.toString();
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
        qCDebug(IbisIpSubscriberLog)<<"address found";
        qCInfo(IbisIpSubscriberLog)<<" selected non-loopback address"<<output.toString();
    }
    else
    {
        qCDebug(IbisIpSubscriberLog)<<"non-loopback address not found";
        if(allowLoopback)
        {
            output=QHostAddress::LocalHost;
            qCInfo(IbisIpSubscriberLog)<<"non-loopback address allowed, setting localhost "<<output.toString();
        }
        else
        {
            qCInfo(IbisIpSubscriberLog)<<"non-loopback address not allowed";
        }
    }

    emit signalAddressUpdate(output);
    return output;
}

QHostAddress IbisIpSubscriber::selectNonLoopbackAddressInSubnet(QHostAddress addressOfPublisher,int mask)
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QHostAddress output;


    bool isIpSet=false;

    if(addressOfPublisher.isLoopback())
    {
        qCDebug(IbisIpSubscriberLog)<<"address of publisher is loopback, setting local host";
        output=QHostAddress::LocalHost;
        isIpSet=true;
    }
    else
    {
        foreach(QHostAddress selectedAddress, list)
        {
            qCDebug(IbisIpSubscriberLog) <<" "<<selectedAddress.toString();
            if(!selectedAddress.isLoopback())
            {
                if (selectedAddress.protocol() == QAbstractSocket::IPv4Protocol )
                {
                    qCDebug(IbisIpSubscriberLog) <<" not loopback"<< selectedAddress.toString();
                    if(isIpSet==false)
                    {
                        if(selectedAddress.isInSubnet(addressOfPublisher,mask))
                        {
                            qCDebug(IbisIpSubscriberLog)<<" address "<<selectedAddress<<" is in subnet of "<<addressOfPublisher<<" mask: "<<mask;
                            qCInfo(IbisIpSubscriberLog)<<"selected address "<<selectedAddress<<" in subnet of "<<addressOfPublisher<<" mask: "<<mask;

                            output=selectedAddress;
                            isIpSet=true;
                        }
                        else
                        {
                            qCDebug(IbisIpSubscriberLog)<<" address "<<selectedAddress<<" is NOT in subnet of "<<addressOfPublisher<<" mask: "<<mask;
                        }
                    }
                }
            }
        }
    }


    qCDebug(IbisIpSubscriberLog)<<"selected address: "<<output;
    qCDebug(IbisIpSubscriberLog)<<"is ip set: "<<isIpSet;
    emit signalAddressUpdate(output);
    return output;
}



void IbisIpSubscriber::postGenericRequest(QUrl subscriberAddress, QString postRequestContent)
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;
    qCDebug(IbisIpSubscriberLog).noquote()<<"posting to address: "<<subscriberAddress<<" "<<postRequestContent;

    QNetworkRequest postRequest(subscriberAddress);


    // https://stackoverflow.com/a/53556560

    postRequest.setTransferTimeout(30000);
    postRequest.setRawHeader("Content-Type", "text/xml");
    //postRequest.setRawHeader("Expect", "100-continue");
    //postRequest.setRawHeader("Connection", "keep-Alive");
    //postRequest.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray postRequestContentQByteArray=postRequestContent.toUtf8() ;

    QPointer<QNetworkReply> reply=postManager.post(postRequest,postRequestContentQByteArray);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriber::slotHttpRequestFinished);

}

/*
void IbisIpSubscriber::slotHttpRequestGenericFinished()
{
    qCDebug(IbisIpSubscriberLog) <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qCDebug(IbisIpSubscriberLog)<<"unsusbscription response:";
    qCDebug(IbisIpSubscriberLog).noquote()<<str;

    if(reply->error()!=QNetworkReply::NoError)
    {
        qCDebug(IbisIpSubscriberLog)<<reply->errorString();

        reply->deleteLater();
        return;
    }

    // subscribedService=subscribeServiceCandidate;


    QDomDocument qDomResponse;
    //bool setContentResult=false;

    emit signalError(bts);

    reply->deleteLater();
}
*/





void IbisIpSubscriber::slotHttpRequestFinished()
{
    qCDebug(IbisIpSubscriberLog) << Q_FUNC_INFO;

    QPointer<QNetworkReply> reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
    {
        qCDebug(IbisIpSubscriberLog) << "finished slot without sender() QNetworkReply";
        qCWarning(IbisIpSubscriberLog)<<"empty reply to subscribe request";
        emit signalIsSubscriptionSuccesful(false);
        return;
    }
    qCDebug(IbisIpSubscriberLog) << Q_FUNC_INFO
                                 << "URL:" << reply->url()
                                 << "HTTP status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (!reply->isOpen())
    {
        qCDebug(IbisIpSubscriberLog) << "Reply device not open";
        qCWarning(IbisIpSubscriberLog)<<" reply device "<<reply->url()<<" not open";
        reply->deleteLater();
        emit signalIsSubscriptionSuccesful(false);
        return;
    }

    if (reply->error() != QNetworkReply::NoError)
    {
        qCDebug(IbisIpSubscriberLog) << reply->errorString();
        qCWarning(IbisIpSubscriberLog)<<" subscription to "<<reply->url()<<" failed with error"<<reply->errorString();
        reply->deleteLater();
        emit signalIsSubscriptionSuccesful(false);
        return;
    }

    const QByteArray bts = reply->readAll();
    const QString str = QString::fromUtf8(bts);

    emit signalResponseNotEmpty(str);

    reply->deleteLater();
}



void IbisIpSubscriber::slotHttpRequestErrorHappened(QNetworkReply::NetworkError code)
{
    qCDebug(IbisIpSubscriberLog) << Q_FUNC_INFO;

    QPointer<QNetworkReply> reply = qobject_cast<QNetworkReply*>(sender());

    qCWarning(IbisIpSubscriberLog)<<" network code:"<<code;

    if (!reply.isNull())
    {
        qCWarning(IbisIpSubscriberLog) <<"subscription failed, Network error occurred:"
                                      << static_cast<int>(reply->error())
                                      << reply->errorString();

        qCWarning(IbisIpSubscriberLog)<<" subscription to "<<reply->url()<<" failed with error"<<reply->errorString();

    }
    else
    {
        qCDebug(IbisIpSubscriberLog) << "finished slot without sender() QNetworkReply";
        qCWarning(IbisIpSubscriberLog)<<"empty reply to subscribe request";
        //    reply->deleteLater();


    }

    reply->deleteLater();
}



