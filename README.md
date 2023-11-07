# VDV301subscriber #
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
