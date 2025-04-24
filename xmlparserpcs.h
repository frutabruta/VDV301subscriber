#ifndef XMLPARSERPCS_H
#define XMLPARSERPCS_H

#include <QtXml>

class XmlParserPcs
{
public:
    XmlParserPcs();
    bool parseBasicNumbers(QString input, int &normalIn, int &normalOut);
};

#endif // XMLPARSERPCS_H
