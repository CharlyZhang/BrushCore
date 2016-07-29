
///  \file CZAffineTransform.h
///  \brief This is the file declare the Class CZAffineTransform.
///
///		recode as 2D in ios
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-11
///  \note

#ifndef _CZAFFINETRANSFORM_H_
#define _CZAFFINETRANSFORM_H_

#include "CZ2DPoint.h"
#include "CZRect.h"

/// 空间变换类
class CZAffineTransform
{
public:
	float a, b, c, d;
	float tx, ty;

	CZAffineTransform():a(0),b(0),c(0),d(0),tx(0),ty(0){};
	CZAffineTransform(float a_, float b_, float c_, float d_, float tx_, float ty_): 
		a(a_),b(b_),c(c_),d(d_),tx(tx_),ty(ty_){};
	
	/// 重载运算符
	CZAffineTransform operator*(const CZAffineTransform &aTrans_) const;
	bool operator==(const CZAffineTransform &aTrans_) const;
    CZAffineTransform&  operator=( const  CZAffineTransform & a_) ;

	/// 将变换再旋转
	int rotate(float angle_);
	/// 将变换再平移
	int translate(float tx_, float ty_);
    /// scale
    int scale(float sx, float sy);
    
    /// invert
    CZAffineTransform& getInvert();
	/// 应用于二维点p
	CZ2DPoint applyTo2DPoint(const CZ2DPoint & p_) const;
	/// 应用于矩形
	CZRect applyToRect(CZRect & rect_);
	/// 是否是单位矩阵
	bool isIdentity();

	/// 生成平移转换
	static CZAffineTransform makeFromTranslation(float tx_, float ty_);
	/// 生成选择转换
	static CZAffineTransform makeFromRotate(float angle_);
    static CZAffineTransform makeFromScale(float sx_, float sy_);
	/// 生成单位转换
	static CZAffineTransform makeIdentity();
};

#endif