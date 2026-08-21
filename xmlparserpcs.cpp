#include "xmlparserpcs.h"

XmlParserPcs::XmlParserPcs() {}


bool XmlParserPcs::parseBasicNumbers(QString input, int &normalIn, int &normalOut)
{
    bool result=true;
    QDomDocument domDocument;
    domDocument.setContent(input);

    QDomNodeList nodeList=domDocument.elementsByTagName("Count");

    for(int i=0;i<nodeList.length();i++)
    {
        QDomNode selectedNode=nodeList.at(i);
        if(selectedNode.toElement().firstChildElement("ObjectClass").text()=="Adult")
        {
            normalIn=selectedNode.toElement().firstChildElement("In").firstChildElement("Value").text().toInt();
            normalOut=selectedNode.toElement().firstChildElement("Out").firstChildElement("Value").text().toInt();
        }

    }
    return result;
}
