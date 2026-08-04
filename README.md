# VDV301subscriber 
Qt C++ library to be used in VDV301 subscriber devices

## Tested services 
- CustomerInformationService (1.0, 2.2CZ1.0, 2.3 )
- DeviceManagementService (1.0)

# Changelog
- 20260729_1756
  - VDV301DataStructures
    - StopPointDestination 
      - removed unused includes
  
- 20260729_1900
  - Vdv301SubscriberMinimalExample update 
  
- 20260729_1756
  - VDV301DataStructures
    - added StopNote
  
- 20260629_1612
  - VDV301DataStructures
    - added VehicleState::getCurrentStopPointDestination  
  
- 20260416_0124
  - VDV301DataStructures 
    - added Vdv301CurrentDisplayContent2_3CZ1_0

- 20260225_1928
  - VDV301DataStructures
    - vehicleState secondsDelay
    
- 20260212_2142
  - IbisIpSubscriberOnePublisher
      - added a subscribeCandidateList to handle CIS published on multiple IPs (wait for timeout - then try next)
      - new slot slotSubscriptionFailed
      - IbisIpSubscriberOnePublisher::postSubscribe
        - added debug statements
      - IbisIpSubscriberOnePublisher::slotAddService
        - reduced common parts between overloaded functions
        - subscribeCandidateList handling
      - IbisIpSubscriberOnePublisher::slotSubscriptionFailed
        - new slot
  - HttpServerSubscriber::route
    - added a logging statement
  - IbisIpSubscriber::isTheServiceRequestedOne
    - merging content of overloaded function
  - IbisIpSubscriber::selectNonLoopbackAddress
    - added debug statements
  - IbisIpSubscriber::slotHttpRequestFinished
    - added debug statements
    - usage of signalIsSubscriptionSuccesful
  - signal moved to IbisIpSubscriber::slotHttpRequestErrorHappened
  - new signal signalAddressUpdate
  - IbisIpSubscriberMultiplePublishers::slotAddServiceManual
    - different call for isTheServiceRequestedOne



- 20260206_1415
  - HttpServerSubscriber
    - implemented QLoggingCategory

- 20260108_1715
  - DevMgmtSubscriber::postSetDeviceConfiguration
    - reply pointer fix
  - DevMgmtSubscriber::slotSetSetDeviceConfigurationFinished
    - reply pointer fix
  - DevMgmtSubscriber2
    - removed duplicate functions - using ones from base function instead
      - slotHttpRequestSubscriptionFinished
      - slotHttpRequestUnsubscriptionFinished
      - postSubscribe
      - postUnsubscribe
      
- 20260108_1509
  - IbisIpSubscriber::slotHttpRequestGenericFinished() 
    - reworked to IbisIpSubscriber::slotHttpRequestFinished
    - new signal signalResponseNotEmpty(QString response);
  - IbisIpSubscriberOnePublisher::slotHandleResponseContent
    - new slot
  - removed slots
    - IbisIpSubscriberOnePublisher::slotHttpRequestSubscriptionFinished();
    - IbisIpSubscriberOnePublisher::slotHttpRequestUnsubscriptionFinished();
    - IbisIpSubscriberOnePublisher::slotSubscribeSent(QNetworkReply *subscriptionReply);
    - IbisIpSubscriberMultiplePublishers::slotHttpRequestUnsubscriptionFinished();
    - IbisIpSubscriberMultiplePublishers::slotHttpRequestSubscriptionFinished();
- 20251220_1735
  - IbisIpSubscriber::postGenericRequest
    - reply defined here
  - rewritten to achieve better pairing of response and request and already deleted response
    - IbisIpSubscriber::slotHttpRequestGenericFinished
    - IbisIpSubscriberMultiplePublishers::slotHttpRequestSubscriptionFinished
    - IbisIpSubscriberMultiplePublishers::slotHttpRequestUnsubscriptionFinished 
    - IbisIpSubscriberOnePublisher::slotHttpRequestSubscriptionFinished
    - IbisIpSubscriberOnePublisher::slotHttpRequestUnsubscriptionFinished
    
- 20251129_0134
    - qCDebug implementations:
      - IbisIpSubscriber
      - IbisIpSubscriberOnePublisher 
      
- 20251118_1558
  - replyPath fix for http server (worked only with xxx)
  - added HttpServerSubscriber::setReplyPath
  - IbisIpSubscriber::start
    - added httpserver set reply path
  - IbisIpSubscriberOnePublisher::start
    - added httpserver set reply path
- 20251028_1939
  - Vdv301DataStructures 
    - Trip == operator fix

- 20251028_1435
  - Vdv301DataStructures 
    - vehicleRef prevent reset
    
- 20251025_2304
  - DevMgmtSubscriber::start
    - modified to allow not starting in constructor
- 20251021_0133
  - IbisIpSubscriber
    - added blockBonjour function    
    - IbisIpSubscriber::selectNonLoopbackAddress
      - added allow-loopback option
    - IbisIpSubscriber::selectNonLoopbackAddressInSubnet
      - enable finding in loopback if no other addresses available
  - IbisIpSubscriberOnePublisher::slotAddService
  - IbisIpSubscriberOnePublisher::slotAddServiceManual
    - reworked using PublisherStruct

- 20251007_1642
  - IbisIpSubscriberOnePublisher::slotAddService
    - temporary fix of sending empty request when nonloopback address is not found
- 20250924_1550
    - XmlGeneratorSubscriber::createUnsubscribeRequest
      - header fix
- 20250924_1509
    - CisSubscriber
      - added replyPath attribute
    - DevMgmtSubscriber2::slotHttpRequestSubscriptionFinished
      - null pointer fix
      - empty result check
    - HttpServerSubscriber::route
      - added replyPath handeling and support for / in beginning
    - IbisIpSubscriber
      - added replyPath support
      - changed reply type to QPointer<QNetworkReply> to fix crashes
    - IbisIpSubscriberOnePublisher
      - added replyPath support
    - XmlGeneratorSubscriber::createSubscribeRequest
      - simplified using value function
      - added replyPath
    - XmlGeneratorSubscriber::createUnsubscribeRequest
      - simplified using value function
      - added replyPath


- 20250624_1721
    - VDV301DataStructures
        - Stop Point

- 20250520_1347
    - fixed Vdv301Datastructures VDV301CurrentDisplayContent dependency
- 20250424_2248
    -   DevMgmtPublisherStruct
        - added new constructor from PublisherStruct
    - PublisherStruct
        - added new constructor PublisherStruct::PublisherStruct(QZeroConfService zcs)
    - DevMgmtSubscriber
        - removed legacy function DevMgmtSubscriber::getDeviceInformation(QZeroConfService zcs)
        - removed legacy function DevMgmtSubscriber::getDeviceConfiguration(QZeroConfService zcs)
        - DevMgmtSubscriber::slotNewDnsSd use of DevMgmtPublisherStruct newDevice(zcs)
        - DevMgmtSubscriber::slotRemoveDnsSd use of DevMgmtPublisherStruct newDevice(zcs)
    - DevMgmtSubscriber2
        - DevMgmtSubscriber2::slotNewDnsSd use of DevMgmtPublisherStruct newDevice(zcs)
    - IbisIpSubscriber
        - new function IbisIpSubscriber::createSubscribeDestination (moved from IbisIpSubscriberMultiplePublishers)
    - IbisIpSubscriberMultiplePublishers
        - IbisIpSubscriberMultiplePublishers::slotAddServiceManual    
        - IbisIpSubscriberMultiplePublishers::slotNewDnsSd refactoring using tryToAddPublisher
        - IbisIpSubscriberMultiplePublishers::slotUpdateDnsSd refactoring using tryToAddPublisher
        - new function IbisIpSubscriberMultiplePublishers::tryToAddPublisher


- 20250424_1926
    - PassengerCountingService first version
    - moved variables from DevMgmtPublisherStruct to new base class PublisherStruct
    - DevMgmtSubscriber
        - has new base class IbisIpSubscriberMultiplePublishers
    - IbisIpSubscriber
        - new function IbisIpSubscriber::isTheServiceRequestedOne
        - new function IbisIpSubscriber::postGenericRequest
        - new slot IbisIpSubscriber::slotHttpRequestGenericFinished
    - IbisIpSubscriberMultiplePublishers
        - new class
    - IbisIpSubscriberOnePublisher
        - IbisIpSubscriberOnePublisher::postGenericRequest moved to IbisIpSubscriber
        - bisIpSubscriberOnePublisher::slotHttpRequestGenericFinished  moved to IbisIpSubscriber
        - new function IbisIpSubscriberOnePublisher::slotAddServiceManual
        - new function IbisIpSubscriberOnePublisher::slotAddServiceManualForce
    - PcsPublisherStruct 
        - new class
    - PcsSubscriber
        - new class
    - PublisherStruct 
        - new class
    - Vdv301PcsAllData
        - new class
    - XmlGeneratorSubscriberPcs
        - new class
    - XmlParserPcs
        - new class    
- 20250404_1836
    - VDV301DataStructures
        - added VehicleModeEnumeration
        - new function Vdv301Enumerations::RouteDeviationEnumeration RouteDeviationEnumerationFromQString
- 20250403_2146
    - IbisIpSubscriber::structureName added setter and getter
    - VDV301DataStructures
        - added Vdv301CurrentDisplayContent

- 20250331_1912
    - new signal IbisIpSubscriber::signalError
    - new function IbisIpSubscriberOnePublisher::postGenericRequest
    - new function IbisIpSubscriberOnePublisher::slotHttpRequestGenericFinished
    - added checks to empty service IbisIpSubscriberOnePublisher::unsubscribe
- 20250315_2147
    - VDV301DataStructures
        - additional announcement overhaul
- 2024_02_23_1638
    - VDV301DataStructures
        - added VDV301Trip::additionalTextMessage1-9List
- 2024_12_06_1805
    - all displayContentClassEnumeration changed from Inner to Interior
- 2024_12_06
    - VDV301DataStructures
        - new function AllData isOnLastStop
- 2024_11_03
    - VDV301DataStructures
        - added Vdv301Connection related variables
- 2024_10_30
    - new version of VDV301DataStructures with 2.3CZ1.0 versions
- 2024_08_23
    - VDV301DataStructures
        - DoorOpenStateEnumerationFromQString
- 2024_06_06
    - new folder with examples
        - basic example Vdv301SubscriberMinimalExample
- 2024_05_14
    - cant be compiled with 5.15 again (functionality not guaranteed)
- 2024_04_22
    - DevMgmtSubscriber2
        - *createSubscribeRequest* now generated by *xmlGeneratorSubscriber*
    - new function *XmlParserSubscriber::dataAcceptedResponseToBoolean*

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
