#include "devmgmtpublisherstruct.h"
#include <QVariant>

DevMgmtPublisherStruct::DevMgmtPublisherStruct()
{
    port=0;;
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
    return port == node.port && adresa == node.adresa;
}

QMap<QString, QVariant> DevMgmtPublisherStruct::toQMap()
{
    QMap<QString,QVariant> vystup;
    vystup["adresa"]=adresa.toString();
    vystup["port"]=QString::number(port);
    vystup["deviceId"]=deviceId;
    vystup["deviceClass"]=deviceClass;
    vystup["deviceName"]=deviceName;
    vystup["hostname"]=hostname;
    vystup["serviceName"]=serviceName;
    vystup["ibisIpVersion"]=ibisIpVersion;
    vystup["swVersion"]=swVersion;
    vystup["manufacturer"]=manufacturer;
    vystup["serialNumber"]=serialNumber;
    return vystup;
}

bool DevMgmtPublisherStruct::isInListByIdClass(QVector<DevMgmtPublisherStruct> vstup)
{
    foreach(DevMgmtPublisherStruct noveZarizeni, vstup)

    if ((deviceClass==noveZarizeni.deviceClass )&&(deviceId==noveZarizeni.deviceId))
    {
        return true;
    }
    return false;
}

