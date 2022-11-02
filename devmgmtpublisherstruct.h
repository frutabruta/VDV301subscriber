#ifndef DEVMGMTPUBLISHERSTRUCT_H
#define DEVMGMTPUBLISHERSTRUCT_H

#include <QHostAddress>

class DevMgmtPublisherStruct
{
public:
    DevMgmtPublisherStruct();
    QHostAddress adresa;
    int port;
    QString deviceId;
    QString deviceClass;
    QString deviceName;
    QString hostname;
    QString serviceName;
    QString ibisIpVersion;
    QString swVersion;
    QString manufacturer;
    QString serialNumber;



    bool operator ==(const DevMgmtPublisherStruct &node) const;
private:

};

#endif // DEVMGMTPUBLISHERSTRUCT_H
