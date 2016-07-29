
///  \file CPath.h
///  \brief This is the file declare the Class CZ2DPoint.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2015-03-26
///  \note		

#ifndef _CZ2DPOINT_H_
#define _CZ2DPOINT_H_

/// 二维点
class CZ2DPoint
{
public:
	float x,y;
	CZ2DPoint(float x_ = 0.0f, float y_ = 0.0f): x(x_), y(y_){};

	bool  operator<( const  CZ2DPoint & p_) const ;
	bool  operator==( const  CZ2DPoint & p_) const ;
	CZ2DPoint&  operator=( const  CZ2DPoint & p_) ;

	/// Here we overload operators 
	inline CZ2DPoint operator+(CZ2DPoint vVector)	{ return CZ2DPoint(vVector.x + x, vVector.y + y);}
	inline CZ2DPoint operator-(CZ2DPoint vVector)	{ return CZ2DPoint(x - vVector.x, y - vVector.y);}
	inline CZ2DPoint operator*(float num)	{ return CZ2DPoint(x * num, y * num);}
	inline float operator*(CZ2DPoint vVector)	{ return (x*vVector.x+y*vVector.y);}
	inline CZ2DPoint operator/(float num)	{ return CZ2DPoint(x / num, y / num); }

	/// 求到另外一点距离
	float distanceTo2DPoint(CZ2DPoint & p_);
};


#endif