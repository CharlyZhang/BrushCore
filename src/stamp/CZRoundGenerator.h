
///  \file CZSpiralGenerator.h
///  \brief This is the file declare the Class CZSpiralGenerator.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-19
///  \note

#ifndef _CZROUNDGENERATOR_H_
#define _CZROUNDGENERATOR_H_

#include "CZStampGenerator.h"

class CZRoundGenerator: public CZStampGenerator
{
public:
	CZRoundGenerator(CZGLContext *ctx);
	~CZRoundGenerator(){};

	bool canRandomize();

	/// ����һ�ݵ�ǰ������
	CZRoundGenerator* copy();

	/// ����ͼ��
	void renderStamp(CZRandom* randomizer);

public:
	CZProperty hardness;
};

#endif
