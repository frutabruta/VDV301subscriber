#ifndef PUBLISHERSTRUCT_H
#define PUBLISHERSTRUCT_H

#include <QHostAddress>
#include <QVariant>

class PublisherStruct
{
public:
    PublisherStruct();

    QHostAddress hostAddress;
    int portNumber;
    QString hostname;

    QString serviceName;
    QString ibisIpVersion;


    QMap<QString, QVariant> toQMap();
    bool operator ==(const PublisherStruct &node) const;
    QString dumpToQString();
};

#endif // PUBLISHERSTRUCT_H
