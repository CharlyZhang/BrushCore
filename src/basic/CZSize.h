#ifndef _CZSIZE_H_
#define _CZSIZE_H_

/// ³ß´ç
class CZSize
{
public:
	float width,height;

	CZSize(float w_ = 0.0f, float  h_ = 0.0f): width(w_), height(h_){};

	bool operator==(const CZSize &s_)
	{
		if(width == s_.width && height == s_.height) return true;
		else										 return false;
	}

	CZSize operator*(float num) const;
};

#endif