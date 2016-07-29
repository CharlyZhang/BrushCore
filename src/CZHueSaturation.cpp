
///  \file CZHueSaturation.cpp
///  \brief This is the file implements the class CZHueSaturation.
///
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-22
///  \note

#include "CZHueSaturation.h"

CZHueSaturation::CZHueSaturation(float h /* = 0.0f */, float s /* = 0.0f */, float b /* = 0.0f */)
{
	hueShift = h;
	saturationShift = s;
	brightnessShift = b;
}
CZHueSaturation::~CZHueSaturation(){}

bool CZHueSaturation::operator==(const CZHueSaturation & hs) const
{
	return (hueShift == hs.hueShift && saturationShift == hs.saturationShift && brightnessShift == hs.brightnessShift);
}

/// 实现coding 接口
void CZHueSaturation::update(CZDecoder *decoder_, bool deep /*= false*/)
{
	hueShift = decoder_->decodeFloat("hue");
	saturationShift = decoder_->decodeFloat("saturation");
	brightnessShift = decoder_->decodeFloat("brightness");
};
void CZHueSaturation::encode(CZCoder *coder_, bool deep /*= false*/)
{
	coder_->encodeFloat(hueShift,"hue");
	coder_->encodeFloat(saturationShift,"saturation");
	coder_->encodeFloat(brightnessShift,"brightness");
};