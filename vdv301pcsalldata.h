#ifndef VDV301PCSALLDATA_H
#define VDV301PCSALLDATA_H

#include <QDateTime>

class Vdv301PcsAllData
{
public:
    Vdv301PcsAllData();

    QDateTime timeStamp;// timeStamp, type="IBIS-IP.dateTime"
};

#endif // VDV301PCSALLDATA_H
