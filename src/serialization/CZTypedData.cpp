//
//  CZTypedData.cpp
//  BrushesCore
//
//  Created by CharlyZhang on 15/11/24.
//  Copyright © 2015年 CharlyZhang. All rights reserved.
//

#include "CZTypedData.h"

using namespace std;

CZTypedData::CZTypedData()
{
    data = nullptr;
    status = kSaveStatusUnsaved;
}

CZTypedData::~CZTypedData()
{
    if (data)
    {
        delete data;
        data = nullptr;
    }
}

CZTypedData * CZTypedData::produceTypedData(CZImage *img, string type, bool compress_, CZSaveStatus status_)
{
    CZTypedData *ret = new CZTypedData;
    /// TO DO:
    return ret;
}

void * CZTypedData::getData()
{
    return (void*)data;
}

CZSaveStatus CZTypedData::getSaveStatus()
{
    return status;
}


string CZTypedData::getMediaType()
{
    return mediaType;
}