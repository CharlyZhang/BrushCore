
///  \file CPath.h
///  \brief This is the file declare the Class CPath.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2015-03-26
///  \note		

#ifndef _CZPATH_H_
#define _CZPATH_H_

#include "../serialization/CZCoding.h"
#include "../CZDefine.h"
#include "../basic/CZColor.h"
#include "../basic/CZRect.h"
#include "../basic/CZ2DPoint.h"
#include "../basic/CZ3DPoint.h"
#include "CZBezierNode.h"

#include <vector>

class CZRandom;
class CZBrush;

/// 定义CZPathAction
typedef enum 
{
    CZPathActionPaint,
    CZPathActionErase
} CZPathAction;

/// 定义CZPath
class CZPath : public CZCoding
{
public:
	CZPath(std::vector<CZBezierNode> *nodes_ = NULL);
	~CZPath();
	/// 重置轨迹
	void reset();
	/// 设置所有结点
	void setNodes(const std::vector<CZBezierNode> &nodes_);
	/// 获得所有控制点
	const std::vector<CZBezierNode>& getNodes() const;
	/// 添加结点
	void addNode(CZBezierNode &node);
	/// 设置是否封闭
	void setClosed(bool isClosed);
	bool isClosed() const;
	/// 绘制轨迹（返回所有数据的范围）
	CZRect paint(CZRandom *randomizer_);
	/// 获取随机数器（根据该轨迹的笔刷的生成器）
	CZRandom *getRandomizer();
	/// 拷贝构造函数
	CZPath& operator=(const CZPath &path_) ;
	/// 实现coding 接口
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);
private:
	/// 绘制数据
	/// 
	///		以最小粒度的离散点(points_)为中心，形成小矩形，委托render绘制，并将此矩形数据返回。
	///
	CZRect drawData();
	
	/// 绘制一个stamp点
	void paintStamp(CZRandom *randomizer);

	/// 绘制两点之间的线.
	///
	///		将两绘制点(linePoints)之间的线离散成更小粒度的离散点(points)，
	///		更新points,sizes,alphas,angles等向量，
	///		这里利用到了画刷的参数。
	///
	///		/param lastLocation - 轨迹最后离散点的位置
	///		/param location		- 当前绘制点的位置
	///		/param randomizer	- 随机器
	///		/note	利用画笔参数生成部分的算法没看懂
	void paintBetweenPoints(const CZ3DPoint &lastLocation, const CZ3DPoint &location, CZRandom *randomizer);
	
	/// 将一连串结点打散，相邻结点用三次贝塞尔曲线连接
	/// 
	///		两个结点（nodes）形成一根三次贝塞尔曲线，再将曲线打散成若干个绘制点（points）
	/// 
	///		/param nodes		- 连续的三维结点
	///		/param closed		- 是否形成闭环
	///		/param points		- 离散后得到的绘制点容器
	///		/return				- 离散后得到的绘制点数目
	unsigned int flattenNodes2Points(const std::vector<CZBezierNode> &nodes, bool closed,std::vector<CZ3DPoint> &points);

public:
	bool							limitBrushSize;
	float							remainder;			///< 绘制轨迹最后离散点后多占用的线空间
	CZBrush							*ptrBrush;			///< 只是引用，不负责建立和销毁
	CZPathAction					action;				///< 标记轨迹的动作是绘制还是擦除
	CZColor							color;

private:
	float							scale;
	bool							closed;

	std::vector<CZBezierNode>			nodes;				///< 贝塞尔曲线所有控制点
	/// 最终绘制点的参数
	std::vector<CZ2DPoint>				points;
	std::vector<float>					sizes;
	std::vector<float>					angles;
	std::vector<float>					alphas;				
};

#endif

