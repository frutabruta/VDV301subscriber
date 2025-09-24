#include "cissubscriber.h"

CisSubscriber::CisSubscriber(QString serviceName,QString structureName,QString version,QString serviceType, int portNumber, QString replyPath): IbisIpSubscriberOnePublisher(serviceName,structureName,version, serviceType, portNumber, replyPath)
{

}
