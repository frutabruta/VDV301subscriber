#ifndef DEVMGMTPUBLISHERSTRUCT_H
#define DEVMGMTPUBLISHERSTRUCT_H

#include <QHostAddress>
#include <QMap>

class DevMgmtPublisherStruct
{
public:
    DevMgmtPublisherStruct();
    QHostAddress hostAddress;
    int portNumber;
    QString deviceId;
    QString deviceClass;
    QString deviceName;
    QString hostname;
    QString serviceName;
    QString ibisIpVersion;
    QString swVersion;
    QString manufacturer;
    QString serialNumber;
    QString status;

    bool hwConfig=false;

    bool isSubscriptionActive=false;



    bool operator ==(const DevMgmtPublisherStruct &node) const;
    QMap<QString, QVariant> toQMap();
    bool isInListByIdClass(QVector<DevMgmtPublisherStruct> publisherList);
private:

};

#endif // DEVMGMTPUBLISHERSTRUCT_H
