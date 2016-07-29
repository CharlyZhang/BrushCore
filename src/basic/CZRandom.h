
///  \file CZRandom.h
///  \brief This is the file declare the Class CZRandom.
///
///		The class CZRandom is a pseudorandom number generator, using Mersenne twister.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-25
///  \note

#ifndef _CZRANDOM_H_
#define _CZRANDOM_H_

#include <stdint.h>

class CZRandom
{
public:
	CZRandom(uint32_t s_ = 156248);
	/// �����ӳ�ʼ��
	void initWithSeed(uint32_t s_);
	
	/// ����һ������
	uint32_t nextInt();
	/// ����һ�������ͣ�Ĭ��Ϊ[0,1]��Χ��
	float nextFloat(float min_ = 0.0f, float max_ = 1.0f);
	/// ����������ţ�����ֵΪ1��-1��
	float nextSign();

private:
	/// ���������
	void generateNumbers();

	uint32_t ix;
	uint32_t MT[624];
};

#endif