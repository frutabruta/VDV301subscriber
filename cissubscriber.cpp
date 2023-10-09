#include "cissubscriber.h"


/*
IbisIpSubscriberOnePublisher::IbisIpSubscriberOnePublisher(QString nazevSluzby,QString struktura,QString verze,QString typSluzby, int cisloPortu) : IbisIpSubscriber(nazevSluzby,struktura, verze, typSluzby, cisloPortu)

*/

CisSubscriber::CisSubscriber(QString serviceName,QString structureName,QString version,QString serviceType, int portNumber): IbisIpSubscriberOnePublisher(serviceName,structureName,version, serviceType, portNumber)
{

}
