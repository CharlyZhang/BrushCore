
///  \file CZBrush.h
///  \brief This is the file declare the Class CZBrush and the Interface CZBrushDelegate.
///
///		The class only describe the params of brushes.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-12
///  \note		

#ifndef _CZBRUSH_H_
#define _CZBRUSH_H_

#include "../CZDefine.h"
#include "../serialization/CZCoding.h"
#include "../basic/CZProperty.h"
#include "../basic/CZSize.h"
#include "../stamp/CZStampGenerator.h"		// for CZGeneratorDelegate
#include <vector>

class CZImage;
class CZRandom;

class CZBrush :public CZCoding, public CZPropertyDelegate, public CZGeneratorDelegate
{
public:
	friend class CZActiveState;				///< 负责管理画刷
	/// 设置笔刷的笔触图片
	bool setStampImage(CZImage* img);
	/// 返回笔刷的笔触图片
	CZImage* getStampImage();
	/// 更改生成器
	bool setGenerator(CZStampGenerator *gen_);
	/// 获取生成器
	CZStampGenerator* getGenerator() const;

	/// 获取相应大小的笔刷图
	CZImage *previewImageWithSize(const CZSize &size);
	
	/// 恢复默认值
	void restoreDefaults();
	/// 运算符重载
	bool operator == (const CZBrush &brush_) const;
	CZBrush & operator = (const CZBrush &brush_);
	
	/// 获取随机数器（根据该轨迹的笔刷的生成器）
	CZRandom *getRandomizer();
	/// 是否处于生成器产生笔触纹理模式
	bool isInStampGeneratingMode();
/*	/// 笔刷属性有多少组（生成器不存在属性时，只有2组属性）
	int numberOfPropertyGroups();
	/// 获取某组属性
	CZArray<CZProperty> & propertiesGroupAt(int i); */
	
	/// 处理属性变化（实现属性委托接口）
	void propertyChanged(CZProperty *property_);
	/// 处理生成器变化（实现生成器委托接口）
	void generatorChanged(CZStampGenerator *gen_);
	/// 实现coding接口
	void update(CZDecoder *decoder, bool deep = false);
	void encode(CZCoder *coder, bool deep = false);

private:
	CZBrush(CZStampGenerator *gen_ = NULL);
	~CZBrush();
	/// 创建属性（不包括值）
	void buildProperties();
	/// 解压缩值
	float decodeValue(const char *key, CZDecoder *decoder, float deft);

public:
	char* uuid;						///< uuid
	CZProperty weight;				///< 画笔的粗细 [1.0, 512.0]	-- pixels	
	CZProperty intensity;			///< 笔刷的浓度 [0.0, 1.0]	-- 纹理的数目
	CZProperty angle;				// [0.0, 360.0];
	CZProperty spacing;				///< 笔刷的密度 [0.01, 2.0]	-- percentage of brush width
	CZProperty rotationalScatter;	// [0.0, 1.0]		"Jitter"
	CZProperty positionalScatter;	// [0.0, 1.0]		"Scatter"

	CZProperty angleDynamics;		// [-1.0, 1.0]
	CZProperty weightDynamics;		///< 目测是画笔滑动时的粗细的缩小值 [-1.0, 1.0]
	CZProperty intensityDynamics;	// [-1.0, 1.0]

private:
	bool inStampGeneratingMode;					///< 是否处于生成器产生笔触纹理模式
	CZRandom *myRandomizer;						///< 当处于自定义笔触模式下时，生成的randomizer
	CZStampGenerator *generator;				///< 笔刷笔触生成器，该类负责销毁
	CZImage *stampImage;						///< 笔刷笔触图片
	CZImage *strokePreview;						///< 笔刷的预览图
};

#endif