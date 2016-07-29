
///  \file CZTool.h
///  \brief This is the file declare the Class CZTool.
///
///		The CZTool is designed for input, it's the super class of all input classes. 
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-28
///  \note

#ifndef _CZTOOL_H_
#define _CZTOOL_H_

#include "../path/CZBezierNode.h"
#include "../basic/CZ2DPoint.h"
#include "../basic/CZRect.h"
#include "../basic/CZSize.h"
#include "../path/CZPath.h"
#include "../basic/CZRandom.h"
#include "../painting/CZPainting.h"
#include "../brush/CZBrush.h"
#include "../CZUtil.h"
#include <vector>

/// 工具类
class CZTool 
{
public:
	CZTool(bool supportPressure = false);
	~CZTool();

	/// 开始移动（如果支持压力，则参数为压力值）
	void moveBegin(CZ2DPoint &p_, float pressure_ = 0.0f);
    void moveBegin(float x_, float y_, float pressure_ = 0.0f);

	/// 正在移动
	///		
	///		将连续5个点处理，每处理一次绘制一遍。
	///
	///		/param p_				  - 当前移动的点位置
	///		/param pressureOrSpeed	  - 当设备支持压力值时，为压力值；否则为移动速度值(pixels/millisecond)
	void moving(CZ2DPoint &p_, float pressureOrSpeed);
    void moving(float x_, float y_, float pressureOrSpeed);
    
	/// 移动结束
	void moveEnd(CZ2DPoint &p_);
    void moveEnd(float x_, float y_);

protected:
	/// 对临时连续点中start_到end_的点进行平均处理
	///		
	///		具体处理方法：将当前点分别与前后两个点连线成l1和l2,再将l1和l2的中点连线成l3,
	///					取l3的中点为当前点的平均处理点
	void averagePointsBetween(int start_, int end_);

	/// 绘制适配的点
	///
	///		每次将临近的几个点进行均化，再计算出第二个点到倒数第二个结点的控制点；
	///		利用前3个点生成曲线，进行绘制；后面两个点则留到下一轮绘制，以便平滑过渡。
	void paintFittedPoints();

	void paintPath(CZPath &path);

public:
	float						scale;							///< 屏幕的缩放比
	CZPainting					*ptrPainting;					///< 指向绘制  keep the same as canvas's painting
	bool						eraseMode;						///< 是否是擦除状态，默认为false

protected:
	std::vector<CZBezierNode>	accumulatedStrokePoints;		///< 累积的笔画点数
	bool						firstEver;						///< 是否是某轨迹的首次移动
	CZRect						strokeBounds;					///< 笔画轨迹的范围
	CZ2DPoint					lastLocation;					///< 笔画轨迹的最后一个点
	bool						realPressure;					///< 是否支持真的压力
	CZBezierNode				pointsToFit[5];					///< 处理的临近五个点
	int							pointsIndex;					///< 处理的点标号
	bool						clearBuffer;					///< 是否要清理缓存
	float						lastRemainder;					///< 上轮绘制的多占空间
	CZRandom					*ptrRandomizer;					///< 指向轨迹的随机器
	bool						moved;							///< 是否在移动
    CZPath                      path;
};


#endif
