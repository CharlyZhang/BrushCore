
///  \file CZColor.cpp
///  \brief This is the file implement the class CZColor.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-25
///  \note

#include "CZColor.h"
#include "../CZUtil.h"
#include <cmath>

/// 采用RGB模式初始化
void CZColor::initWithRGB(float r, float g, float b, float a)
{
	red			= CZUtil::Clamp(0.0f, 1.0f, r);
	green		= CZUtil::Clamp(0.0f, 1.0f, g);
	blue		= CZUtil::Clamp(0.0f, 1.0f, b);
	alpha		= CZUtil::Clamp(0.0f, 1.0f, a);

	float h, s, v;
	CZUtil::RGBtoHSV(red, green, blue, h, s, v);

	hue			= CZUtil::Clamp(0.0f, 1.0f, h);
	saturation	= CZUtil::Clamp(0.0f, 1.0f, s);
	brightness	= CZUtil::Clamp(0.0f, 1.0f, v);
	
}

/// 采用HSV模式初始化
/// 
///		\param h - 色调
///		\param s - 饱和度
///		\param v - 亮度
///		\param a - 不透明度
void CZColor::initWithHSV(float h, float s, float v, float a)
{
	hue			= CZUtil::Clamp(0.0f, 1.0f, h);
	saturation	= CZUtil::Clamp(0.0f, 1.0f, s);
	brightness	= CZUtil::Clamp(0.0f, 1.0f, v);
	alpha		= CZUtil::Clamp(0.0f, 1.0f, a);

	float r,g,b;
	CZUtil::HSVtoRGB(hue,saturation,brightness,r,g,b);

	red			= CZUtil::Clamp(0.0f, 1.0f, r);
	green		= CZUtil::Clamp(0.0f, 1.0f, g);
	blue		= CZUtil::Clamp(0.0f, 1.0f, b);
}

/// 返回随机颜色 - 静态函数
CZColor CZColor::randomColor()
{
	float components[4];

	for (int i = 0; i < 4; i++) 
		components[i] = CZUtil::RandomFloat();

	components[3] = float(0.5f + (components[3] * 0.5f));

	CZColor color;
	color.initWithHSV(components[0], components[1], components[2], components[3]);

	return color;
}

/// 混合其他颜色
CZColor CZColor::colorBlend(const CZColor &c_, float fraction_)
{
	float r = (fraction_ * c_.red) + (1.0f - fraction_) * red;
	float g = (fraction_ * c_.green) + (1.0f - fraction_) * green;
	float b = (fraction_ * c_.blue) + (1.0f - fraction_) * blue;
	float a = (fraction_ * c_.alpha) + (1.0f - fraction_) * alpha;

	return CZColor(r,g,b,a);
}
/// 用RGB模式调整
void CZColor::balanceRGB(float rShift, float gShift, float bShift)
{
	red =	CZUtil::Clamp(0, 1, red + rShift);
	green = CZUtil::Clamp(0, 1, green + gShift);
	blue =	CZUtil::Clamp(0, 1, blue + bShift);

	float h, s, v;
	CZUtil::RGBtoHSV(red, green, blue, h, s, v);

	hue			= CZUtil::Clamp(0.0f, 1.0f, h);
	saturation	= CZUtil::Clamp(0.0f, 1.0f, s);
	brightness	= CZUtil::Clamp(0.0f, 1.0f, v);
}
/// 用HSV模式调整
void CZColor::adjustHSV(float hShift, float sShift, float bShift)
{
	hue = hue + hShift;
	bool negative = (hue < 0);
	hue = fmodf(fabs(hue), 1.0f);
	if (negative) 
	{
		hue = 1.0f - hue;
	}

	sShift = 1 + sShift;
	bShift = 1 + bShift;
	saturation = CZUtil::Clamp(0, 1, saturation * sShift);
	brightness = CZUtil::Clamp(0, 1, brightness * bShift);

	float r,g,b;
	CZUtil::HSVtoRGB(hue,saturation,brightness,r,g,b);

	red			= CZUtil::Clamp(0.0f, 1.0f, r);
	green		= CZUtil::Clamp(0.0f, 1.0f, g);
	blue		= CZUtil::Clamp(0.0f, 1.0f, b);
}

/// 将HSV模式的值转换到一个uint32中，（分别是r,g,b,a）
uint32_t CZColor::hash2Int()
{
	uint32_t h = uint32_t (255.f * red);
	uint32_t s = uint32_t (255.f * green);
	uint32_t b = uint32_t (255.f * blue);
	uint32_t a = uint32_t (255.f * alpha);
	return (h << 24) | (s << 16) | (b << 8) | (a);
}

/// 判断是否相等
bool CZColor::operator ==( const  CZColor& c_) const
{
	if ( this->hue==c_.hue && this->saturation==c_.saturation && this->brightness==c_.brightness && this->alpha==c_.alpha )
	{
		return true;
	} 
	else
	{
		return false;
	}
}

/// 赋值
CZColor& CZColor::operator =( const  CZColor & p_)
{
	this->red = p_.red;
	this->green = p_.green;
	this->blue = p_.blue;
	this->alpha = p_.alpha;
	this->hue = p_.hue;
	this->saturation = p_.saturation;
	this->brightness = p_.brightness;
	return *this;
}

/// 编解码
void CZColor::update(CZDecoder *decoder_, bool deep /* = false */)
{
	/*
	hue_ =  WDClamp(0.0f, 1.0f, [decoder decodeFloatForKey:WDHueKey]);
	saturation_ = WDClamp(0.0f, 1.0f, [decoder decodeFloatForKey:WDSaturationKey]);
	brightness_ = WDClamp(0.0f, 1.0f, [decoder decodeFloatForKey:WDBrightnessKey]);
	alpha_ = WDClamp(0.0f, 1.0f, [decoder decodeFloatForKey:WDAlphaKey]);
	*/
};
void CZColor::encode(CZCoder *coder_, bool deep /* = false */)
{
	/*
	[coder encodeFloat:hue_ forKey:WDHueKey];
	[coder encodeFloat:saturation_ forKey:WDSaturationKey];
	[coder encodeFloat:brightness_ forKey:WDBrightnessKey];
	[coder encodeFloat:alpha_ forKey:WDAlphaKey];
	*/
};