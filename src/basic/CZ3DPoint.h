
///  \file CZ3DPoint.h
///  \brief This is the file declare the CZ3DPoint Class.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-12
///  \note		We can also use the CZ3DPoint Class as 3DVector

#ifndef _CZ3DPOINT_H_
#define _CZ3DPOINT_H_

#include <iostream>

class CZ3DPoint 
{
public:
	float x,y,z;

	CZ3DPoint(float x_=0.0, float y_=0.0, float z_=0.0): x(x_), y(y_), z(z_){};

	bool  operator<( const  CZ3DPoint & p_) const ;
	bool  operator==( const  CZ3DPoint & p_) const ;
	CZ3DPoint&  operator=( const  CZ3DPoint & p_) ;

	/// Here we overload operators 
	CZ3DPoint operator+(CZ3DPoint vVector)
	{
		// Return the added vectors result.
		return CZ3DPoint(vVector.x + x, vVector.y + y, vVector.z + z);
	}
	CZ3DPoint operator-(CZ3DPoint vVector)
	{
		// Return the subtracted vectors result
		return CZ3DPoint(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	CZ3DPoint operator*(float num)
	{
		// Return the scaled vector
		return CZ3DPoint(x * num, y * num, z * num);
	}
	float operator*(CZ3DPoint vVector)
	{
		return (x*vVector.x+y*vVector.y+z*vVector.z);
	}
	CZ3DPoint operator/(float num)
	{
		// Return the scale vector
		return CZ3DPoint(x / num, y / num, z / num);
	}

	/// Here we overload the iostream
	friend std::ostream& operator<<(std::ostream &ostr, CZ3DPoint v);
	friend std::istream& operator>>(std::istream &istr, CZ3DPoint & v);

	///cast to pointer to float for glVertex4fv etc
	operator float* () const {return (float*) this;}
	operator const float* () const {return (const float*) this;}

	/// Here use the class to represent color, and set the values from an unsigned int
	void SetFromUIntColor(unsigned int col) 
	{
		x = ((float)(((unsigned char*)&col)[0]))/255.0f;
		y = ((float)(((unsigned char*)&col)[1]))/255.0f;
		z = ((float)(((unsigned char*)&col)[2]))/255.0f;
	}

	/// 求到另外一点距离
	float distanceTo3DPoint(CZ3DPoint & p_);

	/// 判断是否与某指针指向的对象相等
	bool isEqual(CZ3DPoint *ptrPoint_);
	/// 求点的级数（到零点的距离）
	float magnitude();
	/// 求点的规范化点
	CZ3DPoint normalizedPoint();
	/// 规范化本点
	void normalize();
};

#endif
