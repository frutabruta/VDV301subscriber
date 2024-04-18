# VDV301subscriber #
Qt C++ library to be used in VDV301 subscriber devices

## Tested services ##
- CustomerInformationService (1.0, 2.2CZ1.0, 2.3 )
- DeviceManagementService (1.0)

# Changelog # 
- 2024_04_18
    - functions moved from IbisIpSubscriber to XmlGeneratorSubscriber
        - *createSubscribeRequest*
        - *createUnsubscribeRequest*
    - IbisIpSubscriber
        - new variable *mSubnetMask* (default value 16)
        - new function *selectNonLoopbackAddressInSubnet* to allow sending correct IP address in subscribe request to publishers in different networks when subscriber has more than one network interface.
    - IbisIpSubscriberOnePublisher
        - slotAddService 
            - now contains *deviceAddress=selectNonLoopbackAddressInSubnet(zcs->ip(),mSubnetMask);*

- 2024_02_15
    - new class XmlParserSubscriber
        - functions moved from DevMgmtSubscriber
            - *deviceInformationToDevice*
            - *deviceConfigurationToDevice*
            - *deviceStatusToDevice*
    - new class XmlGeneratorSubscriber
        -
    - DevMgmtSubscriber
        - new function *postSetDeviceConfiguration*
        - new slot *slotSetSetDeviceConfigurationFinished*
    - DevMgmtSubscriber2
        - *signalIsUnsubscriptionSuccesful* renamed to *signalIsUnsubscriptionSuccessful*
        - *signalIsSubscriptionSuccesful2* renamed to *signalIsSubscriptionSuccessful2*
        

- 2024_02_04
    - DevMgmtPublisherStruct
        - added variable *status*
        - added variable *isSubscriptionActive*
    - DevMgmtSubscriber
        - changed into base class for *DevMgmtSubscriber1* and *DevMgmtSubscriber1*, due to necessity to define function *allConnects* separately
        - new functions
            - *getStatus*
            - *deviceInformationToDevice*
            - *deviceConfigurationToDevice*
            - *deviceStatusToDevice*
        - removed function
            - *allConnects()* (moved to *DevMgmtSubscriber1*)
        - functions modified
            - *slotRequestReceived* simplified
        - added *start* to the constructor

    - DevMgmtSubscriber1
        - new class, inherits from *DevMgmtSubscriber*
        - based on DeviceManagementService 1.0
        - supports only *Get*  operations
    - DevMgmtSubscriber2
        - new class, inherits from *DevMgmtSubscriber*
        - supports Subscribe operations on *DeviceStatus*
        - based on DeviceManagementService 2.2
        - matches received status to devices by IP address
    - HttpServerSubscriber
        - new class *HttpServerRequest*
        - new signal *signalWholeRequest*
            - includes IP adress and port of the request sender
        - *HttpServerSubscriber::route* modified to send *signalWholeRequest*
    -IbisIpSubscriber
        - new function *start*
        - added getter and setter for *mPortNumber

- 2024_01_04
    - fixed expected unsubscription response
        - unsubscription in now only accepted if Active=false in Unsubscription response
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
