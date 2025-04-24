#include "pcspublisherstruct.h"

PcsPublisherStruct::PcsPublisherStruct() {}

PcsPublisherStruct::PcsPublisherStruct(const PublisherStruct& pubStruct, int doorNum) : PublisherStruct(pubStruct), doorNumber(doorNum) {}
