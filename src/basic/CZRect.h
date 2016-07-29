
#ifndef _CZRECT_H_
#define _CZRECT_H_

#include "CZ2DPoint.h"
#include "CZSize.h"

/// 矩形类
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

	/// 返回零矩形
	static CZRect zeroRect(){ return CZRect(); };
	/// 判断是否为零矩形
	bool isZeroRect() const;
	/// 合并其他矩形
	CZRect unionWith(const CZRect & rect_);
	/// 求与其他矩形的交集
	CZRect intersectWith(const CZRect & rect_);
	/// 扩展的整数矩形
	CZRect expandToIntergral();
};

#endif