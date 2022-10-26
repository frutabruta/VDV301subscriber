#ifndef DEVMGMTSUBSCRIBER_H
#define DEVMGMTSUBSCRIBER_H
#include "ibisipsubscriber.h"
#include "devmgmtpublisherstruct.h"
#include "QObject"
class DevMgmtSubscriber : public IbisIpSubscriber
{
      Q_OBJECT
public:

    //konstruktor
    explicit DevMgmtSubscriber(QString nazevSluzby, QString struktura, QString  verze, QString  typSluzby, int cisloPortu);

    //instance knihoven
     QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    //promenne
    int cisloPortu=0;
    QString nazevSluzby="";
      QVector<DevMgmtPublisherStruct> seznamZarizeni;
    QString struktura="";
     QString typSluzby="";
    QString verze="";



    //funkce
    bool getDeviceConfiguration(DevMgmtPublisherStruct &zarizeni, QZeroConfService zcs);
    bool getDeviceInformation(DevMgmtPublisherStruct &zarizeni, QZeroConfService zcs);

    QString getVersion(QDomDocument document, QString element);
private:

    //konstruktor

    //instance knihoven

    //promenne

    //funkce


    void vsechnyConnecty();
public slots:

    void slotNewDnsSd(QZeroConfService zcs);
    void slotZpracujData(QString vstup);
    QByteArray slotRequestReceived(QNetworkReply *reply);
private slots:


signals:
    //void aktualizaceSeznamu();
    void stazeniHotovo(QString vystup);

};

#endif // DEVMGMTSUBSCRIBER_H
