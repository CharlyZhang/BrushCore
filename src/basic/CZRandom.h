
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
	/// 用种子初始化
	void initWithSeed(uint32_t s_);
	
	/// 产生一个整形
	uint32_t nextInt();
	/// 产生一个浮点型（默认为[0,1]范围）
	float nextFloat(float min_ = 0.0f, float max_ = 1.0f);
	/// 产生随机符号（返回值为1或-1）
	float nextSign();

private:
	/// 产生随机数
	void generateNumbers();

	uint32_t ix;
	uint32_t MT[624];
};

#endif