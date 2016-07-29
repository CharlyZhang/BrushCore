
///  \file CZBezierSegment.h
///  \brief This is the file declare the Class CZBezierSegment.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-15
///  \note

#ifndef _CZBEZIERSEGMENT_H_
#define _CZBEZIERSEGMENT_H_

#include "CZBezierNode.h"
#include "../basic/CZ3DPoint.h"
#include "../CZDefine.h"
#include <vector>

class CZBezierSegment
{
public:
	CZBezierSegment(){};
	CZBezierSegment(CZ3DPoint &start_, CZ3DPoint &out_, CZ3DPoint &in_, CZ3DPoint &end_);
	~CZBezierSegment();
	/// 由两点构造贝塞尔线段（静态函数）
	/// 
	///		/param start_	- 贝塞尔曲线起点
	///		/param end_		- 贝塞尔曲线终点
	///		/return			- 生成的贝塞尔曲线
	static CZBezierSegment* segmentBetweenNodes(const CZBezierNode &start_, const CZBezierNode &end_);
	/// 将贝塞尔曲线打散成绘制点
	///
	///		/param &points	- 盛装打散的绘制点的容器
	void flattenIntoArray(std::vector<CZ3DPoint> & points);
	/// 判断贝塞尔曲线是否平整？
	///
	///		暂时还不明白，细节算法.
	/// 
	///		/param tolerance	- 误差值
	///
	bool isFlatWithTolerance(float tolerance);
	/// 将本线段从t处分割
	///
	///		利用几何法，将本线段的四个控制点形成的贝塞尔曲线从t处分割成两条三次贝塞尔曲线
	/// 
	///		/param t	- 分割本贝塞尔曲线的位置参数[0,1]
	///		/param *L	- 分割后左部分的贝塞尔曲线
	///		/param *R	- 分割后右部分的贝塞尔曲线
	///		/return		- 分割处曲线上的点
	/// 
	CZ3DPoint splitAtT(float t, CZBezierSegment *L, CZBezierSegment *R);

	/// 依次为该贝塞尔曲线四个控制点
	CZ3DPoint start;
	CZ3DPoint outHandle;
	CZ3DPoint inHandle;
	CZ3DPoint end;
};

#endif