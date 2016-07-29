
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
	/// Ĭ����rgbaģʽ��ʼ��
	CZColor(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f){initWithRGB(r,g,b,a);};
	/// ����RGBģʽ��ʼ��
	void initWithRGB(float r, float g, float b, float a);
	/// ����HSVģʽ��ʼ��
	/// 
	///		\param h - ɫ��
	///		\param s - ���Ͷ�
	///		\param v - ����
	///		\param a - ��͸����
	void initWithHSV(float h, float s, float v, float a);

	/// ���������ɫ
	static CZColor randomColor(); 
	/// �����ض���ɫ�ľ�̬����
	static inline CZColor blackColor()				{ return CZColor(0.0f,0.0f,0.0f,1.0f);}
	static inline CZColor whiteColor(float a=1.0f)	{ return CZColor(1.0f,1.0f,1.0f,a);}
	static inline CZColor grayColor()				{ return CZColor(.25f,.25f,.025f,1.0f);}
	static inline CZColor cyanColor()				{ return CZColor(0.0f,1.0f,1.0f,1.0f);}	///< ��ɫ
	static inline CZColor redColor()				{ return CZColor(1.0f,0.0f,0.0f,1.0f);}
	static inline CZColor magentaColor()			{ return CZColor(1.0f,0.0f,1.0f,1.0f);}	///< Ʒ��
	static inline CZColor greenColor()				{ return CZColor(0.0f,1.0f,0.0f,1.0f);}
	static inline CZColor yellowColor()				{ return CZColor(1.0f,1.0f,0.0f,1.0f);}
	static inline CZColor blueColor()				{ return CZColor(0.0f,0.0f,1.0f,1.0f);}
    
    static inline CZColor* CZColorWithHSV(float h, float s, float v, float a) {
        CZColor *ret = new CZColor;
        ret->initWithHSV(h, s, v, a);
        return ret;
    }
	
	/// ���ز�͸���ĸ�ֵ
	inline CZColor colorAdjustAlpha(float a_=1.0f)	{ return CZColor(red,green,blue,a_);}
	/// ����ȡ����ɫ
	inline CZColor colorInverted()					{ return CZColor(1.0f-red,1.0f-green,1.0f-blue,alpha);}
	/// ȥ���Ͷ�
	inline CZColor colorDesaturated()				{ CZColor color; color.initWithHSV(hue,0,brightness,alpha); return color;}
	
	/// ���������ɫ
	CZColor colorBlend(const CZColor &c_, float fraction_);
	/// ��RGBģʽ����
	void balanceRGB(float rShift, float gShift, float bShift);
	/// ��HSVģʽ����
	void adjustHSV(float hShift, float sShift, float bShift);

	/// ��HSVģʽ��ֵת����һ��uint32�У����ֱ���r,g,b,a��
	uint32_t hash2Int();

	/// �ж��Ƿ����
	bool  operator==( const  CZColor & c_) const ;
	/// ��ֵ
	CZColor&  operator =( const  CZColor & p_); 

	/// �����
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);

	/// ��Ա����
	float hue;				///< ɫ��
	float saturation;		///< ���Ͷ�
	float brightness;		///< ����
	float alpha;		
	float red;
	float green;
	float blue;
};

#endif