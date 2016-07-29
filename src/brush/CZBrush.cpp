
///  \file CZBrush.cpp
///  \brief This is the file implements the Class CZBrush.
///
///		The class only describe the params of brushes.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-8
///  \note

#include "CZBrush.h"
#include "../basic/CZImage.h"
#include "../basic/CZRandom.h"
#include "../CZActiveState.h"
#include "../CZUtil.h"
#include "CZBrushPreview.h"

static const char *CZGeneratorKey = "generator";
static const char *CZWeightKey = "weight";
static const char *CZIntensityKey = "intensity";
static const char *CZAngleKey = "angle";
static const char *CZSpacingKey = "spacing";
static const char *CZRotationalScatterKey = "rotationalScatter";
static const char *CZPositionalScatterKey = "positionalScatter";
static const char *CZAngleDynamicsKey = "angleDynamics";
static const char *CZWeightDynamicsKey = "weightDynamics";
static const char *CZIntensityDynamicsKey = "intensityDynamics";
//static const char *CZUUIDKey = "uuid";


CZBrush::CZBrush(CZStampGenerator *gen_ /*= NULL*/)
{	
	uuid = CZUtil::generateUUID();

	generator = NULL;
	strokePreview = NULL;
	stampImage = NULL;

	inStampGeneratingMode = false;
	if(gen_) setGenerator(gen_);
	
	buildProperties();

	myRandomizer = new CZRandom();
}
CZBrush::~CZBrush()
{
	if (generator) { delete generator; generator = NULL;}
	if (strokePreview) { delete strokePreview; strokePreview = NULL;}
	if (stampImage) { delete stampImage; stampImage = NULL;}
	if (uuid)		{ delete [] uuid;	uuid = NULL;}
	delete myRandomizer;
	CZBrushPreview::destroy();
}

/// 设置笔刷的笔触图片
bool CZBrush::setStampImage(CZImage* img)
{
	if (img == NULL)
	{
		LOG_WARN("img is NULL");
		return false;
	}

	inStampGeneratingMode = false;
	if (uuid)	{ delete [] uuid; uuid = CZUtil::generateUUID();}
	if (strokePreview) { delete strokePreview; strokePreview = NULL;}
	if (stampImage) { delete stampImage; }
	
	stampImage = img;

	return true;
}

/// 返回笔刷的笔触图片
CZImage* CZBrush::getStampImage()
{
	if (inStampGeneratingMode)
	{
		if (generator)
		{
			if(stampImage == NULL) stampImage = generator->getStamp()->copy();
		}
		else
		{
			LOG_ERROR("generator is NULL,while brush is in stamp generating mode");
			return NULL;
		}
	}
	
	return stampImage;
}

/// 更改生成器
bool CZBrush::setGenerator(CZStampGenerator *gen_)
{
	if (gen_ == NULL)
	{
		LOG_WARN("generator is NULL");
		return false;
	}

	delete generator;
	generator = gen_;

	inStampGeneratingMode = true;
	generator->ptrDelegate = this;
	generator->configureBrush(this);

	if (uuid)	{ delete [] uuid; uuid = CZUtil::generateUUID();}
	if (strokePreview) { delete strokePreview; strokePreview = NULL;}
	if (stampImage) { delete stampImage; stampImage = NULL;}

	return true;
}
/// 获取生成器
CZStampGenerator* CZBrush::getGenerator() const
{
	return generator;
}

/// 获取相应大小的笔刷图
CZImage *CZBrush::previewImageWithSize(const CZSize &size)
{
	if (strokePreview && size.width==strokePreview->width && size.height==strokePreview->height) 
		return strokePreview;
	
	CZBrushPreview *preview = CZBrushPreview::getInstance();
    
    preview->mainScreenScale = CZActiveState::getInstance()->mainScreenScale;
	preview->setBrush(this);
	strokePreview = preview->previewWithSize(size);

	return strokePreview;
}

/// 恢复默认值
void CZBrush::restoreDefaults()
{
	if (!inStampGeneratingMode) return;
	if(generator) generator->configureBrush(this);
	else	LOG_WARN("generator is NULL\n");
}

/// 运算符重载
bool CZBrush::operator == (const CZBrush &brush_) const
{
	return (strcmp(uuid,brush_.uuid) == 0);
}

CZBrush &CZBrush::operator = (const CZBrush &brush_)
{
	setGenerator(brush_.getGenerator()->copy());

	angle.value = brush_.angle.value;
	weight.value = brush_.weight.value;
	intensity.value =  brush_.intensity.value;
	spacing.value = brush_.spacing.value;
	rotationalScatter.value = brush_.rotationalScatter.value;
	positionalScatter.value = brush_.positionalScatter.value;
	angleDynamics.value = brush_.angleDynamics.value;
	weightDynamics.value = brush_.weightDynamics.value;
	intensityDynamics.value = brush_.intensityDynamics.value;
	strcpy(uuid,brush_.uuid);

	return *this;
}

/// 获取随机数器（根据该轨迹的笔刷的生成器）
CZRandom *CZBrush::getRandomizer()
{
	if(inStampGeneratingMode) return generator->getRandomizer();
	else					  return myRandomizer;
}

/// 是否处于生成器产生笔触纹理模式
bool CZBrush::isInStampGeneratingMode()
{
	return inStampGeneratingMode;
}

/*
/// 笔刷属性有多少组（生成器不存在属性时，只有2组属性）
int CZBrush::numberOfPropertyGroups()
{
	if(inStampGeneratingMode && generator->getProperties().size())	return 3;
	return 2;
}
/// 获取某组属性
CZArray<CZProperty> & CZBrush::propertiesGroupAt(int i)
{
	static CZArray<CZProperty> ret;

	if (inStampGeneratingMode)
	{
		CZArray<CZProperty> &temp = generator->getProperties();

		if(temp.size() == 0)  i++;
	}
	else	i++;
	

	ret.clear();
	if (i == 0) 
	{
		// shape group
		for(CZArray<CZProperty>::iterator itr = temp.begin(); itr != temp.end(); itr++)
			ret.push_back(*itr);
	} 
	else if (i == 1) 
	{
		// spacing group
		ret.push_back(intensity);
		ret.push_back(angle);
		ret.push_back(spacing);
		ret.push_back(rotationalScatter);
		ret.push_back(positionalScatter);
	} 
	else if (i == 2) 
	{
		// dynamic group
		ret.push_back(angleDynamics);
		ret.push_back(weightDynamics);
		ret.push_back(intensityDynamics);
	}
	
	return ret;
}
*/

/// 处理属性变化（实现属性委托接口）

void CZBrush::propertyChanged(CZProperty *property_)
{
	if (strokePreview) { delete strokePreview; strokePreview = NULL;}
	if (stampImage) { delete stampImage; stampImage = NULL;}
	if (uuid)		{ delete [] uuid;	uuid = NULL;}
}

/// 处理生成器变化（实现生成器委托接口）
void CZBrush::generatorChanged(CZStampGenerator *gen_)
{
	if (strokePreview) { delete strokePreview; strokePreview = NULL;}
	if (stampImage) { delete stampImage; stampImage = NULL;}
	if (uuid)		{ delete [] uuid;	uuid = NULL;}
}

/// 实现coding接口
void CZBrush::update(CZDecoder *decoder, bool deep /*= false*/)
{
	if (deep) 
	{
		if(generator) delete generator;
		generator = (CZStampGenerator *) decoder->decodeObject(CZGeneratorKey);
		generator->ptrDelegate = this;
		buildProperties();
	}

	weight.value	= decodeValue(CZWeightKey, decoder, weight.value);
	intensity.value = decodeValue(CZIntensityKey, decoder, intensity.value);
	angle.value		= decodeValue(CZAngleKey, decoder, angle.value);
	spacing.value	= decodeValue(CZSpacingKey, decoder, spacing.value);
	rotationalScatter.value = decodeValue(CZRotationalScatterKey, decoder, rotationalScatter.value);
	positionalScatter.value = decodeValue(CZPositionalScatterKey, decoder, positionalScatter.value);
	angleDynamics.value		= decodeValue(CZAngleDynamicsKey, decoder, angleDynamics.value);
	weightDynamics.value	= decodeValue(CZWeightDynamicsKey, decoder, weightDynamics.value);
	intensityDynamics.value = decodeValue(CZIntensityDynamicsKey, decoder, intensityDynamics.value);
	//uid = decoder->decodeUint(CZUUIDKey);
}
void CZBrush::encode(CZCoder *coder, bool deep/* = false*/)
{
	if (deep) 
	{
		coder->encodeObject(generator, CZGeneratorKey, deep);
	}

	//coder->encodeUint(uuid,CZUUIDKey);
	coder->encodeFloat(weight.value, CZWeightKey);
	coder->encodeFloat(intensity.value, CZIntensityKey);
	coder->encodeFloat(angle.value, CZAngleKey);
	coder->encodeFloat(spacing.value, CZSpacingKey);
	coder->encodeFloat(rotationalScatter.value, CZRotationalScatterKey);
	coder->encodeFloat(positionalScatter.value, CZPositionalScatterKey);
	coder->encodeFloat(angleDynamics.value, CZAngleDynamicsKey);
	coder->encodeFloat(weightDynamics.value, CZWeightDynamicsKey);
	coder->encodeFloat(intensityDynamics.value, CZIntensityDynamicsKey);
}


/// 创建属性(不包括值)
void CZBrush::buildProperties()
{
	weight.title = "Weight";
	weight.conversionFactor = 1;
	weight.minimumValue = 1;
	weight.maximumValue = 512;
	weight.ptrDelegate = this;

	intensity.title = "Intensity";
	intensity.ptrDelegate = this;

	angle.title = "Angle";
	angle.maximumValue = 360;
	angle.conversionFactor = 1;
	angle.ptrDelegate = this;

	spacing.title = "Spacing";
	spacing.minimumValue = 0.004f;
	spacing.maximumValue = 2.0f;
	spacing.percentage = true;
	spacing.ptrDelegate = this;

	rotationalScatter.title = "Jitter";
	rotationalScatter.ptrDelegate = this;
	positionalScatter.title = "Scatter";
	positionalScatter.ptrDelegate = this;

	angleDynamics.title = "Dynamic Angle";
	angleDynamics.minimumValue = -1.0f;
	angleDynamics.ptrDelegate = this;

	weightDynamics.title = "Dynamic Weight";
	weightDynamics.minimumValue = -1.0f;
	weightDynamics.ptrDelegate = this;

	intensityDynamics.title = "Dynamic Intensity";
	intensityDynamics.minimumValue = -1.0f;
	intensityDynamics.ptrDelegate = this;
}

/// 解压缩值
float CZBrush::decodeValue(const char *key, CZDecoder *decoder, float deft)
{
	float value = decoder->decodeFloat(key,123456789);
	if (value == 123456789) 
	{
		// for legacy files
		CZProperty *old = (CZProperty *) decoder->decodeObject(strcmp(key,CZWeightKey)==0 ? "noise": key);
		return old ? old->value : deft;
	} 
	else 
	{
		return value;
	}
}
