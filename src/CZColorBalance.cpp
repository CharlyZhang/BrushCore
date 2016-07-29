
///  \file CZColorBalance.cpp
///  \brief This is the file implements the class CZColorBalance.
///
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-16
///  \note

#include "CZColorBalance.h"

CZColorBalance::CZColorBalance(float r /* = 0.0f */, float g /* = 0.0f */, float b /* = 0.0f */)
{
	r /= 2.0;
	g /= 2.0;
	b /= 2.0;

	float average = (r + g + b) / 3.0f;
	r -= average;
	g -= average;
	b -= average;

	redShift = r;
	greenShift = g;
	blueShift = b;
}
CZColorBalance::~CZColorBalance(){}

bool CZColorBalance::operator==(const CZColorBalance & cb) const
{
	return (redShift == cb.redShift && blueShift == cb.blueShift && greenShift == cb.greenShift);
}

/// 实现coding 接口
void CZColorBalance::update(CZDecoder *decoder_, bool deep /*= false*/)
{
	redShift = decoder_->decodeFloat("red");
	greenShift = decoder_->decodeFloat("green");
	blueShift = decoder_->decodeFloat("blue");
};
void CZColorBalance::encode(CZCoder *coder_, bool deep /*= false*/)
{
	coder_->encodeFloat(redShift,"red");
	coder_->encodeFloat(greenShift,"green");
	coder_->encodeFloat(blueShift,"blue");
};