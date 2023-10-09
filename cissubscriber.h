#ifndef CISSUBSCRIBER_H
#define CISSUBSCRIBER_H
#include "ibisipsubscriberonepublisher.h"

class CisSubscriber : public IbisIpSubscriberOnePublisher
{
      Q_OBJECT
public:

    explicit CisSubscriber(QString serviceName, QString structureName, QString version, QString serviceType, int portNumber);
};

#endif // CISSUBSCRIBER_H
