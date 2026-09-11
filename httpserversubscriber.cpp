#include "httpserversubscriber.h"

Q_LOGGING_CATEGORY(HttpServerSubscriberLog, "HttpServerSubscriber")

HttpServerSubscriber::HttpServerSubscriber(quint16 portNumber, QString replyPath)
{
    mPortNumber=portNumber;
    mReplyPath=replyPath;
    contentRoot=createOkResponse();
    //start();

}

QString HttpServerSubscriber::replyPath() const
{
    return mReplyPath;
}

void HttpServerSubscriber::setReplyPath(const QString &newReplyPath)
{
    mReplyPath = newReplyPath;
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
    qCDebug(HttpServerSubscriberLog) <<Q_FUNC_INFO;


    this->route(contentGet,contentBodyMap,mReplyPath);
    this->listen();

    return 1;
}

#if QT_VERSION > QT_VERSION_CHECK(6, 8, 0)

void HttpServerSubscriber::addServerHeaders(const QHttpServerRequest &request, QHttpServerResponse &resp)
{
    Q_UNUSED(request);

    QHttpHeaders headers = resp.headers();
    headers.append(QHttpHeaders::WellKnownHeader::Server, "Super server!");
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "text/xml");
    resp.setHeaders(std::move(headers));
}

#endif


int HttpServerSubscriber::route(QString &getRequestContent,  QMap<QString,QString> &contentBodyList, QString replyPath)
{

    qCDebug(HttpServerSubscriberLog) <<Q_FUNC_INFO;

    qCDebug(HttpServerSubscriberLog)<<"get request content="<<getRequestContent;
    httpServer.route("/CustomerInformationService/Get<arg>", [&contentBodyList](const QUrl &url,const QHttpServerRequest &request)
                     {
                         QString struktura= QStringLiteral("%1").arg(url.path());
                         qCDebug(HttpServerSubscriberLog)<<"argument "<<struktura;

                         qCDebug(HttpServerSubscriberLog)<<"request "<<"/CustomerInformationService/Get<arg>";

                         return contentBodyList.value(struktura);

                     });

    httpServer.route(replyPath, [replyPath,this](const QHttpServerRequest &request)
                     {
                         qCDebug(HttpServerSubscriberLog)<<"replyPath: "<<replyPath;
                         HttpServerRequest requestReturnValue;
                         requestReturnValue.body=request.body();
                         requestReturnValue.hostAddress=request.remoteAddress();

#if QT_VERSION > QT_VERSION_CHECK(6, 5, 0)
                         requestReturnValue.port=request.remotePort();
#endif
                         qCInfo(HttpServerSubscriberLog)<<" received data from "<<replyPath<<" size:"<<requestReturnValue.body.size()<<" from:"<<requestReturnValue.hostAddress<<":"<<requestReturnValue.port;
                         emit signalDataReceived(requestReturnValue.body);
                         emit signalWholeRequest(requestReturnValue);

                         QString okResponse="HTTP/1.1 200 OK";
                         //return this->obsahRoot;
                         return "";
                         //return intObsahGet;
                     });
    httpServer.route("/"+replyPath, [replyPath,this](const QHttpServerRequest &request)
                     {
                         qCDebug(HttpServerSubscriberLog)<<"replyPath : /"<<replyPath;
                         HttpServerRequest requestReturnValue;
                         requestReturnValue.body=request.body();
                         requestReturnValue.hostAddress=request.remoteAddress();
                         qCInfo(HttpServerSubscriberLog)<<" received data from "<<replyPath<<" size:"<<requestReturnValue.body.size()<<" from:"<<requestReturnValue.hostAddress<<":"<<requestReturnValue.port;

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
    if(replyPath!="")
    {
        httpServer.route("/", [this](const QHttpServerRequest &request)
                         {
                             qCDebug(HttpServerSubscriberLog)<<"OBU doesn't respect ReplyPath";

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

                         });
    }




#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)

//not used on Qt5.15
#if QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
    httpServer.afterRequest([](QHttpServerResponse &&resp)
                            {
                                resp.setHeader("Server", "Super server!");
                                resp.setHeader("Content-Type", "text/xml");
                                return std::move(resp);
                            });
#else
    httpServer.addAfterRequestHandler(this, &HttpServerSubscriber::addServerHeaders);
#endif
#endif

    return 1;
}

int HttpServerSubscriber::listen()
{
    qCDebug(HttpServerSubscriberLog) << Q_FUNC_INFO;
#if QT_VERSION > QT_VERSION_CHECK(6, 8, 0)
    std::unique_ptr<QTcpServer> tcpServer = std::make_unique<QTcpServer>();

    bool isListening;
    if (mPortNumber != 0)
    {
        /* manual port choice */
        isListening = tcpServer->listen(QHostAddress::Any, mPortNumber);
    }
    else
    {
        /* automatic port selection */
        isListening = tcpServer->listen(QHostAddress::Any);
    }

    if (!isListening)
    {
        qCDebug(HttpServerSubscriberLog) << QCoreApplication::translate(
            "QHttpServerExample", "Server failed to listen on a port.");
        return 0;
    }


    if (!httpServer.bind(tcpServer.get()))
    {
        qCDebug(HttpServerSubscriberLog) << QCoreApplication::translate(
            "QHttpServerExample", "Server failed to bind the HTTP server to the TCP server.");
        return 0;
    }


    // httpServer now owns/parents the QTcpServer, so release our unique_ptr
    // and keep a non-owning, dangle-safe reference via QPointer.
    quint16 port = tcpServer->serverPort();
    mTcpServer = tcpServer.release();

    qCDebug(HttpServerSubscriberLog) << "Starting server at port:" << QString::number(port);
    qCDebug(HttpServerSubscriberLog) << QCoreApplication::translate(
                    "QHttpServerExample",
                    "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)").arg(port);
    return port;
#else
    if (mPortNumber!=0)
    {
        /* manual port choice */
        const auto port = httpServer.listen(QHostAddress::Any,mPortNumber);
        if (!port)
        {
            qCDebug(HttpServerSubscriberLog) << QCoreApplication::translate(
                "QHttpServerExample", "Server failed to listen on a port.");

        }

        qCDebug(HttpServerSubscriberLog)<<"Starting server at port:"<<QString::number(port);
        return port;

        qCDebug(HttpServerSubscriberLog)<< QCoreApplication::translate("QHttpServerExample", "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)").arg(port);

    }
    else
    {
        /* automatic port selection */
        const auto port = httpServer.listen(QHostAddress::Any);
        if (!port) {
            qCDebug(HttpServerSubscriberLog) << QCoreApplication::translate(
                "QHttpServerExample", "Server failed to listen on a port.");

        }

        return port;

        qCDebug(HttpServerSubscriberLog) << QCoreApplication::translate("QHttpServerExample", "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)").arg(port);

    }


    return 1;
#endif
}


void HttpServerSubscriber::setContentGet(QString input)
{
    qCDebug(HttpServerSubscriberLog) <<Q_FUNC_INFO;
    this->contentGet=input;

}

void HttpServerSubscriber::setContentSubscribe(QString input)
{
    qCDebug(HttpServerSubscriberLog) <<Q_FUNC_INFO;
    this->subscribeResponseContent=input;
}

int HttpServerSubscriber::setContentBody(QMap<QString,QString> input )
{
    qCDebug(HttpServerSubscriberLog) <<Q_FUNC_INFO;
    contentBodyMap=input;


    //qCDebug(HttpServerSubscribeLog)<<"obsah CDC z pole: "<<obsahTelaPole.value("CurrentDisplayContent");

    return 1;
}


QString HttpServerSubscriber::createOkResponse()
{
    qCDebug(HttpServerSubscriberLog) <<Q_FUNC_INFO;
    QString header;
    header+=("HTTP/1.1 200 OK\r\n");       // \r needs to be before \n
    header+=("Content-Type: application/xml\r\n");
    header+=("Connection: close\r\n");
    header+=("Pragma: no-cache\r\n");
    header+=("\r\n");
    return header;
}
