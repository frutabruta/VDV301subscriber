#ifndef XMLPARSERREMOTECONTROL_H
#define XMLPARSERREMOTECONTROL_H

#include <QObject>
#include <QDomDocument>

#include "VDV301DataStructures/vdv301enumerations.h"

class XmlParserRemoteControl
{
public:
    XmlParserRemoteControl();
    static void parseStateChange(QDomDocument &domDocument, Vdv301Enumerations::RemoteControlMessageTypeEnumeration &messageType, QString &messageParameter);
};

#endif // XMLPARSERREMOTECONTROL_H
