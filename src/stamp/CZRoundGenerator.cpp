
#include "CZRoundGenerator.h"

using namespace std;

CZRoundGenerator::CZRoundGenerator(CZGLContext *ctx):CZStampGenerator(ctx)
{
	/// ��������
	hardness.title = "Hardness";
	hardness.value = 1.0;
	//density.delegate = self;
	//(self.rawProperties)[@"density"] = density;
}

/// ����һ�ݵ�ǰ������
CZRoundGenerator* CZRoundGenerator::copy()
{
	CZRoundGenerator *ret = new CZRoundGenerator(ptrGLContext);

	ret->seed = seed;
	ret->size = size;
	ret->scale = scale;
	ret->hardness = hardness;
	return ret;
}

bool CZRoundGenerator::canRandomize()
{
	return false;
}

/// ����ͼ��
void CZRoundGenerator::renderStamp(CZRandom* randomizer)
{
	drawRadialFade(hardness.value);
}