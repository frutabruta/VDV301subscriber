#include "xmlparsersubscriber.h"

XmlParserSubscriber::XmlParserSubscriber() {}


void XmlParserSubscriber::deviceInformationToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device)
{
    device.deviceClass=domDocument.elementsByTagName("DeviceClass").at(0).firstChild().nodeValue();
    device.deviceName=domDocument.elementsByTagName("DeviceName").at(0).firstChildElement("Value").firstChild().nodeValue();
    device.manufacturer=domDocument.elementsByTagName("Manufacturer").at(0).firstChildElement("Value").firstChild().nodeValue();
    device.serialNumber=domDocument.elementsByTagName("SerialNumber").at(0).firstChildElement("Value").firstChild().nodeValue();
    device.swVersion=getVersion(domDocument,"SwVersion");
}

void XmlParserSubscriber::deviceConfigurationToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device)
{
    device.deviceId=domDocument.elementsByTagName("DeviceID").at(0).firstChildElement("Value").firstChild().nodeValue();
}


void XmlParserSubscriber::deviceStatusToDevice(QDomDocument &domDocument, DevMgmtPublisherStruct &device)
{
    device.status=domDocument.elementsByTagName("DeviceState").at(0).firstChild().nodeValue();

}


QString XmlParserSubscriber::getVersion(QDomDocument document, QString element)
{
    QString vysledek="";
    QDomNodeList seznamElementu = document.elementsByTagName("DataVersion");

    for (int i=0;i<seznamElementu.count();i++)
    {
        QString tagName=seznamElementu.at(i).toElement().firstChildElement("DataType").firstChildElement("Value").firstChild().nodeValue();
        if(tagName==element)
        {
            vysledek=seznamElementu.at(i).toElement().firstChildElement("VersionRef").firstChildElement("Value").firstChild().nodeValue();
            return vysledek;
        }
    }

    return vysledek;
}
