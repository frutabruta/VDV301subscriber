#include "xmlgeneratorsubscriber.h"
#include <QDebug>

XmlGeneratorSubscriber::XmlGeneratorSubscriber() {}


QDomDocument XmlGeneratorSubscriber::setDeviceConfigurationRequest(QDomDocument xmlDocument, QString id)
{
    qDebug()<<Q_FUNC_INFO;
    xmlDocument.appendChild(createProcessingInformation(xmlDocument,encoding));


    QDomElement domSetDeviceConfigurationRequest=xmlDocument.createElement("DeviceManagementService.SetDeviceConfigurationRequest");
    QDomElement domDeviceId=value(xmlDocument,"DeviceID",id);

    domSetDeviceConfigurationRequest.appendChild(domDeviceId);
    xmlDocument.appendChild(domSetDeviceConfigurationRequest);

    qDebug()<<xmlDocument.toString();



    return xmlDocument;

}

QDomProcessingInstruction XmlGeneratorSubscriber::createProcessingInformation(QDomDocument &xmlDocument, QString encoding)
{
    return xmlDocument.createProcessingInstruction("xml","version=\"1.0\" encoding=\""+encoding+"\" ");
}

/*
<?xml version="1.0" encoding="utf-16"?>
<DeviceManagementService.SetDeviceConfigurationRequest xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <DeviceID>
    <Value>722</Value>
  </DeviceID>
</DeviceManagementService.SetDeviceConfigurationRequest>
 */


QDomElement XmlGeneratorSubscriber::value(QDomDocument &xmlDocument, QString elementName, QString content)
{
    QDomElement result=xmlDocument.createElement(elementName); //verze 2.2CZ1.0
    result.appendChild(xmlDocument.createElement("Value")).appendChild( xmlDocument.createTextNode(content));
    return result;
}


QString XmlGeneratorSubscriber::createSubscribeRequest(QHostAddress clientIpAddress, int port)
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

QString XmlGeneratorSubscriber::createUnsubscribeRequest(QHostAddress clientIpAddress, int port)
{
    QDomDocument xmlDocument;
    QDomProcessingInstruction dProcessingInformation=xmlDocument.createProcessingInstruction("xml","version=\"1.0\" encoding=\"utf-8\" ");
    xmlDocument.appendChild(dProcessingInformation);
    QDomElement dSubscribeRequest =xmlDocument.createElement("UnsubscribeRequest");
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
