
///  \file CZColorBalance.h
///  \brief This is the file declare the class CZColorBalance.
///
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-16
///  \note


#ifndef _CZCOLORBALANCE_H_
#define _CZCOLORBALANCE_H_

#include "serialization/CZCoding.h"

class CZColorBalance : public CZCoding
{
public:
	CZColorBalance(float r = 0.0f, float g = 0.0f, float b = 0.0f);
	~CZColorBalance();

	bool operator == (const CZColorBalance & cb) const;

	/// 实现coding 接口
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);

	float redShift;
	float greenShift;
	float blueShift;
};

#endif