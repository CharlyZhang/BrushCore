
#ifndef _CZRECT_H_
#define _CZRECT_H_

#include "CZ2DPoint.h"
#include "CZSize.h"

/// ������
class CZRect
{
public:
	CZ2DPoint	origin;
	CZSize		size;	

	CZRect(): origin(0.0f,0.0f), size(0.0f,0.0f){};
	CZRect(float x_, float y_, float w_, float h_): origin(x_,y_), size(w_,h_){};

	inline float getMinX() const { return origin.x; };
	inline float getMinY() const { return origin.y; };
	inline float getMaxX() const { return origin.x + size.width; };
	inline float getMaxY() const { return origin.y + size.height; };
	inline CZ2DPoint getCenter()
	{ 
		return CZ2DPoint(origin.x + size.width/2, origin.y + size.height/2); 
	};

	/// ���������
	static CZRect zeroRect(){ return CZRect(); };
	/// �ж��Ƿ�Ϊ�����
	bool isZeroRect() const;
	/// �ϲ���������
	CZRect unionWith(const CZRect & rect_);
	/// �����������εĽ���
	CZRect intersectWith(const CZRect & rect_);
	/// ��չ����������
	CZRect expandToIntergral();
};

#endif