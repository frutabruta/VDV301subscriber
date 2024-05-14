#include "httpserversubscriber.h"

HttpServerSubscriber::HttpServerSubscriber(quint16 portNumber)
{
    mPortNumber=portNumber;
    contentRoot=createOkResponse();
    //start();

}

quint16 HttpServerSubscriber::portNumber() const
{
    return mPortNumber;
}

void HttpServerSubscriber::setPortNumber(quint16 newPortNumber)
{
    mPortNumber = newPortNumber;
}

int HttpServerSubscriber::start()
{
    qDebug() <<Q_FUNC_INFO;

    this->route(contentGet,contentBodyMap);
    this->listen();

    return 1;
}


int HttpServerSubscriber::route(QString &getRequestContent,  QMap<QString,QString> &contentBodyList)
{

    qDebug() <<Q_FUNC_INFO;

    qDebug()<<"get request content="<<getRequestContent;
    httpServer.route("/CustomerInformationService/Get<arg>", [&contentBodyList](const QUrl &url,const QHttpServerRequest &request)
                     {
                         QString struktura= QStringLiteral("%1").arg(url.path());
                         qDebug()<<"argument "<<struktura;

                         qDebug()<<"request "<<"/CustomerInformationService/Get<arg>";

                         return contentBodyList.value(struktura);

                     });

    httpServer.route("/", [this](const QHttpServerRequest &request)
                     {

                         HttpServerRequest requestReturnValue;
                         requestReturnValue.body=request.body();
                         requestReturnValue.hostAddress=request.remoteAddress();

#if QT_VERSION > QT_VERSION_CHECK(6, 5, 0)
                         requestReturnValue.port=request.remotePort();
#endif

                         emit signalDataReceived(requestReturnValue.body);
                         emit signalWholeRequest(requestReturnValue);

                         QString okResponse="HTTP/1.1 200 OK";
                         //return this->obsahRoot;
                         return "";
                         //return intObsahGet;
                     });
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    httpServer.afterRequest([](QHttpServerResponse &&resp)
                            {
                                resp.setHeader("Server", "Super server!");
                                resp.setHeader("Content-Type", "text/xml");
                                return std::move(resp);
                            });
#endif
    return 1;
}

int HttpServerSubscriber::listen()
{
    qDebug() <<Q_FUNC_INFO;

    quint16 newPort;
    if(mPortNumber==0)
    {
        newPort = httpServer.listen(QHostAddress::Any);
    }
    else
    {
        newPort=httpServer.listen(QHostAddress::Any,mPortNumber);
    }


    // manualni port:  const auto port = httpServer.listen(QHostAddress::Any,cisloPortu);
    if (!newPort)
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
    qDebug() << QCoreApplication::translate("QHttpServerExample", "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)").arg(newPort);
    return 1;
}


void HttpServerSubscriber::setContentGet(QString input)
{
    qDebug() <<Q_FUNC_INFO;
    this->contentGet=input;

}

void HttpServerSubscriber::setContentSubscribe(QString input)
{
    qDebug() <<Q_FUNC_INFO;
    this->subscribeResponseContent=input;
}

int HttpServerSubscriber::setContentBody(QMap<QString,QString> input )
{
    qDebug() <<Q_FUNC_INFO;
    contentBodyMap=input;


    //qDebug()<<"obsah CDC z pole: "<<obsahTelaPole.value("CurrentDisplayContent");

    return 1;
}


QString HttpServerSubscriber::createOkResponse()
{
    qDebug() <<Q_FUNC_INFO;
    QString header;
    header+=("HTTP/1.1 200 OK\r\n");       // \r needs to be before \n
    header+=("Content-Type: application/xml\r\n");
    header+=("Connection: close\r\n");
    header+=("Pragma: no-cache\r\n");
    header+=("\r\n");
    return header;
}
