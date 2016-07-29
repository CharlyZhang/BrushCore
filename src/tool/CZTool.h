
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

/// ������
class CZTool 
{
public:
	CZTool(bool supportPressure = false);
	~CZTool();

	/// ��ʼ�ƶ������֧��ѹ���������Ϊѹ��ֵ��
	void moveBegin(CZ2DPoint &p_, float pressure_ = 0.0f);
    void moveBegin(float x_, float y_, float pressure_ = 0.0f);

	/// �����ƶ�
	///		
	///		������5���㴦��ÿ����һ�λ���һ�顣
	///
	///		/param p_				  - ��ǰ�ƶ��ĵ�λ��
	///		/param pressureOrSpeed	  - ���豸֧��ѹ��ֵʱ��Ϊѹ��ֵ������Ϊ�ƶ��ٶ�ֵ(pixels/millisecond)
	void moving(CZ2DPoint &p_, float pressureOrSpeed);
    void moving(float x_, float y_, float pressureOrSpeed);
    
	/// �ƶ�����
	void moveEnd(CZ2DPoint &p_);
    void moveEnd(float x_, float y_);

protected:
	/// ����ʱ��������start_��end_�ĵ����ƽ������
	///		
	///		���崦����������ǰ��ֱ���ǰ�����������߳�l1��l2,�ٽ�l1��l2���е����߳�l3,
	///					ȡl3���е�Ϊ��ǰ���ƽ�������
	void averagePointsBetween(int start_, int end_);

	/// ��������ĵ�
	///
	///		ÿ�ν��ٽ��ļ�������о������ټ�����ڶ����㵽�����ڶ������Ŀ��Ƶ㣻
	///		����ǰ3�����������ߣ����л��ƣ�������������������һ�ֻ��ƣ��Ա�ƽ�����ɡ�
	void paintFittedPoints();

	void paintPath(CZPath &path);

public:
	float						scale;							///< ��Ļ�����ű�
	CZPainting					*ptrPainting;					///< ָ�����  keep the same as canvas's painting
	bool						eraseMode;						///< �Ƿ��ǲ���״̬��Ĭ��Ϊfalse

protected:
	std::vector<CZBezierNode>	accumulatedStrokePoints;		///< �ۻ��ıʻ�����
	bool						firstEver;						///< �Ƿ���ĳ�켣���״��ƶ�
	CZRect						strokeBounds;					///< �ʻ��켣�ķ�Χ
	CZ2DPoint					lastLocation;					///< �ʻ��켣�����һ����
	bool						realPressure;					///< �Ƿ�֧�����ѹ��
	CZBezierNode				pointsToFit[5];					///< ������ٽ������
	int							pointsIndex;					///< ����ĵ���
	bool						clearBuffer;					///< �Ƿ�Ҫ������
	float						lastRemainder;					///< ���ֻ��ƵĶ�ռ�ռ�
	CZRandom					*ptrRandomizer;					///< ָ��켣�������
	bool						moved;							///< �Ƿ����ƶ�
    CZPath                      path;
};


#endif
