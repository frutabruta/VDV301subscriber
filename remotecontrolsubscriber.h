#ifndef REMOTECONTROLSUBSCRIBER_H
#define REMOTECONTROLSUBSCRIBER_H

#include "ibisipsubscriberonepublisher.h"
#include "VDV301DataStructures/vdv301enumerations.h"
#include "VDV301subscriber/xmlparserremotecontrol.h"

class RemoteControlSubscriber : public IbisIpSubscriberOnePublisher
{
    Q_OBJECT
public:
    RemoteControlSubscriber(QString serviceName, QString structureName, QString version, QString serviceType, int portNumber, QString replyPath="");
    XmlParserRemoteControl parser;
private slots:
    void slotHandleMessage(QString data);
signals:
    void signalMessageType(Vdv301Enumerations::RemoteControlMessageTypeEnumeration messageType);
};

#endif // REMOTECONTROLSUBSCRIBER_H
