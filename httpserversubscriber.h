#ifndef HTTPSERVERSUBSCRIBER_H
#define HTTPSERVERSUBSCRIBER_H

#include <QObject>
#include <QWidget>

#include <QtHttpServer>

class HttpServerSubscriber: public QObject
{
    Q_OBJECT
public:
    HttpServerSubscriber(quint16 portNumber);

    QHttpServer httpServer;

    quint16 mPortNumber=0;

    QString contentGet="obsahGet";
    QString contentSubscribe="<SubscribeResponse xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><Active><Value>true</Value></Active></SubscribeResponse>";

    QByteArray requestBody="xx";
    QString contentRoot="";

    void setContentGet(QString input);
    void setContentSubscribe(QString input);
    int start();

    int listen();
    int route(QString &getRequestContent, QMap<QString, QString> &contentBodyList);


    int setContentBody(QMap<QString, QString> input);
    quint16 portNumber() const;
    void setPortNumber(quint16 newPortNumber);

private:
    QString createOkResponse();
    QMap<QString,QString> contentBodyMap;
signals:
    void signalContentChanged(QByteArray vysledek,QString struktura) ;
    void signalDataReceived(QString vysledek) ;
    //void zmenaObsahu() ;
};

#endif // HTTPSERVERSUBSCRIBER_H
