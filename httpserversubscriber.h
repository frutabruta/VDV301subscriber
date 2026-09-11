#ifndef HTTPSERVERSUBSCRIBER_H
#define HTTPSERVERSUBSCRIBER_H

#include <QObject>
#include <QWidget>
#include <QtHttpServer>
#include <QLoggingCategory>

class HttpServerRequest
{
public:
    QByteArray body="";
    QHostAddress hostAddress;
    quint16 port=0;
};

class HttpServerSubscriber: public QObject
{
    Q_OBJECT
public:
    HttpServerSubscriber(quint16 portNumber, QString replyPath="");

    QHttpServer httpServer;



    quint16 mPortNumber=0;

    QString contentGet="obsahGet";
    QString subscribeResponseContent="<SubscribeResponse xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><Active><Value>true</Value></Active></SubscribeResponse>";
    QString mReplyPath="";

    QByteArray requestBody="xx";
    QString contentRoot="";

    void setContentGet(QString input);
    void setContentSubscribe(QString input);
    int start();

    int listen();
    int route(QString &getRequestContent, QMap<QString, QString> &contentBodyList, QString replyPath="");


    int setContentBody(QMap<QString, QString> input);
    quint16 portNumber() const;
    void setPortNumber(quint16 newPortNumber);

    QString replyPath() const;
    void setReplyPath(const QString &newReplyPath);

private:
    QString createOkResponse();
    QMap<QString,QString> contentBodyMap;

#if QT_VERSION > QT_VERSION_CHECK(6, 8, 0)
    QPointer<QTcpServer> mTcpServer;
    void addServerHeaders(const QHttpServerRequest &request, QHttpServerResponse &resp);
#endif

signals:
    void signalContentChanged(QByteArray vysledek,QString struktura) ;
    void signalDataReceived(QString vysledek) ;
    void signalWholeRequest(const HttpServerRequest request);
    //void zmenaObsahu() ;
};

#endif // HTTPSERVERSUBSCRIBER_H
