
///  \file CZHueSaturation.h
///  \brief This is the file declare the class CZHueSaturation.
///
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-22
///  \note

#ifndef _CZHUESATURATION_H_
#define _CZHUESATURATION_H_

#include "serialization/CZCoding.h"

class CZHueSaturation : public CZCoding
{
public:
	CZHueSaturation(float h = 0.0f, float s = 0.0f, float b = 0.0f);
	~CZHueSaturation();

	bool operator == (const CZHueSaturation & hs) const;

	/// 实现coding 接口
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);

	float hueShift;
	float saturationShift;
	float brightnessShift;
};

#endif