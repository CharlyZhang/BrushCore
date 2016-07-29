
///  \file CZ3DPoint.cpp
///  \brief This is the file implement the Class CZ3DPoint.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-12
///  \note

#include "CZ3DPoint.h"
#include <cmath>

/// 重载运算符
bool CZ3DPoint::operator <( const  CZ3DPoint& p_) const
{
	if ( this->x < p_.x )
	{
		return true;
	} 
	else if ( this->x==p_.x && this->y < p_.y )
	{
		return true;
	}
	else if ( this->x==p_.x && this->y==p_.y && this->z<p_.z )
	{
		return true;
	} 
	else
	{
		return false;
	}
}
bool CZ3DPoint::operator ==( const  CZ3DPoint& p_) const
{
	if ( this->x==p_.x && this->y==p_.y && this->z==p_.z )
	{
		return true;
	} 
	else
	{
		return false;
	}
}
CZ3DPoint& CZ3DPoint::operator =( const  CZ3DPoint& p_) 
{
	this->x = p_.x; 
	this->y = p_.y;
	this->z = p_.z;
	return *this;
}

std::ostream& operator<<(std::ostream &ostr, CZ3DPoint v) 
{
	return ostr << "(" <<v.x << " " << v.y << " " << v.z << ")";
}
std::istream& operator>>(std::istream &istr, CZ3DPoint & v) 
{
	return istr >> v.x >> v.y >> v.z;
}

/// 求到另外一点距离
float CZ3DPoint::distanceTo3DPoint(CZ3DPoint & p_)
{ 
	return sqrt((x-p_.x)*(x-p_.x) + (y-p_.y)*(y-p_.y) + (z-p_.z)*(z-p_.z)); 
}

bool CZ3DPoint::isEqual(CZ3DPoint *ptrPoint_)
{
	if(ptrPoint_ == this) return true;
	if(!ptrPoint_)		  return false;
	
	return (this->x==ptrPoint_->x &&
			this->y==ptrPoint_->y &&
			this->z==ptrPoint_->z);
}

/// 求点的级数（到零点的距离）
float CZ3DPoint::magnitude()
{
	return sqrt(x*x + y*y + z*z);
}

/// 求点的规范化点
CZ3DPoint CZ3DPoint::normalizedPoint()
{
	return *this / magnitude();
}

/// 规范化本点
void CZ3DPoint::normalize()
{
	*this = *this / magnitude();
}