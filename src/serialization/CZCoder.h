
///  \file CZCoder.h
///  \brief This is the file declare the class CZCoder.
///
///		CZCoder is an interface. It defines the imperative methods of coder.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-25
///  \note

#ifndef _CZCODER_H_
#define _CZCODER_H_

#include <string>
#include <vector>
#include "CZDataProvider.h"
#include "../basic/CZImage.h"

class CZCoding;

class CZCoder
{
public:
	virtual void encodeObject(CZCoding* obj, const std::string key, bool deep) = 0;
    virtual void encodeArray(std::vector<CZCoding*> &array, const std::string key) = 0;
    virtual void encodeDataProvider(CZDataProvider *dataProvider, const std::string key) =0;
	virtual void encodeFloat(float value, const std::string key) = 0;
	virtual void encodeString(char *str, const std::string key) = 0;
	virtual void encodeUint(unsigned int value, const std::string key) = 0;
    virtual void encodeBoolean(bool value, const std::string key) = 0;
    virtual void encodeImage(CZImage* image, const std::string key) = 0;
};
#endif