
///  \file CZDecoder.h
///  \brief This is the file declare the class CZDecoder.
///
///		CZDecoder is an interface. It defines the imperative methods of decoder.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-25
///  \note

#ifndef _CZDECODER_H_
#define _CZDECODER_H_

#include <string>

class CZCoding;

class CZDecoder
{
public:
    virtual CZCoding* decodeObject(const std::string key) = 0;
    virtual unsigned int decodeUint(const std::string key) = 0;
    virtual float decodeFloat(const std::string key, float deft = 0.0f) = 0;
};
#endif