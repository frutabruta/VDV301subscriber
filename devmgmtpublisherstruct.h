#ifndef DEVMGMTPUBLISHERSTRUCT_H
#define DEVMGMTPUBLISHERSTRUCT_H



#include "publisherstruct.h"

class DevMgmtPublisherStruct : public PublisherStruct
{
public:
    DevMgmtPublisherStruct();

    QString deviceId;
    QString deviceClass;
    QString deviceName;



    QString swVersion;
    QString manufacturer;
    QString serialNumber;
    QString status;

    bool hwConfig=false;

    bool isSubscriptionActive=false;



 //   bool operator ==(const DevMgmtPublisherStruct &node) const;
    QMap<QString, QVariant> toQMap();
    bool isInListByIdClass(QVector<DevMgmtPublisherStruct> publisherList);
private:

};

#endif // DEVMGMTPUBLISHERSTRUCT_H
