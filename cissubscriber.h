#ifndef CISSUBSCRIBER_H
#define CISSUBSCRIBER_H
#include "ibisipsubscriberonepublisher.h"

class CisSubscriber : public IbisIpSubscriberOnePublisher
{
      Q_OBJECT
public:

    explicit CisSubscriber(QString nazevSluzby, QString struktura, QString verze, QString typSluzby, int cisloPortu);
};

#endif // CISSUBSCRIBER_H
