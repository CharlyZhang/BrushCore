
#include "CZRect.h"
#include "../CZUtil.h"

/// 判断是否为零矩形
bool CZRect::isZeroRect() const
{
	if(origin.x==0.0f && origin.y==0.0f
		&& size.width ==0.0f && size.height ==0.0f)
		return true;
	else
		return false;
}

/// 合并其他矩形
CZRect CZRect::unionWith(const CZRect & rect_)
{
	if(isZeroRect())			return rect_;
	else if(rect_.isZeroRect()) return *this;

	float minX = CZUtil::Min(this->getMinX(), rect_.getMinX());
	float minY = CZUtil::Min(this->getMinY(), rect_.getMinY());
	float maxX = CZUtil::Max(this->getMaxX(), rect_.getMaxX());
	float maxY = CZUtil::Max(this->getMaxY(), rect_.getMaxY());

	return CZRect(minX, minY, maxX-minX, maxY-minY);
}

/// 求与其他矩形的交集
CZRect CZRect::intersectWith(const CZRect & rect_)
{
	if(!isZeroRect() && !rect_.isZeroRect())
	{
		float minX1 = getMinX();	float minX2 = rect_.getMinX();
		float minY1 = getMinY();	float minY2 = rect_.getMinY();
		float maxX1 = getMaxX();	float maxX2 = rect_.getMaxX();
		float maxY1 = getMaxY();	float maxY2 = rect_.getMaxY();

		if ((maxX1 > minX2) &&
			(minX1 < maxX2) &&
			(minY1 < maxY2) &&
			(maxY1 > minY2))
		{
			CZRect temp;

			// fill in temp with the intersection
			temp.origin.x = (minX1 > minX2) ? minX1 : minX2;
			temp.origin.y = (minY1 > minY2) ? minY1 : minY2;
			temp.size.width = ((maxX1 < maxX2) ? maxX1 : maxX2)-temp.origin.x;
			temp.size.height = ((maxY1 < maxY2) ? maxY1 : maxY2)-temp.origin.y;

			return temp;
		}
	}

	return  CZRect();
}

/// 扩展的整数矩形
CZRect CZRect::expandToIntergral()
{
	float minX = floor(this->getMinX());
	float minY = floor(this->getMinY());
	float maxX = ceil(this->getMaxX());
	float maxY = ceil(this->getMaxY());

	return CZRect(minX, minY, maxX-minX, maxY-minY);
}