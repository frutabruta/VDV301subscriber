#include "xmlparserremotecontrol.h"


XmlParserRemoteControl::XmlParserRemoteControl() {}


void XmlParserRemoteControl::parseStateChange(QDomDocument &domDocument, Vdv301Enumerations::RemoteControlMessageTypeEnumeration &messageType, QString &messageParameter)
{
    QString root=domDocument.firstChildElement().tagName();
    QString messageTypeString=domDocument.elementsByTagName("MessageType").at(0).firstChild().nodeValue();
    messageType=Vdv301Enumerations::RemoteControlMessageTypeEnumerationFromQString(messageTypeString);
    messageParameter=domDocument.elementsByTagName("MessageParameter").at(0).firstChild().nodeValue();
}
