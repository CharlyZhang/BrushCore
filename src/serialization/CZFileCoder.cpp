//
//  CZFileCoder.cpp
//  BrushesCore
//
//  Created by CharlyZhang on 15/11/24.
//  Copyright © 2015年 CharlyZhang. All rights reserved.
//

#include "CZFileCoder.h"


void CZFileCoder::encodeObject(CZCoding* obj, const std::string key, bool deep)
{
    
}

void CZFileCoder::encodeArray(std::vector<CZCoding*> &array, const std::string key){};
void CZFileCoder::encodeDataProvider(CZDataProvider *dataProvider, const std::string key){};
void CZFileCoder::encodeFloat(float value, const std::string key){};
void CZFileCoder::encodeString(char *str, const std::string key){};
void CZFileCoder::encodeUint(unsigned int value, const std::string key){};
void CZFileCoder::encodeBoolean(bool value, const std::string key)
{};
void CZFileCoder::encodeImage(CZImage* image, const std::string key){};