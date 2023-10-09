#include "devmgmtpublisherstruct.h"
#include <QVariant>

DevMgmtPublisherStruct::DevMgmtPublisherStruct()
{
    portNumber=0;;
    deviceId="";
    deviceClass="";
    deviceName="";
    hostname="";
    serviceName="";
    ibisIpVersion="";
    swVersion="";
    manufacturer="";
    serialNumber="";
}




bool DevMgmtPublisherStruct::operator==(const DevMgmtPublisherStruct &node) const
{
    return portNumber == node.portNumber && hostAddress == node.hostAddress;
}

QMap<QString, QVariant> DevMgmtPublisherStruct::toQMap()
{
    QMap<QString,QVariant> output;
    output["adresa"]=hostAddress.toString();
    output["port"]=QString::number(portNumber);
    output["deviceId"]=deviceId;
    output["deviceClass"]=deviceClass;
    output["deviceName"]=deviceName;
    output["hostname"]=hostname;
    output["serviceName"]=serviceName;
    output["ibisIpVersion"]=ibisIpVersion;
    output["swVersion"]=swVersion;
    output["manufacturer"]=manufacturer;
    output["serialNumber"]=serialNumber;
    return output;
}

bool DevMgmtPublisherStruct::isInListByIdClass(QVector<DevMgmtPublisherStruct> publisherList)
{
    foreach(DevMgmtPublisherStruct selectedPublisher, publisherList)

    if ((deviceClass==selectedPublisher.deviceClass )&&(deviceId==selectedPublisher.deviceId))
    {
        return true;
    }
    return false;
}

