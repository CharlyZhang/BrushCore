
///  \file CZStampGenerator.h
///  \brief This is the file declare the Class CZStampGenerator and the Interface CZGeneratorDelegate.
///
///		This is a virtual class.
///		It is the super class of all that generating the stamp image of brush stamp.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-19
///  \note

#ifndef _CZSTAMPGENERATOR_H_
#define _CZSTAMPGENERATOR_H_

#include "../CZDefine.h"		
#include "../serialization/CZCoding.h"
#include "../basic/CZProperty.h"
#include "../basic/CZRandom.h"
#include "../basic/CZRect.h"
#include "../basic/CZSize.h"
#include "../graphic/CZFbo.h"
#include "../graphic/CZShader.h"
#include "../basic/CZMat4.h"
#include <vector>
#include <map>
#include <string>

class CZGeneratorDelegate;
class CZBrush;
class CZPath;
class CZImage;
class CZGLContext;

/// 画刷纹理生成器基类
class CZStampGenerator : public CZCoding
{
public:
	CZStampGenerator(CZGLContext *ctx);
	~CZStampGenerator();

	/// 拷贝一份当前生成器
	virtual CZStampGenerator* copy();
	/// 判断是否相同
	bool isEqual(CZStampGenerator *);

	/// 重置种子
	void resetSeed();
	/// 是否能随机化
	virtual bool canRandomize();
	/// 随机化
	void randomize();
	/// 获取笔刷图案
	CZImage *getStamp(bool isSmall = false);
	/// 获取随机化器
	CZRandom *getRandomizer();
	/// 配置笔刷参数
	virtual void configureBrush(CZBrush *brush);
	/// 返回属性值
	std::vector<CZProperty> & getProperties();
    
    /// test property
    void propertiesChanged() { if(stamp) {  delete stamp; stamp = NULL;} };

	/// 实现coding接口
	void update(CZDecoder *decoder_, bool deep = false){};
	void encode(CZCoder *coder_, bool deep = false){};

protected:
	/// 绘制图案
	virtual void renderStamp(CZRandom* randomizer);
	/// 生成笔刷图案
	CZImage* generateStamp();
	/// 
	CZPath* splat();
	/// 
	CZRect randomRect();

	/// 获取着色器
	CZShader* getShader(std::string name);

	///// helper for subclass
	/// 绘制径向衰变圈
	void drawRadialFade(float hardness);

public:
	unsigned int seed;
	CZSize size;
	float scale;				///< 缩放比
	float baseDimension;
	CZGeneratorDelegate *ptrDelegate;
	unsigned char blurRadius;

protected:
	CZImage *stamp;				///< 笔刷图案
	CZImage *smallStamp;
	CZRandom *randomizer;		///< 与seed相关的随机化器
	CZGLContext *ptrGLContext;	///< gl上下文
	std::map<std::string,CZProperty*> rawProperties;
	std::map<std::string,CZShader*>	shaders;	///< 着色器
	CZMat4 projMat;
};

class CZGeneratorDelegate
{
public:
	virtual void generatorChanged(CZStampGenerator *gen_) = 0;
};

#endif