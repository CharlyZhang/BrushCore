
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

/// ��ˢ��������������
class CZStampGenerator : public CZCoding
{
public:
	CZStampGenerator(CZGLContext *ctx);
	~CZStampGenerator();

	/// ����һ�ݵ�ǰ������
	virtual CZStampGenerator* copy();
	/// �ж��Ƿ���ͬ
	bool isEqual(CZStampGenerator *);

	/// ��������
	void resetSeed();
	/// �Ƿ��������
	virtual bool canRandomize();
	/// �����
	void randomize();
	/// ��ȡ��ˢͼ��
	CZImage *getStamp(bool isSmall = false);
	/// ��ȡ�������
	CZRandom *getRandomizer();
	/// ���ñ�ˢ����
	virtual void configureBrush(CZBrush *brush);
	/// ��������ֵ
	std::vector<CZProperty> & getProperties();
    
    /// test property
    void propertiesChanged() { if(stamp) {  delete stamp; stamp = NULL;} };

	/// ʵ��coding�ӿ�
	void update(CZDecoder *decoder_, bool deep = false){};
	void encode(CZCoder *coder_, bool deep = false){};

protected:
	/// ����ͼ��
	virtual void renderStamp(CZRandom* randomizer);
	/// ���ɱ�ˢͼ��
	CZImage* generateStamp();
	/// 
	CZPath* splat();
	/// 
	CZRect randomRect();

	/// ��ȡ��ɫ��
	CZShader* getShader(std::string name);

	///// helper for subclass
	/// ���ƾ���˥��Ȧ
	void drawRadialFade(float hardness);

public:
	unsigned int seed;
	CZSize size;
	float scale;				///< ���ű�
	float baseDimension;
	CZGeneratorDelegate *ptrDelegate;
	unsigned char blurRadius;

protected:
	CZImage *stamp;				///< ��ˢͼ��
	CZImage *smallStamp;
	CZRandom *randomizer;		///< ��seed��ص��������
	CZGLContext *ptrGLContext;	///< gl������
	std::map<std::string,CZProperty*> rawProperties;
	std::map<std::string,CZShader*>	shaders;	///< ��ɫ��
	CZMat4 projMat;
};

class CZGeneratorDelegate
{
public:
	virtual void generatorChanged(CZStampGenerator *gen_) = 0;
};

#endif