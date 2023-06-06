#include "httpserversubscriber.h"

HttpServerSubscriber::HttpServerSubscriber(quint16 ppp)
{
    cisloPortu=ppp;
    obsahRoot=vyrobHlavickuOk();
    //start();

}

int HttpServerSubscriber::start()
{
    qDebug() <<Q_FUNC_INFO;

    this->route(obsahGet,obsahTelaPole);
    this->listen();

    return 1;
}


int HttpServerSubscriber::route(QString &intObsahGet,  QMap<QString,QString> &obsahyBody)
{

    qDebug() <<Q_FUNC_INFO;
    httpServer.route("/CustomerInformationService/SubscribeAllData", [this ](const QHttpServerRequest &request)
    {
        qDebug()<<"request "<<"/CustomerInformationService/SubscribeAllData";
        qDebug()<<"tady se mel spustit emit";

        QString textVysledek="true";
        QString odpoved="";
        odpoved+="<?xml version=\"1.0\" encoding=\"utf-16\"?>";
        odpoved+="<SubscribeResponse xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
        odpoved+="<Active><Value>";
        odpoved+=textVysledek;
        odpoved+="</Value></Active>";
        odpoved+="</SubscribeResponse>";
        this->bodyPozadavku=request.body();
        QString struktura="AllData";
        emit zmenaObsahu(request.body(),struktura);
        //odpoved=this->obsahSubscribe;

        return this->obsahSubscribe;
        //return odpoved;
        //return "Hello world";
    });
    httpServer.route("/CustomerInformationService/SubscribeCurrentDisplayContent", [this ](const QHttpServerRequest &request)
    {
        //qDebug()<<request.headers()["Connection"].isNull();
        qDebug()<<"request "<<"/CustomerInformationService/SubscribeCurrentDisplayContent";
        //qDebug()<<request.body();

        qDebug()<<"tady se mel spustit emit";

        QString textVysledek="true";
        QString odpoved="";
        odpoved+="<?xml version=\"1.0\" encoding=\"utf-16\"?>";
        odpoved+="<SubscribeResponse xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
        odpoved+="<Active><Value>";
        odpoved+=textVysledek;
        odpoved+="</Value></Active>";
        odpoved+="</SubscribeResponse>";
        this->bodyPozadavku=request.body();
        QString struktura="CurrentDisplayContent";
        emit zmenaObsahu(request.body(),struktura);
        //odpoved=this->obsahSubscribe;

        return this->obsahSubscribe;
        //return odpoved;
        //return "Hello world";
    });

    qDebug()<<"vnejsi intObsahGet="<<intObsahGet;
    httpServer.route("/CustomerInformationService/Get<arg>", [&obsahyBody](const QUrl &url,const QHttpServerRequest &request)
    {
        QString struktura= QStringLiteral("%1").arg(url.path());
        qDebug()<<"argument "<<struktura;

        qDebug()<<"request "<<"/CustomerInformationService/Get<arg>";
        //qDebug()<<request.body();
        //return obsahyBody.value("AllData");
        return obsahyBody.value(struktura);
        //return intObsahGet;
    });

    httpServer.route("/", [this](const QHttpServerRequest &request)
    {
        emit prijemDat(request.body());
        QString okResponse="HTTP/1.1 200 OK";
        //return this->obsahRoot;
        return "";
        //return intObsahGet;
    });

    httpServer.afterRequest([](QHttpServerResponse &&resp)
    {
        resp.setHeader("Server", "Super server!");
        resp.setHeader("Content-Type", "text/xml");
        return std::move(resp);
    });
    return 1;
}

int HttpServerSubscriber::listen()
{
    qDebug() <<Q_FUNC_INFO;

    quint16 nactenyPort;
    if(cisloPortu==0)
    {
        nactenyPort = httpServer.listen(QHostAddress::Any);
    }
    else
    {
        nactenyPort=httpServer.listen(QHostAddress::Any,cisloPortu);
    }


    // manualni port:  const auto port = httpServer.listen(QHostAddress::Any,cisloPortu);
    if (!nactenyPort)
    {
        qDebug() << QCoreApplication::translate(
                        "QHttpServerExample", "Server failed to listen on a port.");
        return 0;
    }
    // nactenyPort=httpServer.listen(QHostAddress::Any,cisloPortu);

    /* automaticky port
     const auto port = httpServer.listen(QHostAddress::Any);
    if (!port) {
        qDebug() << QCoreApplication::translate(
                "QHttpServerExample", "Server failed to listen on a port.");
        return 0;
    }
    */
    qDebug() << QCoreApplication::translate("QHttpServerExample", "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)").arg(nactenyPort);
    return 1;
}


void HttpServerSubscriber::zapisDoPromenneGet(QString vstup)
{
    qDebug() <<Q_FUNC_INFO;
    this->obsahGet=vstup;

}

void HttpServerSubscriber::zapisDoSubscribe(QString vstup)
{
    qDebug() <<Q_FUNC_INFO;
    this->obsahSubscribe=vstup;
}

int HttpServerSubscriber::nastavObsahTela(QMap<QString,QString> vstup )
{
    qDebug() <<Q_FUNC_INFO;
    obsahTelaPole=vstup;


    //qDebug()<<"obsah CDC z pole: "<<obsahTelaPole.value("CurrentDisplayContent");

    return 1;
}


QString HttpServerSubscriber::vyrobHlavickuOk()
{
    qDebug() <<Q_FUNC_INFO;
    QString hlavicka;
    hlavicka+=("HTTP/1.1 200 OK\r\n");       // \r needs to be before \n
    hlavicka+=("Content-Type: application/xml\r\n");
    hlavicka+=("Connection: close\r\n");
    hlavicka+=("Pragma: no-cache\r\n");
    hlavicka+=("\r\n");
    return hlavicka;
}
