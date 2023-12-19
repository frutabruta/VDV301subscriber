# VDV301subscriber #
Qt C++ library to be used in VDV301 subscriber devices

## Tested services ##
- CustomerInformationService (1.0, 2.2CZ1.0, 2.3 )
- DeviceManagementService (1.0)

# Changelog # 
- 2023_12_19_1138
    - subscription in now only accepted if Active=true in Subscription response
    - check for errors in http response in Subscribe and Unsubscribe
- 2023_12_19
    - new signal signalIsUnsubscriptionSuccesful
    - unsubscription in now only accepted if Active=true in Unsubscription response
- 2023_12_14
    - HttpServerSubscriber
        - removed options from HttpServerSubscriber::route which were copied from VDV301publisher
        - variable contentSubscribe renamed to subscribeResponseContent
    - IbisIpSubscriber
        - new function IbisIpSubscriber::createUnsubscribeRequest
    - IbisIpSubscriberOnePublisher
        - new function IbisIpSubscriberOnePublisher::postUnsubscribe
        - new function IbisIpSubscriberOnePublisher::checkExistingServices
        - new function  IbisIpSubscriberOnePublisher::unsubscribe
        - new slot IbisIpSubscriberOnePublisher::slotHttpRequestUnsubscriptionFinished()
        - slotHttpFinished() renamed to slotHttpRequestSubscriptionFinished()

- 2023_11_20
    - HttpServerSubscriber port now changes correctly when IbisIpSubscriberOnePublisher::setPortNumber is called
- 2023_11_08
    - IbisIpSubscriberOnePublisher
        - fix of unsuccessful subscription when service name updates (CustomerInfromationService (2) etc.)
- 2023_10_30
    - DevMgmtSubscriber
        - fix of overwritting IbisIpSubscriber by DevMgmtSubscriber (caused empty serviceType)
- 2023_10_17
    - empty IP address fix (check for empty subscriber IP before subscribing )
- 2023_10_14
    - added fix when QZeroConf::serviceAdded doesn't trigger (program now reacts to QZeroConf::serviceUpdated too)

- 2023_10_09
    - translation of variables and functions to english
- 2023_08_30
    - oprava syntaxe funkce connect
    - oprava chyby ASSERT failure, vytvoření destruktoru
    - změna definice QNetworkAccessManager bez "new"

- 2023_07_04
    - IbisSibscriberOnePublisher
        - přesunutí vsechnyConnecty() z konstruktoru
