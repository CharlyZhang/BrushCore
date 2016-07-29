
///  \file CPath.cpp
///  \brief This is the file implements the Class CZ2DPoint.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2015-03-26
///  \note	

#include "CZ2DPoint.h"
#include <cmath>

/// 重载运算符
bool CZ2DPoint::operator <( const  CZ2DPoint& p_) const
{
	if ( this->x < p_.x )
	{
		return true;
	} 
	else if ( this->x==p_.x && this->y < p_.y )
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CZ2DPoint::operator ==( const  CZ2DPoint& p_) const
{
	if ( this->x==p_.x && this->y==p_.y)
	{
		return true;
	} 
	else
	{
		return false;
	}
}
CZ2DPoint& CZ2DPoint::operator =( const  CZ2DPoint& p_) 
{
	this->x = p_.x; 
	this->y = p_.y;

	return *this;
}

/// 求到另外一点距离
float CZ2DPoint::distanceTo2DPoint(CZ2DPoint & p_)
{ 
	return sqrt((x-p_.x)*(x-p_.x) + (y-p_.y)*(y-p_.y)); 
}