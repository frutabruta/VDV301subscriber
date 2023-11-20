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
    httpServer.route("/CustomerInformationService/SubscribeAllData", [this ](const QHttpServerRequest &request)
    {
        //needs to be modified to generate XML response properly
        //error responses are not supported
        qDebug()<<"request "<<"/CustomerInformationService/SubscribeAllData";

        QString responseResult="true";
        QString response="";
        response+="<?xml version=\"1.0\" encoding=\"utf-16\"?>";
        response+="<SubscribeResponse xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
        response+="<Active><Value>";
        response+=responseResult;
        response+="</Value></Active>";
        response+="</SubscribeResponse>";
        this->requestBody=request.body();
        QString structureName="AllData";
        emit signalContentChanged(request.body(),structureName);

        return this->contentSubscribe;

    });
    httpServer.route("/CustomerInformationService/SubscribeCurrentDisplayContent", [this ](const QHttpServerRequest &request)
    {

        qDebug()<<"request "<<"/CustomerInformationService/SubscribeCurrentDisplayContent";


        QString responseResult="true";
        QString result="";
        result+="<?xml version=\"1.0\" encoding=\"utf-16\"?>";
        result+="<SubscribeResponse xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
        result+="<Active><Value>";
        result+=responseResult;
        result+="</Value></Active>";
        result+="</SubscribeResponse>";
        this->requestBody=request.body();
        QString structureName="CurrentDisplayContent";
        emit signalContentChanged(request.body(),structureName);

        return this->contentSubscribe;

    });

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
        emit signalDataReceived(request.body());
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
    this->contentSubscribe=input;
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
