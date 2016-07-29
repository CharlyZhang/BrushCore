
///  \file CZColor.h
///  \brief This is the file declare the class CZColor.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-25
///  \note

#ifndef _CZCOLOR_H_
#define _CZCOLOR_H_

#include "../serialization/CZCoding.h"
#include <stdint.h>			/// for uint32_t

class CZColor: public CZCoding 
{
public:
	/// 默认用rgba模式初始化
	CZColor(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f){initWithRGB(r,g,b,a);};
	/// 采用RGB模式初始化
	void initWithRGB(float r, float g, float b, float a);
	/// 采用HSV模式初始化
	/// 
	///		\param h - 色调
	///		\param s - 饱和度
	///		\param v - 亮度
	///		\param a - 不透明度
	void initWithHSV(float h, float s, float v, float a);

	/// 返回随机颜色
	static CZColor randomColor(); 
	/// 返回特定颜色的静态函数
	static inline CZColor blackColor()				{ return CZColor(0.0f,0.0f,0.0f,1.0f);}
	static inline CZColor whiteColor(float a=1.0f)	{ return CZColor(1.0f,1.0f,1.0f,a);}
	static inline CZColor grayColor()				{ return CZColor(.25f,.25f,.025f,1.0f);}
	static inline CZColor cyanColor()				{ return CZColor(0.0f,1.0f,1.0f,1.0f);}	///< 青色
	static inline CZColor redColor()				{ return CZColor(1.0f,0.0f,0.0f,1.0f);}
	static inline CZColor magentaColor()			{ return CZColor(1.0f,0.0f,1.0f,1.0f);}	///< 品红
	static inline CZColor greenColor()				{ return CZColor(0.0f,1.0f,0.0f,1.0f);}
	static inline CZColor yellowColor()				{ return CZColor(1.0f,1.0f,0.0f,1.0f);}
	static inline CZColor blueColor()				{ return CZColor(0.0f,0.0f,1.0f,1.0f);}
    
    static inline CZColor* CZColorWithHSV(float h, float s, float v, float a) {
        CZColor *ret = new CZColor;
        ret->initWithHSV(h, s, v, a);
        return ret;
    }
	
	/// 返回不透明的该值
	inline CZColor colorAdjustAlpha(float a_=1.0f)	{ return CZColor(red,green,blue,a_);}
	/// 返回取反颜色
	inline CZColor colorInverted()					{ return CZColor(1.0f-red,1.0f-green,1.0f-blue,alpha);}
	/// 去饱和度
	inline CZColor colorDesaturated()				{ CZColor color; color.initWithHSV(hue,0,brightness,alpha); return color;}
	
	/// 混合其他颜色
	CZColor colorBlend(const CZColor &c_, float fraction_);
	/// 用RGB模式调整
	void balanceRGB(float rShift, float gShift, float bShift);
	/// 用HSV模式调整
	void adjustHSV(float hShift, float sShift, float bShift);

	/// 将HSV模式的值转换到一个uint32中，（分别是r,g,b,a）
	uint32_t hash2Int();

	/// 判断是否相等
	bool  operator==( const  CZColor & c_) const ;
	/// 赋值
	CZColor&  operator =( const  CZColor & p_); 

	/// 编解码
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);

	/// 成员变量
	float hue;				///< 色调
	float saturation;		///< 饱和度
	float brightness;		///< 亮度
	float alpha;		
	float red;
	float green;
	float blue;
};

#endif