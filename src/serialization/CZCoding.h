
///  \file CZCoding.h
///  \brief This is the file declare the class CZCoding.
///
///		CZCoder is an interface. It defines the imperative methods of the class which need coding.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-25
///  \note		checked at 2015-03-25

#ifndef _CZCODING_H_
#define _CZCODING_H_

#include "CZCoder.h"
#include "CZDecoder.h"

class CZCoding
{
public:
	virtual void update(CZDecoder *decoder_, bool deep = false) = 0;
	virtual void encode(CZCoder *coder_, bool deep = false) = 0;
};
#endif