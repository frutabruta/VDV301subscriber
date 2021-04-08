#ifndef IBISIPSUBSCRIBER_H
#define IBISIPSUBSCRIBER_H

#include <QObject>
#include <QtHttpServer>
#include <QtXml>
#include "VDV301subscriber/newhttpserver.h"
#include "qzeroconf.h"

class IbisIpSubscriber : public QObject
{
    Q_OBJECT
public:
    explicit IbisIpSubscriber(QObject *parent = nullptr);

    IbisIpSubscriber(QString nazevSluzby, QString struktura, QString verze, QString typSluzby, int cisloPortu);
    QByteArray vyrobHlavickuOk();
    void hledejSluzby(QString typsluzby, int start);
    void addService(QZeroConfService zcs);
    QString vytvorSubscribeRequest(QHostAddress ipadresa, int port);
    bool odebirano=false;

private:
    NewHttpServer InstanceNovehoServeru;
    int cisloPortuInterni=0;
    QString nazevSluzbyInterni="";
    QString obsahInterni="";
    QString hlavickaInterni="";
    QString strukturaInterni="";
    QString verzeInterni="";
    QString typSluzbyInterni="_ibisip_http._tcp";
    QZeroConf zeroConf;

    int najdiSluzbu(QString hledanaSluzba, QString hledanaVerze, QZeroConfService zcs);
    QHostAddress projedAdresy();
    void PostSubscribe(QUrl adresaDispleje, QString dataDoPostu);
signals:
    int dataNahrana (QString vysledek);
    void nalezenaSluzba(QZeroConfService zcs);

public slots:
    //void vypisObsahRequestu();
    void vypisObsahRequestu(QString vysledek);

};

#endif // IBISIPSUBSCRIBER_H
