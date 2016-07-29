
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

/// �ռ�任��
class CZAffineTransform
{
public:
	float a, b, c, d;
	float tx, ty;

	CZAffineTransform():a(0),b(0),c(0),d(0),tx(0),ty(0){};
	CZAffineTransform(float a_, float b_, float c_, float d_, float tx_, float ty_): 
		a(a_),b(b_),c(c_),d(d_),tx(tx_),ty(ty_){};
	
	/// ���������
	CZAffineTransform operator*(const CZAffineTransform &aTrans_) const;
	bool operator==(const CZAffineTransform &aTrans_) const;
    CZAffineTransform&  operator=( const  CZAffineTransform & a_) ;

	/// ���任����ת
	int rotate(float angle_);
	/// ���任��ƽ��
	int translate(float tx_, float ty_);
    /// scale
    int scale(float sx, float sy);
    
    /// invert
    CZAffineTransform& getInvert();
	/// Ӧ���ڶ�ά��p
	CZ2DPoint applyTo2DPoint(const CZ2DPoint & p_) const;
	/// Ӧ���ھ���
	CZRect applyToRect(CZRect & rect_);
	/// �Ƿ��ǵ�λ����
	bool isIdentity();

	/// ����ƽ��ת��
	static CZAffineTransform makeFromTranslation(float tx_, float ty_);
	/// ����ѡ��ת��
	static CZAffineTransform makeFromRotate(float angle_);
    static CZAffineTransform makeFromScale(float sx_, float sy_);
	/// ���ɵ�λת��
	static CZAffineTransform makeIdentity();
};

#endif