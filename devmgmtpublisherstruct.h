#ifndef DEVMGMTPUBLISHERSTRUCT_H
#define DEVMGMTPUBLISHERSTRUCT_H



#include "publisherstruct.h"

class DevMgmtPublisherStruct : public PublisherStruct
{
public:
    DevMgmtPublisherStruct();
    DevMgmtPublisherStruct(const PublisherStruct &pubStruct);

    QString deviceId;
    QString deviceClass;
    QString deviceName;

    QString swVersion;
    QString manufacturer;
    QString serialNumber;
    QString status;

    bool hwConfig=false;

    bool isSubscriptionActive=false;


    QMap<QString, QVariant> toQMap();
    bool isInListByIdClass(QVector<DevMgmtPublisherStruct> publisherList);

private:

};

#endif // DEVMGMTPUBLISHERSTRUCT_H
