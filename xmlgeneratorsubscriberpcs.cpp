#include "xmlgeneratorsubscriberpcs.h"

XmlGeneratorSubscriberPcs::XmlGeneratorSubscriberPcs() {}


QString XmlGeneratorSubscriberPcs::startCounting(int doorNumber)
{
    QString result="";
    result+="<PassengerCountingService.StartCountingRequest>"
              "<DoorIdList>"
              "<DoorID>"
              "<Value>";
    result+=QString::number(doorNumber);
    result+="</Value>"
              "</DoorID>"
              "</DoorIdList>"
              "</PassengerCountingService.StartCountingRequest>";
    return result;
}


QString XmlGeneratorSubscriberPcs::stopCounting(int doorNumber)
{
    QString result="";
    result+="<PassengerCountingService.StopCountingRequest>"
              "<DoorIdList>"
              "<DoorID>"
              "<Value>";
    result+=QString::number(doorNumber);
    result+="</Value>"
              "</DoorID>"
              "</DoorIdList>"
              "</PassengerCountingService.StopCountingRequest>";
    return result;
}


QString XmlGeneratorSubscriberPcs::setCounterData(int doorNumber)
{
    QString result="";
    result+="<PassengerCountingService.SetCounterDataRequest>"
              "<DoorSetList>"
              "<DoorID>"
              "<Value>";
    result+=QString::number(doorNumber);



    result+="</Value>"
              "</DoorID>";

    result+= R"(
    <CountSet>
        <ObjectClass>Adult</ObjectClass>
        <In>
        <Value>0</Value>
        </In>
        <Out>
        <Value>0</Value>
        </Out>
        </CountSet>
        <CountSet>
        <ObjectClass>Child</ObjectClass>
        <In>
        <Value>0</Value>
        </In>
        <Out>
        <Value>0</Value>
        </Out>
        </CountSet>
        <CountSet>
        <ObjectClass>Bike</ObjectClass>
        <In>
        <Value>0</Value>
        </In>
        <Out>
        <Value>0</Value>
        </Out>
        </CountSet>
        <CountSet>
        <ObjectClass>WheelChair</ObjectClass>
        <In>
        <Value>0</Value>
        </In>
        <Out>
        <Value>0</Value>
        </Out>
        </CountSet>

    )";


     result+=  "</DoorSetList>"
               "</PassengerCountingService.SetCounterDataRequest>";

    return result;
}
