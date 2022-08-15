#ifndef IBISIPSUBSCRIBER_H
#define IBISIPSUBSCRIBER_H

#include <QObject>
#include <QtHttpServer>
#include <QtXml>
#include "httpserversubscriber.h"
#include "QtZeroConf/qzeroconf.h"

class IbisIpSubscriber : public QObject
{
    Q_OBJECT
public:
    explicit IbisIpSubscriber(QObject *parent = nullptr);

    IbisIpSubscriber(QString nazevSluzby, QString struktura, QString verze, QString typSluzby, int cisloPortu);
    QByteArray vyrobHlavickuOk();
    void hledejSluzby(QString typSluzby, int start);

    QString vytvorSubscribeRequest(QHostAddress ipadresa, int port);
    bool odebirano=false;
    bool existujeKandidat=false;

    QTimer *timer = new QTimer(this);
    int defaultniCasovac=120000;
    QVector<QZeroConfService> seznamSluzeb;
    void novePrihlaseniOdberu();

     QNetworkAccessManager manager2;
     QNetworkReply *reply;

     QHostAddress adresaZarizeni;

private:
    HttpServerSubscriber InstanceNovehoServeru;
    // QNetworkAccessManager *manager2 = new QNetworkAccessManager();
  //  QNetworkAccessManager manager2;
    int cisloPortuInterni=0;
    QString nazevSluzbyInterni="";
    QString obsahInterni="";
    QString hlavickaInterni="";
    QString strukturaInterni="";
    QString verzeInterni="";
    QString typSluzbyInterni="_ibisip_http._tcp";
    QZeroConf zeroConf;



    QZeroConfService aktualniSluzbaMdns;
    QZeroConfService kandidatSluzbaMdns;



    int jeSluzbaHledanaVerze(QString hledanaSluzba, QString hledanaVerze, QZeroConfService zcs);
    QHostAddress projedAdresy();
    void PostSubscribe(QUrl adresaDispleje, QString dataDoPostu);
    int vymazSluzbuZeSeznamu(QVector<QZeroConfService> &intSeznamSluzeb, QZeroConfService sluzba);

signals:
    int dataNahrana (QString vysledek);
    void nalezenaSluzba(QZeroConfService zcs);
    void aktualizaceSeznamu();
    void signalZtrataOdberu();

public slots:
    //void vypisObsahRequestu();
    void vypisObsahRequestu(QString vysledek);


private slots:
    void slotCasovacVyprsel();
    void slotOdstranenaSluzba(QZeroConfService zcs);
    void slotAddService(QZeroConfService zcs);
    void slotSubscribeOdeslan(QNetworkReply *rep);
    void httpFinished();
};

#endif // IBISIPSUBSCRIBER_H
