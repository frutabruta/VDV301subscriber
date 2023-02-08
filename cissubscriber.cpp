#include "cissubscriber.h"


/*
IbisIpSubscriberOnePublisher::IbisIpSubscriberOnePublisher(QString nazevSluzby,QString struktura,QString verze,QString typSluzby, int cisloPortu) : IbisIpSubscriber(nazevSluzby,struktura, verze, typSluzby, cisloPortu)

*/

CisSubscriber::CisSubscriber(QString nazevSluzby,QString struktura,QString verze,QString typSluzby, int cisloPortu): IbisIpSubscriberOnePublisher(nazevSluzby,struktura,verze, typSluzby, cisloPortu)
{

}
