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

    //konstruktor a destruktor
    explicit IbisIpSubscriber(QObject *parent = nullptr);
    IbisIpSubscriber(QString nazevSluzby, QString struktura, QString verze, QString typSluzby, int cisloPortu);

    //instance knihoven
    QNetworkAccessManager manager2;
    QNetworkReply *reply;

    //promenne
    QVector<QZeroConfService> seznamSluzeb;
    QHostAddress adresaZarizeni;

    //funkce
    QByteArray vyrobHlavickuOk();
    void hledejSluzby(QString typSluzby, int start);
  //  void novePrihlaseniOdberu();
    QString vytvorSubscribeRequest(QHostAddress ipadresa, int port);


    //nezarazeno
private:

    //instance knihoven


    //promenne




    //funkce
    QHostAddress projedAdresy();


    void vsechnyConnecty();

    //nezarazeno


protected:

    //instance knihoven
      HttpServerSubscriber InstanceNovehoServeru;

    //promenne
    QZeroConf zeroConf;
    QString typSluzbyInterni="_ibisip_http._tcp";

        int cisloPortuInterni=0;
    QString hlavickaInterni="";
    QString nazevSluzbyInterni="";
  //  QString obsahInterni="";
    QString strukturaInterni="";
    QString verzeInterni="";



    //funkce
    int jeSluzbaHledanaVerze(QString hledanaSluzba, QString hledanaVerze, QZeroConfService zcs);
    void PostSubscribe(QUrl adresaDispleje, QString dataDoPostu);
    int vymazSluzbuZeSeznamu(QVector<QZeroConfService> &intSeznamSluzeb, QZeroConfService sluzba);


    //ostatni




signals:
    int dataNahrana (QString vysledek);
 //   void nalezenaSluzba(QZeroConfService zcs);
    void signalAktualizaceSeznamu();
    void signalZtrataOdberu();

public slots:
    //void vypisObsahRequestu();
 //   void vypisObsahRequestu(QString vysledek);


private slots:

  //  void slotOdstranenaSluzba(QZeroConfService zcs);
  //  void slotAddService(QZeroConfService zcs);
  //  void slotSubscribeOdeslan(QNetworkReply *rep);
  //  void httpFinished();
};

#endif // IBISIPSUBSCRIBER_H
