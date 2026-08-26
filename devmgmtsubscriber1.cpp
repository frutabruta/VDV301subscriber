#include "devmgmtsubscriber1.h"

DevMgmtSubscriber1::DevMgmtSubscriber1(QZeroConf& zeroConf, QString serviceName, QString structureName, QString  version, QString  serviceType, int portNumber)
    : DevMgmtSubscriber(zeroConf, serviceName,structureName, version,serviceType, portNumber)
{
    qDebug()<<Q_FUNC_INFO;
    allConnects();
}


void DevMgmtSubscriber1::allConnects()
{
    qDebug()<<Q_FUNC_INFO;
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &DevMgmtSubscriber::slotNewDnsSd);
    // connect(&zeroConf, &QZeroConf::serviceUpdated, this, &DevMgmtSubscriber::slotNewDnsSd);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &DevMgmtSubscriber::slotRemoveDnsSd);
    connect(&manager,&QNetworkAccessManager::finished,this,&DevMgmtSubscriber::slotRequestReceived);
    connect(this,&DevMgmtSubscriber::downloadFinished,this,&DevMgmtSubscriber::slotHandleData);

}
