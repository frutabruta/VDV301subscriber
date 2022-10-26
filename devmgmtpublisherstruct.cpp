#include "devmgmtpublisherstruct.h"

DevMgmtPublisherStruct::DevMgmtPublisherStruct()
{
    port=0;;
    deviceId="";
    deviceClass="";
    deviceName="";
    hostname="";
    serviceName="";
    ibisIpVersion="";
    swVersion="";
    manufacturer="";
    serialNumber="";
}


bool DevMgmtPublisherStruct::operator==(const DevMgmtPublisherStruct &node)
{
return port == node.port &&
adresa == node.adresa;
}

