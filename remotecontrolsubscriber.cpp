#include "remotecontrolsubscriber.h"

RemoteControlSubscriber::RemoteControlSubscriber(QZeroConf& zeroConf, QString serviceName,QString structureName,QString version,QString serviceType, int portNumber, QString replyPath)
    : IbisIpSubscriberOnePublisher(zeroConf, serviceName,structureName,version, serviceType, portNumber, replyPath)
{
    connect(this,&IbisIpSubscriber::signalDataReceived,this,&RemoteControlSubscriber::slotHandleMessage);
}


void RemoteControlSubscriber::slotHandleMessage(QString data)
{
    QString messageParameter="";
    Vdv301Enumerations::RemoteControlMessageTypeEnumeration messageType;
    QDomDocument document;
    document.setContent(data);
    parser.parseStateChange(document,messageType,messageParameter);

    emit signalMessageType(messageType);
}


