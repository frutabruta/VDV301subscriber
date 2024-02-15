#ifndef XMLGENERATORSUBSCRIBER_H
#define XMLGENERATORSUBSCRIBER_H

#include <QObject>
#include <QDomDocument>

class XmlGeneratorSubscriber
{
public:
    XmlGeneratorSubscriber();
    QDomProcessingInstruction createProcessingInformation(QDomDocument &xmlDocument, QString encoding);

    QString encoding="utf-8";
    QDomDocument setDeviceConfigurationRequest(QDomDocument xmlDocument, QString id);
    QDomElement value(QDomDocument &xmlDocument, QString elementName, QString content);
private:

};

#endif // XMLGENERATORSUBSCRIBER_H
