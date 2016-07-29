
///  \file CZBezierNode.h
///  \brief This is the file declare the Class CZBezierNode.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-15
///  \note

#ifndef _CZBEZIERNODE_H_
#define _CZBEZIERNODE_H_

#include "../basic/CZ3DPoint.h"
#include "../basic/CZ2DPoint.h"

class CZBezierNode
{
public:
	/// 锚点和其两个控制点（z用来表示该点的压力值）
	CZ3DPoint inPoint;
	CZ3DPoint anchorPoint;
	CZ3DPoint outPoint;

	CZBezierNode(){}
	CZBezierNode(const CZ2DPoint &location_, float pressure_);
	CZBezierNode(const CZ3DPoint & anchor_);
	CZBezierNode(const CZ3DPoint &in_, const CZ3DPoint &anchor_, const CZ3DPoint &out_):inPoint(in_),outPoint(out_),anchorPoint(anchor_){};
	~CZBezierNode();

	CZBezierNode& operator = (const CZBezierNode& bn);
};
#endif