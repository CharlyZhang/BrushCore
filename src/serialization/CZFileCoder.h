//
//  CZFileCoder.h
//  BrushesCore
//
//  Created by CharlyZhang on 15/11/24.
//  Copyright © 2015年 CharlyZhang. All rights reserved.
//

#ifndef _CZFILECODER_H_
#define _CZFILECODER_H_

#include "CZCoder.h"

class CZFileCoder : public CZCoder
{
public:
    void encodeObject(CZCoding* obj, const std::string key, bool deep);
    void encodeArray(std::vector<CZCoding*> &array, const std::string key);
    void encodeDataProvider(CZDataProvider *dataProvider, const std::string key);
    void encodeFloat(float value, const std::string key);
    void encodeString(char *str, const std::string key);
    void encodeUint(unsigned int value, const std::string key);
    void encodeBoolean(bool value, const std::string key);
    void encodeImage(CZImage* image, const std::string key);
    
private:
    
};

#endif
