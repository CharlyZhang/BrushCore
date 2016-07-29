
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
	/// 取最大最小值
	static inline float Max(float a, float b)	{ return a>b ? a:b;}
	static inline float Min(float a, float b)	{ return a<b ? a:b;}
    static inline int32_t Max(int32_t a, int32_t b)	{ return a>b ? a:b;}
    static inline int32_t Min(int32_t a, int32_t b)	{ return a<b ? a:b;}
	/// 规范化
	static inline float Clamp(float min, float max, float value) {return (value < min) ? min : (value > max) ? max : value;}
	/// 取随机数[0,1]
	static inline float RandomFloat()	{return rand()*1.0f / RAND_MAX;};

	/// 不同颜色模式的转换
	static void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b);
	static void RGBtoHSV(float r, float g, float b, float &h, float &s, float &v);

	/// 生UUID（由调用者负责销毁）
	static char* generateUUID();

	/// 判断是否支持深度颜色
	static bool canUseHDTexture();

	/// 将一连串结点打散，相邻结点用三次贝塞尔曲线连接
	/// 
	///		两个结点（nodes）形成一根三次贝塞尔曲线，再将曲线打散成若干个绘制点（points）
	/// 
	///		/param nodes		- 连续的三维结点
	///		/param points		- 离散后得到的绘制点容器
	///		/return				- 离散后得到的绘制点数目
	static unsigned int flattenNodes2Points(const std::vector<CZBezierNode> &nodes, bool closed, std::vector<CZ3DPoint> &points);


	static void CZCheckGLError_(const char *file, int line);

	/// 正弦函数,由[0,1]到[0,1] -CZFreehandTool类引用
	static float sineCurve(float input);

	/// 读取Bmp文件
	static bool loadBMP(char* fileName,unsigned char* &buf,long &width,long &height);

	/// 绘制矩形
	static void drawRect(const CZRect &rect, const CZAffineTransform &transform);

	/// 读取json文件内容
	std::map<std::string,std::vector<std::string> > & readJsonContent(const char* jsonFile);
};

#endif

