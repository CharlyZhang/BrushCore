//
//  CZTypedData.h
//  BrushesCore
//
//  Created by CharlyZhang on 15/11/24.
//  Copyright © 2015年 CharlyZhang. All rights reserved.
//

#ifndef _CZTYPEDDATA_H_
#define _CZTYPEDDATA_H_

#include "CZDataProvider.h"
#include "CZImage.h"
#include <string>

class CZTypedData : CZDataProvider
{
public:
    CZTypedData();
    ~CZTypedData();
    static CZTypedData* produceTypedData(CZImage *img, std::string type, bool compress_, CZSaveStatus status_);
    
    /// CZDataProvider methods
    void* getData();
    CZSaveStatus getSaveStatus();
    std::string  getMediaType();
    
private:
    bool compress;
    CZSaveStatus status;
    std::string mediaType;
    CZImage *data;
};

#endif
