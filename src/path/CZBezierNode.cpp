
///  \file CZBezierNode.cpp
///  \brief This is the file implement the Class CZBezierNode.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-15
///  \note

#include "CZBezierNode.h"

CZBezierNode::~CZBezierNode(){}
CZBezierNode::CZBezierNode(const CZ2DPoint &location_, float pressure_)
{
	CZ3DPoint p(location_.x, location_.y, pressure_);
	inPoint = p;
	anchorPoint = p;
	outPoint = p;
}
CZBezierNode::CZBezierNode(const CZ3DPoint & anchor_)
{
	inPoint = anchor_;
	anchorPoint = anchor_;
	outPoint = anchor_;
}

CZBezierNode& CZBezierNode::operator = (const CZBezierNode& bn)
{
	inPoint = bn.inPoint;
	anchorPoint = bn.anchorPoint;
	outPoint = bn.anchorPoint;
	return *this;
}