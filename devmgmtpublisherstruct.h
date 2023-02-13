#ifndef DEVMGMTPUBLISHERSTRUCT_H
#define DEVMGMTPUBLISHERSTRUCT_H

#include <QHostAddress>
#include <QMap>

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

    bool hwConfig=false;



    bool operator ==(const DevMgmtPublisherStruct &node) const;
    QMap<QString, QVariant> toQMap();
    bool isInListByIdClass(QVector<DevMgmtPublisherStruct> vstup);
private:

};

#endif // DEVMGMTPUBLISHERSTRUCT_H
