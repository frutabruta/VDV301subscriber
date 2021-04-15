#ifndef IBISIPSUBSCRIBER_H
#define IBISIPSUBSCRIBER_H

#include <QObject>
#include <QtHttpServer>
#include <QtXml>
#include "newhttpserver.h"
#include "QtZeroConf/qzeroconf.h"

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
    QTimer *timer = new QTimer(this);
     int defaultniCasovac=120000;
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

private slots:
    void casovacVyprsel();
};

#endif // IBISIPSUBSCRIBER_H
