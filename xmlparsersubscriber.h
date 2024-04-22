#ifndef XMLPARSERSUBSCRIBER_H
#define XMLPARSERSUBSCRIBER_H

#include <QObject>
#include <QDomDocument>

#include "devmgmtpublisherstruct.h"

class XmlParserSubscriber
{
public:
    XmlParserSubscriber();
    void deviceInformationToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device);
    void deviceConfigurationToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device);
    void deviceStatusToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device);
    QString getVersion(QDomDocument document, QString element);
    bool dataAcceptedResponseToBoolean(QDomDocument &domDocument);
};

#endif // XMLPARSERSUBSCRIBER_H
