#ifndef PCSPUBLISHERSTRUCT_H
#define PCSPUBLISHERSTRUCT_H

#include "VDV301subscriber/publisherstruct.h"

class PcsPublisherStruct : public PublisherStruct
{
public:
    PcsPublisherStruct();
    PcsPublisherStruct(const PublisherStruct& pubStruct, int doorNum);
    int doorNumber=1;
};


#endif // PCSPUBLISHERSTRUCT_H
