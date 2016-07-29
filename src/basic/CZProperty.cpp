
#include "CZProperty.h"
#include "../CZUtil.h"
#include <stdlib.h>

CZProperty::CZProperty()
{
	minimumValue = 0.0f;
	maximumValue = 1.0f;
	conversionFactor = 100.0f;
}

CZProperty& CZProperty::operator=(const CZProperty& pro)
{
	title = pro.title;
	percentage = pro.percentage;
	conversionFactor = pro.conversionFactor;
	minimumValue = pro.minimumValue;
	maximumValue = pro.maximumValue;
	value = pro.value;
	return *this;
}

bool CZProperty::operator==(const CZProperty& pro)
{
	return (title == pro.title	&&
			percentage == pro.percentage &&
			conversionFactor == pro.conversionFactor &&
			minimumValue == pro.minimumValue &&
			maximumValue == pro.maximumValue &&
			value == pro.value);
}

/// 设置数值
void CZProperty::setValue(float v)
{
	//float clampedV = CZUtil::Clamp(minimumValue,maximumValue,v);
	//if (clampedV != value)
	//{
	//	value = v;
	//	if(ptrDelegate)	ptrDelegate->propertyChanged(this);
	//	else			LOG_WARN("Property delegate has bot been denigated");
	//}

}
/// 随机化
void CZProperty::randomize()
{
	//float r = CZUtil::RandomFloat() * (maximumValue - minimumValue) + minimumValue;
	//value = round(r * conversionFactor) / conversionFactor;
}

/// 操纵
bool CZProperty::canIncrement()
{
	return value < maximumValue;
}
void CZProperty::increment()
{
	//setValue((round(value*conversionFactor)+1) / conversionFactor);
}
bool CZProperty::canDecrement()
{
	return value > minimumValue;
}
void CZProperty::decrement()
{
	//setValue((round(value*conversionFactor)-1) / conversionFactor);
}