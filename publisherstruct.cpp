#include "publisherstruct.h"

PublisherStruct::PublisherStruct() {}

PublisherStruct::PublisherStruct(QZeroConfService zcs) {
    serviceName=zcs->name();
    hostAddress=zcs->ip();
    portNumber=zcs->port();
    hostname=zcs->host();
    ibisIpVersion=zcs.data()->txt().value("ver");
    servicePath=zcs.data()->txt().value("path", "");
}


QMap<QString, QVariant> PublisherStruct::toQMap()
{
    QMap<QString,QVariant> output;
    output["adresa"]=hostAddress.toString();
    output["port"]=QString::number(portNumber);
    output["hostname"]=hostname;
    output["serviceName"]=serviceName;
    output["ibisIpVersion"]=ibisIpVersion;
    output["servicePath"]=servicePath;

    return output;
}


QString PublisherStruct::dumpToQString()
{

    QStringList list;
    list<<hostAddress.toString();
    list<<QString::number(portNumber);
    list<<hostname;
    list<<serviceName;
    list<<ibisIpVersion;
    list<<servicePath;

    return list.join(" ");
}


bool PublisherStruct::operator==(const PublisherStruct &node) const
{
    return portNumber == node.portNumber && hostAddress == node.hostAddress && servicePath == node.servicePath;
}
