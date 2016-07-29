
///  \file CZUtil.h
///  \brief This is the file declare a series of utilities.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-11
///  \note

#ifndef _CZUTIL_H_
#define _CZUTIL_H_

#include "CZDefine.h"
#include "basic/CZAffineTransform.h"
#include "basic/CZRect.h"
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <string>
#include <map>

class CZBezierNode;
class CZ3DPoint;

#if CZ_DEBUG_GL
	#define CZCheckGLError()	CZUtil::CZCheckGLError_(__FILE__, __LINE__)
#else
	#define CZCheckGLError()
#endif

class CZUtil
{
public:
	/// ȡ�����Сֵ
	static inline float Max(float a, float b)	{ return a>b ? a:b;}
	static inline float Min(float a, float b)	{ return a<b ? a:b;}
    static inline int32_t Max(int32_t a, int32_t b)	{ return a>b ? a:b;}
    static inline int32_t Min(int32_t a, int32_t b)	{ return a<b ? a:b;}
	/// �淶��
	static inline float Clamp(float min, float max, float value) {return (value < min) ? min : (value > max) ? max : value;}
	/// ȡ�����[0,1]
	static inline float RandomFloat()	{return rand()*1.0f / RAND_MAX;};

	/// ��ͬ��ɫģʽ��ת��
	static void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b);
	static void RGBtoHSV(float r, float g, float b, float &h, float &s, float &v);

	/// ��UUID���ɵ����߸������٣�
	static char* generateUUID();

	/// �ж��Ƿ�֧�������ɫ
	static bool canUseHDTexture();

	/// ��һ��������ɢ�����ڽ�������α�������������
	/// 
	///		������㣨nodes���γ�һ�����α��������ߣ��ٽ����ߴ�ɢ�����ɸ����Ƶ㣨points��
	/// 
	///		/param nodes		- ��������ά���
	///		/param points		- ��ɢ��õ��Ļ��Ƶ�����
	///		/return				- ��ɢ��õ��Ļ��Ƶ���Ŀ
	static unsigned int flattenNodes2Points(const std::vector<CZBezierNode> &nodes, bool closed, std::vector<CZ3DPoint> &points);


	static void CZCheckGLError_(const char *file, int line);

	/// ���Һ���,��[0,1]��[0,1] -CZFreehandTool������
	static float sineCurve(float input);

	/// ��ȡBmp�ļ�
	static bool loadBMP(char* fileName,unsigned char* &buf,long &width,long &height);

	/// ���ƾ���
	static void drawRect(const CZRect &rect, const CZAffineTransform &transform);

	/// ��ȡjson�ļ�����
	std::map<std::string,std::vector<std::string> > & readJsonContent(const char* jsonFile);
};

#endif

