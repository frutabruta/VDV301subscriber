#include "ibisipsubscriberonepublisher.h"

IbisIpSubscriberOnePublisher::IbisIpSubscriberOnePublisher(QString nazevSluzby,QString struktura,QString verze,QString typSluzby, int cisloPortu) : IbisIpSubscriber(nazevSluzby,struktura, verze, typSluzby, cisloPortu)
{
    qDebug()<<Q_FUNC_INFO;

    vsechnyConnecty();

    timer->start(defaultniCasovac);
}


void IbisIpSubscriberOnePublisher::vsechnyConnecty()
{
    qDebug()<<Q_FUNC_INFO;

        connect(timer, &QTimer::timeout, this, &IbisIpSubscriberOnePublisher::slotCasovacVyprsel);

        connect(&InstanceNovehoServeru,&HttpServerSubscriber::prijemDat,this,&IbisIpSubscriberOnePublisher::slotVypisObsahRequestu) ;
        connect(&zeroConf, &QZeroConf::serviceAdded, this, &IbisIpSubscriberOnePublisher::slotAddService);
        connect(&zeroConf, &QZeroConf::serviceRemoved, this, &IbisIpSubscriberOnePublisher::slotOdstranenaSluzba);
}

void IbisIpSubscriberOnePublisher::novePrihlaseniOdberu()
{
    qDebug() <<  Q_FUNC_INFO;
    odebirano=false;
    existujeKandidat=false;
    hledejSluzby(typSluzbyInterni,1);
}



void IbisIpSubscriberOnePublisher::PostSubscribe(QUrl adresaDispleje, QString dataDoPostu)
{

    qDebug() <<  Q_FUNC_INFO;
    qDebug()<<"postuju na adresu "<<adresaDispleje<<" "<<dataDoPostu;

    QNetworkRequest pozadavekPOST(adresaDispleje);
    qDebug()<<"A";

    // https://stackoverflow.com/a/53556560

    pozadavekPOST.setTransferTimeout(30000);
    pozadavekPOST.setRawHeader("Content-Type", "text/xml");
    //pozadavekPOST.setRawHeader("Expect", "100-continue");
    //pozadavekPOST.setRawHeader("Connection", "keep-Alive");


    //pozadavekPOST.setRawHeader("Accept-Encoding", "gzip, deflate");

    QByteArray dataDoPostuQByte=dataDoPostu.toUtf8() ;





    qDebug()<<"B";


    //QNetworkAccessManager *manager2 = new QNetworkAccessManager();


    //   connect(manager2,SIGNAL( (QNetworkReply*)),this,SLOT(slotSubscribeOdeslan(QNetworkReply*)),Qt::DirectConnection);
    //    connect(manager2,SIGNAL(finished(QNetworkReply*)),this,SLOT(slotSubscribeOdeslan(QNetworkReply*)),Qt::DirectConnection);

    reply=manager2.post(pozadavekPOST,dataDoPostuQByte);
    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberOnePublisher::slotHttpFinished);


    //  connect(manager2,SIGNAL(finished(QNetworkReply*)),manager2,SLOT(deleteLater()),Qt::DirectConnection);

    qDebug()<<"C";




    connect(reply, &QNetworkReply::finished, this, &IbisIpSubscriberOnePublisher::slotHttpFinished);

    qDebug()<<"D";



    qDebug()<<"E";
}

void IbisIpSubscriberOnePublisher::slotAddService(QZeroConfService zcs)
{

    qDebug() <<  Q_FUNC_INFO;
    // QTableWidgetItem *cell;
    // qDebug() << "Added service: " << zcs;
    QString nazev=zcs->name();
    QString ipadresa=zcs->ip().toString();
    QString verze=zcs.data()->txt().value("ver");
    int port=zcs->port();
    qDebug() <<"nazev sluzby "<<nazev<<" ip adresa "<<ipadresa<<" port "<<QString::number(port)<<" data" <<verze;

    seznamSluzeb.append(zcs);
    emit signalAktualizaceSeznamu();

    if (jeSluzbaHledanaVerze(nazevSluzbyInterni,verzeInterni,zcs)&&(this->existujeKandidat==false)&&(this->odebirano==false))
    {
        qDebug()<<"odesilam subscribe na "<<ipadresa<<":"<<QString::number(port)<<" sluzba "<<nazev;

        QString adresaZaLomitkem="/"+nazevSluzbyInterni+"/Subscribe"+strukturaInterni;
        QString adresaCileString="http://"+zcs->ip().toString()+":"+QString::number(zcs->port())+adresaZaLomitkem;
        qDebug()<<"adresaCile string "<<adresaCileString;
        QUrl adresaKamPostovatSubscribe=QUrl(adresaCileString);
        existujeKandidat=true;
        kandidatSluzbaMdns=zcs;
        PostSubscribe(adresaKamPostovatSubscribe,this->vytvorSubscribeRequest(adresaZarizeni,cisloPortuInterni));

    }

   // emit nalezenaSluzba( zcs);

}

void IbisIpSubscriberOnePublisher::slotCasovacVyprsel()
{
    qDebug() <<  Q_FUNC_INFO;
    emit signalZtrataOdberu();
    novePrihlaseniOdberu();
}

void IbisIpSubscriberOnePublisher::slotHttpFinished()
{
    qDebug() <<  Q_FUNC_INFO;

    QByteArray bts = reply->readAll();
    QString str(bts);
    qDebug()<<"odpoved na subscribe:"<<str;

    aktualniSluzbaMdns=kandidatSluzbaMdns;

    this->odebirano=true;
    reply->deleteLater();
    //reply = nullptr;
}

void IbisIpSubscriberOnePublisher::slotOdstranenaSluzba(QZeroConfService zcs)
{
    qDebug() <<  Q_FUNC_INFO;
    vymazSluzbuZeSeznamu(seznamSluzeb,zcs);
    if(zcs==aktualniSluzbaMdns)
    {
        emit signalZtrataOdberu();
        novePrihlaseniOdberu();
    }
}

void IbisIpSubscriberOnePublisher::slotSubscribeOdeslan(QNetworkReply *rep)
{
    qDebug() <<  Q_FUNC_INFO;
    QByteArray bts = rep->readAll();
    QString str(bts);
    qDebug()<<"odpoved na subscribe:"<<str;

    aktualniSluzbaMdns=kandidatSluzbaMdns;

    this->odebirano=true;
}


void IbisIpSubscriberOnePublisher::slotVypisObsahRequestu(QString vysledek)
{
    qDebug() <<  Q_FUNC_INFO;
   // QByteArray posledniRequest=InstanceNovehoServeru.bodyPozadavku;
    QDomDocument xmlrequest;
    xmlrequest.setContent(vysledek);
    timer->start(defaultniCasovac);
    emit dataNahrana(vysledek);
}



