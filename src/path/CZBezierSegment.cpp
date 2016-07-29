
///  \file CZBezierSegment.cpp
///  \brief This is the file implement the Class CZBezierSegment.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-15
///  \note		将点的数据声明为局部变量，而不是指针会导致空间浪费

#include "CZBezierSegment.h"
#include <cmath>

const float kDefaultFlatness = 1;


CZBezierSegment::CZBezierSegment(CZ3DPoint &start_, CZ3DPoint &out_, CZ3DPoint &in_, CZ3DPoint &end_)
{
	start = start_;
	inHandle = in_;
	outHandle = out_;
	end = end_;
}
CZBezierSegment::~CZBezierSegment(){};

/// 由两点构造贝塞尔线段（静态函数）
/// 
///		/param start_	- 贝塞尔曲线起点
///		/param end_		- 贝塞尔曲线终点
///		/return			- 生成的贝塞尔曲线
CZBezierSegment * CZBezierSegment::segmentBetweenNodes(const CZBezierNode &start_, const CZBezierNode &end_)
{
	CZBezierSegment *segment = new CZBezierSegment;

	segment->start = start_.anchorPoint;
	segment->outHandle = start_.outPoint;
	segment->inHandle = end_.inPoint;
	segment->end = end_.anchorPoint;

	return segment;
}

void CZBezierSegment::flattenIntoArray(std::vector<CZ3DPoint> & points)
{
	if (isFlatWithTolerance(kDefaultFlatness)) 
	{
		if (points.size() == 0) 
		{
			points.push_back(this->start);
		}
		points.push_back(this->end);
	} 
	else
	{
		// recursive case
		CZBezierSegment L, R;

		this->splitAtT(0.5f, &L, &R);

		L.flattenIntoArray(points);
		R.flattenIntoArray(points);
	}
}

bool CZBezierSegment::isFlatWithTolerance(float tolerance)
{
	if (start == outHandle && inHandle ==end ) 
	{
		return true;
	}

	CZ3DPoint delta = end - start;

	float dx = delta.x;
	float dy = delta.y;

	float d2 = fabs((outHandle.x - end.x) * dy - (outHandle.y - end.y) * dx);
	float d3 = fabs((inHandle.x - end.x) * dy - (inHandle.y - end.y) * dx);

	if ((d2 + d3) * (d2 + d3) <= tolerance * (dx * dx + dy * dy)) 
		return true;

	return false;
}

CZ3DPoint CZBezierSegment::splitAtT(float t, CZBezierSegment *L, CZBezierSegment *R)
{
	CZ3DPoint A,B,C,D,E,F;

	A = start		+ ( (outHandle - start) * t);
	B = outHandle	+ ( (inHandle - outHandle) * t);
	C = inHandle	+ ( (end - inHandle) * t);

	D = A	+ ( (B - A) * t);
	E = B	+ ( (C - B) * t);
	F = D	+ ( (E - D) * t);

	if (L) 
	{
		(*L).start = start;
		(*L).outHandle = A;
		(*L).inHandle = D;
		(*L).end = F;
	}

	if (R) 
	{
		(*R).start = F;
		(*R).outHandle = E;
		(*R).inHandle = C;
		(*R).end = end;
	}

	if ((L || R) && start == outHandle && inHandle == end ) 
	{
		// no curves    
		if (L) 
		{
			(*L).inHandle = (*L).end;
		}
		if (R) 
		{
			(*R).outHandle = (*R).start;
		}
	}

	return F;
}
