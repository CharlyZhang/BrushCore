
///  \file CZAffineTransform.cpp
///  \brief This is the file implement the Class CZAffineTransform.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-11
///  \note

#include "CZAffineTransform.h"
#include "../CZUtil.h"

/// ���������
CZAffineTransform CZAffineTransform::operator*(const CZAffineTransform &aTrans_) const
{
	return CZAffineTransform(a*aTrans_.a + b*aTrans_.c,
							 a*aTrans_.b + b*aTrans_.d,
							 c*aTrans_.a + d*aTrans_.c,
							 c*aTrans_.b + d*aTrans_.d,
							 tx*aTrans_.a + ty*aTrans_.c + aTrans_.tx,
							 tx*aTrans_.b + ty*aTrans_.d + aTrans_.ty);
}
bool CZAffineTransform::operator==(const CZAffineTransform &aTrans_) const
{
	return(a == aTrans_.a && b == aTrans_.b && c == aTrans_.c
		&& d == aTrans_.d && tx == aTrans_.tx && ty == aTrans_.ty);
}

CZAffineTransform&  CZAffineTransform::operator=( const  CZAffineTransform & a_)
{
    this->a = a_.a;
    this->b = a_.b;
    this->c = a_.c;
    this->d = a_.d;
    this->tx = a_.tx;
    this->ty = a_.ty;
    return *this;
}

/// ���任����ת
int CZAffineTransform::rotate(float angle_)
{
	*this = CZAffineTransform::makeFromRotate(angle_) * (*this);
	return 0;
}

/// ���任��ƽ��
int CZAffineTransform::translate(float tx_, float ty_)
{
	*this = CZAffineTransform::makeFromTranslation(tx_,ty_) * (*this);
	return 0;
}

/// scale
int CZAffineTransform::scale(float sx, float sy)
{
    *this = CZAffineTransform::makeFromScale(sx, sy) * (*this);
    return 0;
}

/// invert
CZAffineTransform& CZAffineTransform::getInvert()
{
    return *this;
}

/// Ӧ���ڶ�ά��p
CZ2DPoint CZAffineTransform::applyTo2DPoint(const CZ2DPoint & p_) const
{
	CZ2DPoint ret;
	ret.x = float( (double)a*p_.x + (double)c*p_.y + tx);
	ret.y = float( (double)b*p_.x + (double)d*p_.y + ty);

	return ret;
}

/// Ӧ���ھ���
CZRect CZAffineTransform::applyToRect(CZRect & rect_)
{
	CZ2DPoint a = CZ2DPoint(rect_.getMinX(), rect_.getMinY());
	CZ2DPoint b = CZ2DPoint(rect_.getMaxX(), rect_.getMinY());
	CZ2DPoint c = CZ2DPoint(rect_.getMinX(), rect_.getMaxY());
	CZ2DPoint d = CZ2DPoint(rect_.getMaxX(), rect_.getMaxY());

	a = this->applyTo2DPoint(a);
	b = this->applyTo2DPoint(b);
	c = this->applyTo2DPoint(c);
	d = this->applyTo2DPoint(d);

	float minX = CZUtil::Min(a.x, CZUtil::Min(b.x, CZUtil::Min(c.x,d.x)));
	float maxX = CZUtil::Max(a.x, CZUtil::Max(b.x, CZUtil::Max(c.x,d.x)));
	float minY = CZUtil::Min(a.y, CZUtil::Min(b.y, CZUtil::Min(c.y,d.y)));
	float maxY = CZUtil::Max(a.y, CZUtil::Max(b.y, CZUtil::Max(c.y,d.y)));

	return CZRect(minX,minY, maxX-minX, maxY-minY);
}

/// �Ƿ��ǵ�λ����
bool CZAffineTransform::isIdentity()
{
	if(a == 1 && b == 0 && c == 0 &&
		d == 1 && tx == 0 && ty == 0) 
		return true;
	else
		return false;
}

/// ����ƽ��ת��
CZAffineTransform CZAffineTransform::makeFromTranslation(float tx_, float ty_)
{
	return CZAffineTransform(1,0,0,1,tx_,ty_);
}

/// ������תת��
CZAffineTransform CZAffineTransform::makeFromRotate(float angle_)
{
	return CZAffineTransform(cos(angle_), sin(angle_), -sin(angle_), cos(angle_), 0,0);
}

CZAffineTransform CZAffineTransform::makeFromScale(float sx_, float sy_)
{
    return CZAffineTransform(sx_,0,0,sy_,0,0);
}

/// ���ɵ�λת��
CZAffineTransform CZAffineTransform::makeIdentity()
{
	return CZAffineTransform(1, 0, 0, 1, 0,0);
};