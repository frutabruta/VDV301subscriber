#ifndef DEVMGMTSUBSCRIBER_H
#define DEVMGMTSUBSCRIBER_H
#include "ibisipsubscriber.h"
#include "devmgmtpublisherstruct.h"

class DevMgmtSubscriber : public IbisIpSubscriber
{
    Q_OBJECT
public:

    //konstruktor
    explicit DevMgmtSubscriber(QString nazevSluzby, QString struktura, QString  verze, QString  typSluzby, int cisloPortu);
    ~DevMgmtSubscriber();
    //instance knihoven
    QNetworkAccessManager manager; // = new QNetworkAccessManager(this);
    //promenne
    int cisloPortu=0;
    QString nazevSluzby="";
    QVector<DevMgmtPublisherStruct> seznamZarizeniDetekce;
    QVector<DevMgmtPublisherStruct> seznamZarizeniKonfigurace;
    QString struktura="";
    QString typSluzby="";
    QString verze="";



    //funkce
    bool getDeviceConfiguration(QZeroConfService zcs);
    bool getDeviceInformation(QZeroConfService zcs);

    QString getVersion(QDomDocument document, QString element);
    bool getDeviceInformation(DevMgmtPublisherStruct &zarizeni);
    bool getDeviceConfiguration(DevMgmtPublisherStruct &zarizeni);
private:

    //konstruktor

    //instance knihoven

    //promenne

    //funkce


    void vsechnyConnecty();

public slots:

    void slotNewDnsSd(QZeroConfService zcs);
    void slotRemoveDnsSd(QZeroConfService zcs);
    void slotZpracujData(QString vstup);
    QByteArray slotRequestReceived(QNetworkReply *reply);
    void slotAktualizaceZarizeni();
private slots:


signals:
    //void aktualizaceSeznamu();
    void stazeniHotovo(QString vystup);


};

#endif // DEVMGMTSUBSCRIBER_H
