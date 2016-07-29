
///  \file CZRandom.cpp
///  \brief This is the file implement the Class CZRandom.
///
///		The class CZRandom is a pseudorandom number generator, using Mersenne twister.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-25
///  \note

#include "CZRandom.h"

CZRandom::CZRandom(uint32_t s_ /* = 156248 */)
{
	initWithSeed(s_);
}

/// �����ӳ�ʼ��
void CZRandom::initWithSeed(uint32_t s_)
{
	MT[0] = s_;
	for (int i = 1; i < 624; i++) 
	{
		MT[i] = (1812433253 * (MT[i-1] ^ (MT[i-1] >> 30)) + i);
	}

	ix = 0;
}

/// ���������
void CZRandom::generateNumbers()
{
	for (int i = 0; i < 624; i++) 
	{ 
		uint32_t y = (MT[i] & 0x80000000) + (MT[(i+1) % 624] & (0x7FFFFFFF));
		MT[i] = MT[(i + 397) % 624] ^ (y >> 1);

		if (y % 2 == 1) {
			MT[i] = MT[i] ^ 2567483615;
		}
	}
}

/// ����һ������
uint32_t CZRandom::nextInt()
{
	if (ix == 0)  generateNumbers();

	uint32_t y = MT[ix];

	y = y ^ (y >> 11);
	y = y ^ ((y << 7) & 2636928640);
	y = y ^ ((y << 15) & 4022730752);
	y = y ^ (y >> 18);

	ix = (ix + 1) % 624;
	return y;
}

/// ����һ�������ͣ�Ĭ��Ϊ[0,1]��Χ��
float CZRandom::nextFloat(float min_ /* = 0.0f */, float max_ /* = 1.0f */)
{
	float r = (float ) (nextInt() % 100000);
	r = (r / 99999.0f);
	return min_ + r * (max_ - min_);
}

/// �����������
float CZRandom::nextSign()
{
	return float (1.0 - (nextInt() % 2) * 2.0f);
}
