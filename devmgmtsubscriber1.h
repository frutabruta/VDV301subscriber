#ifndef DEVMGMTSUBSCRIBER1_H
#define DEVMGMTSUBSCRIBER1_H

#include "devmgmtsubscriber.h"

class DevMgmtSubscriber1: public DevMgmtSubscriber
{
public:
    DevMgmtSubscriber1(QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber);
private:
    void allConnects();
};

#endif // DEVMGMTSUBSCRIBER1_H
