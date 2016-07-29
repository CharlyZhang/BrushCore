
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
	friend class CZActiveState;				///< �������ˢ
	/// ���ñ�ˢ�ıʴ�ͼƬ
	bool setStampImage(CZImage* img);
	/// ���ر�ˢ�ıʴ�ͼƬ
	CZImage* getStampImage();
	/// ����������
	bool setGenerator(CZStampGenerator *gen_);
	/// ��ȡ������
	CZStampGenerator* getGenerator() const;

	/// ��ȡ��Ӧ��С�ı�ˢͼ
	CZImage *previewImageWithSize(const CZSize &size);
	
	/// �ָ�Ĭ��ֵ
	void restoreDefaults();
	/// ���������
	bool operator == (const CZBrush &brush_) const;
	CZBrush & operator = (const CZBrush &brush_);
	
	/// ��ȡ������������ݸù켣�ı�ˢ����������
	CZRandom *getRandomizer();
	/// �Ƿ��������������ʴ�����ģʽ
	bool isInStampGeneratingMode();
/*	/// ��ˢ�����ж����飨����������������ʱ��ֻ��2�����ԣ�
	int numberOfPropertyGroups();
	/// ��ȡĳ������
	CZArray<CZProperty> & propertiesGroupAt(int i); */
	
	/// �������Ա仯��ʵ������ί�нӿڣ�
	void propertyChanged(CZProperty *property_);
	/// �����������仯��ʵ��������ί�нӿڣ�
	void generatorChanged(CZStampGenerator *gen_);
	/// ʵ��coding�ӿ�
	void update(CZDecoder *decoder, bool deep = false);
	void encode(CZCoder *coder, bool deep = false);

private:
	CZBrush(CZStampGenerator *gen_ = NULL);
	~CZBrush();
	/// �������ԣ�������ֵ��
	void buildProperties();
	/// ��ѹ��ֵ
	float decodeValue(const char *key, CZDecoder *decoder, float deft);

public:
	char* uuid;						///< uuid
	CZProperty weight;				///< ���ʵĴ�ϸ [1.0, 512.0]	-- pixels	
	CZProperty intensity;			///< ��ˢ��Ũ�� [0.0, 1.0]	-- �������Ŀ
	CZProperty angle;				// [0.0, 360.0];
	CZProperty spacing;				///< ��ˢ���ܶ� [0.01, 2.0]	-- percentage of brush width
	CZProperty rotationalScatter;	// [0.0, 1.0]		"Jitter"
	CZProperty positionalScatter;	// [0.0, 1.0]		"Scatter"

	CZProperty angleDynamics;		// [-1.0, 1.0]
	CZProperty weightDynamics;		///< Ŀ���ǻ��ʻ���ʱ�Ĵ�ϸ����Сֵ [-1.0, 1.0]
	CZProperty intensityDynamics;	// [-1.0, 1.0]

private:
	bool inStampGeneratingMode;					///< �Ƿ��������������ʴ�����ģʽ
	CZRandom *myRandomizer;						///< �������Զ���ʴ�ģʽ��ʱ�����ɵ�randomizer
	CZStampGenerator *generator;				///< ��ˢ�ʴ������������ฺ������
	CZImage *stampImage;						///< ��ˢ�ʴ�ͼƬ
	CZImage *strokePreview;						///< ��ˢ��Ԥ��ͼ
};

#endif