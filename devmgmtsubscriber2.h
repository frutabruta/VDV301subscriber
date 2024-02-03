#ifndef DEVMGMTSUBSCRIBER2_H
#define DEVMGMTSUBSCRIBER2_H

#include "devmgmtsubscriber.h"

class DevMgmtSubscriber2 : public DevMgmtSubscriber
{
    Q_OBJECT
public:
    //DevMgmtSubscriber2();
    DevMgmtSubscriber2(QString serviceName, QString structureName, QString version, QString serviceType, int portNumber);
private slots:
    void slotNewDnsSd(QZeroConfService zcs);
    void slotHttpRequestSubscriptionFinished();
    void slotHttpRequestUnsubscriptionFinished();
    void slotHandleWholeRequest(HttpServerRequest receivedData);
private:
    void postSubscribe(QUrl subscriberAddress, QString postRequestContent);
    void postUnsubscribe(QUrl subscriberAddress, QString postRequestContent);
    void allConnects();
    void slotHandleReceivedData(QString receivedData);
    int indexOfSubscriberOnListByIp(QVector<DevMgmtPublisherStruct> deviceList, QHostAddress hostAddress);
signals:
    void signalSubscriptionSuccessful(QZeroConfService zcs);
    void signalIsSubscriptionSuccesful2(bool result);
    void signalUnsubscriptionSuccessful(QZeroConfService zcs);
    void signalIsUnsubscriptionSuccesful(bool result);

};

#endif // DEVMGMTSUBSCRIBER2_H
