#ifndef IBISIPSUBSCRIBERONEPUBLISHER_H
#define IBISIPSUBSCRIBERONEPUBLISHER_H

#include "ibisipsubscriber.h"


class IbisIpSubscriberOnePublisher : public IbisIpSubscriber
{
    Q_OBJECT
public:


    //konstruktor a destruktor
    explicit IbisIpSubscriberOnePublisher(QString nazevSluzby,QString struktura,QString verze,QString typSluzby, int cisloPortu);

    //instance knihoven
    QTimer *timer = new QTimer(this);
    //promenne
    int defaultniCasovac=120000;
    bool odebirano=false;
    bool existujeKandidat=false;

    QZeroConfService aktualniSluzbaMdns;
    QZeroConfService kandidatSluzbaMdns;

    //funkce


    void novePrihlaseniOdberu();
    void slotVypisObsahRequestu(QString vysledek);
    void PostSubscribe(QUrl adresaDispleje, QString dataDoPostu);
    void start();

private:
    void vsechnyConnecty();

public slots:
    void slotCasovacVyprsel();
    void slotAddService(QZeroConfService zcs);
private slots:
    void slotHttpFinished();
    void slotSubscribeOdeslan(QNetworkReply *rep);
    void slotOdstranenaSluzba(QZeroConfService zcs);

signals:
    void signalUspesnySubscribe(QZeroConfService zcs);


    //instance knihoven

    //promenne


    //funkce

};

#endif // IBISIPSUBSCRIBERONEPUBLISHER_H
