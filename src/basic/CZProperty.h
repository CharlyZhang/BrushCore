
///  \file CZProperty.h
///  \brief This is the file declare the Class CZProperty and the Interface CZPropertyDelegate.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-12
///  \note

#ifndef _CZPROPERTY_H_
#define _CZPROPERTY_H_

#include "../serialization/CZCoding.h"
#include <string>

class CZPropertyDelegate;

class CZProperty : public CZCoding
{
public:
	CZProperty();
	CZProperty & operator = (const CZProperty&);
	bool operator == (const CZProperty&);
	/// 设置数值
	void setValue(float v);
	/// 随机化
	void randomize();
	/// 操纵
	bool canIncrement();
	void increment();
	bool canDecrement();
	void decrement();

	/// 序列化接口
	void update(CZDecoder *decoder_, bool deep = false){};
	void encode(CZCoder *coder_, bool deep = false){};

	float value;
	float maximumValue, minimumValue;
	float conversionFactor;
	bool percentage;
	std::string title;

	CZPropertyDelegate *ptrDelegate;
};

class CZPropertyDelegate 
{
public:
	virtual void propertyChanged(CZProperty *property_) = 0;
};

#endif