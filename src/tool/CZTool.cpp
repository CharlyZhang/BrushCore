
///  \file CZTool.cpp
///  \brief This is the file implements the Class CZTool.
///
///		The CZTool is designed for input, it's the super class of all input classes. 
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-28
///  \note

#include "CZTool.h"
#include "../CZActiveState.h"
#include "../stamp/CZSpiralGenerator.h"		///< for moveEnd() Brush
#include "../basic/CZMat4.h"
#include "../CZDefine.h"

#define kMaxError                   10.0f
#define kSpeedFactor                3.0f
#define kBezierInterpolationSteps   5.0f

using namespace  std;

CZTool::CZTool(bool supportPressure /* = false */)
{
	realPressure = supportPressure;
	scale = 1.0f;
	lastRemainder = 0.0f;
	ptrRandomizer = NULL;

	ptrPainting = NULL;

	eraseMode = false;
}
CZTool::~CZTool()
{
	accumulatedStrokePoints.clear();
}

/// ��ʼ�ƶ�
void CZTool::moveBegin(CZ2DPoint &p_, float pressure_ /* = 0.0f */)
{
    if(!ptrPainting)
    {
        LOG_ERROR("ptrPainting is NULL\n");
        return;
    }
    
	moved = false;

	firstEver = true;

	strokeBounds = CZRect::zeroRect();
	accumulatedStrokePoints.clear();

	CZ2DPoint &location = p_;

	// capture first point
	lastLocation = location;
	float pressure = 1.0f;

#if 0
	/// 
	// see if we've got real pressure
	realPressure = NO;
	if ([recognizer isKindOfClass:[WDPanGestureRecognizer class]]) {
		UITouch *touch = [recognizer.touches anyObject];
		pressure = [[WDStylusManager sharedStylusManager] pressureForTouch:touch realPressue:&realPressure];
	}
#endif

	if (realPressure) pressure = pressure_;

	CZBezierNode node(location,pressure);
	pointsToFit[0] = node;
	pointsIndex = 1;
    
	clearBuffer = true;
}
void CZTool::moveBegin(float x_, float y_, float pressure /* = 0.0f */)
{
    CZ2DPoint p(x_,y_);
    moveBegin(p,pressure);
}

/// �����ƶ�
///		/param p_				  - ��ǰ�ƶ��ĵ�λ��
///		/param pressureOrSpeed	  - ���豸֧��ѹ��ֵʱ��Ϊѹ��ֵ������Ϊ�ƶ��ٶ�ֵ
void CZTool::moving(CZ2DPoint &p_, float pressureOrSpeed)
{
    if(!ptrPainting)
    {
        LOG_ERROR("ptrPainting is NULL\n");
        return;
    }
    
	moved = true;

	CZ2DPoint &location = p_;
	float       distanceMoved = location.distanceTo2DPoint(lastLocation);

	if (distanceMoved < 3.0 / scale) 
	{
		// haven't moved far enough
		return;
	}

	float pressure = 1.0f;

	if(!realPressure)
	{
		// account for view scale
		//pressureOrSpeed /= scale;

		/// convert speed into "pressure"
		pressure = CZUtil::sineCurve(1.0f - CZUtil::Min(kSpeedFactor, pressureOrSpeed) / kSpeedFactor);
		pressure = 1.0f - pressure;
	}
	else
		pressure = pressureOrSpeed;

	if (firstEver)
	{
		pointsToFit[0].inPoint.z = pressure;
		pointsToFit[0].anchorPoint.z = pressure;
		pointsToFit[0].outPoint.z = pressure;
		firstEver = false;
	} 
	else if (pointsIndex != 0)
	{
		// average out the pressures
		pressure = (pressure + pointsToFit[pointsIndex - 1].anchorPoint.z) / 2;
	}

	pointsToFit[pointsIndex++] = CZBezierNode(location,pressure);

	// special case: otherwise the 2nd overall point never gets averaged
	if (pointsIndex == 3)
	{
		// since we just incremented pointsIndex (it was really just 2)
		averagePointsBetween(1,2);     
	}

	if (pointsIndex == 5) 
	{
		paintFittedPoints();
	}

	// save data for the next iteration
	lastLocation = location;
}
void CZTool::moving(float x_, float y_, float pressureOrSpeed)
{
    CZ2DPoint p(x_, y_);
    moving(p, pressureOrSpeed);
}

/// �ƶ�����
void CZTool::moveEnd(CZ2DPoint &p_)
{
    if(!ptrPainting)
    {
        LOG_ERROR("ptrPainting is NULL\n");
        return;
    }
    
	CZColor     color = CZActiveState::getInstance()->getPaintColor();
//	CZBrush     *ptrBrush = CZActiveState::getInstance()->getActiveBrush();
	//CZCanvas    canvas;// = (WDCanvas *) recognizer.view;
	//CZPainting  painting(size);// = canvas.painting;

	CZ2DPoint   &location = p_;
	
	if (!moved) 
	{ // draw a single stamp
        LOG_INFO("draw stamp\n");
		CZBezierNode node(location,1.0);
		path.reset();
		path.addNode(node);
		accumulatedStrokePoints.push_back(node);

		paintPath(path);
	} 
	else
	{
		paintFittedPoints();
	}

	/*
	if (CGRectIntersectsRect(strokeBounds_, painting.bounds)) {
		if (accumulatedStrokePoints_.count > 0) {
			WDPath *accumulatedPath = [[WDPath alloc] init];
			accumulatedPath.nodes = accumulatedStrokePoints_;
			accumulatedPath.color = color;
			accumulatedPath.brush = [brush copy];
			changeDocument(painting, [WDAddPath addPath:accumulatedPath erase:eraseMode layer:painting.activeLayer sourcePainting:painting]);
		}
	}
	*/
	//if (CGRectIntersectsRect(strokeBounds_, painting.bounds)) {
	//	[painting.activeLayer commitStroke:strokeBounds_ color:color erase:eraseMode undoable:YES];
	//}
	ptrPainting->getActiveLayer()->commitStroke(strokeBounds,color,eraseMode,true);

	ptrPainting->setActivePath(NULL);

}
void CZTool::moveEnd(float x_, float y_)
{
    CZ2DPoint p(x_, y_);
    moveEnd(p); 
}

/// ����ʱ��������start_��end_�ĵ����ƽ������
///		
///		���崦����������ǰ��ֱ���ǰ�����������߳�l1��l2,�ٽ�l1��l2���е����߳�l3,
///					ȡl3���е�Ϊ��ǰ���ƽ�������
void CZTool::averagePointsBetween(int start_, int end_)
{
	for (int i = start_; i < end_; i++) 
	{
		CZ3DPoint current = pointsToFit[i].anchorPoint * 0.5;
		CZ3DPoint prev =	pointsToFit[i-1].anchorPoint * 0.25;
		CZ3DPoint next =	pointsToFit[i+1].anchorPoint * 0.25;

		pointsToFit[i].anchorPoint = current + prev + next ;
	}
}

/// ��������ĵ�
///
///		ÿ�ν��ٽ��ļ�������о������ټ�����ڶ����㵽�����ڶ������Ŀ��Ƶ㣻
///		����ǰ3�����������ߣ����л��ƣ�������������������һ�ֻ��ƣ��Ա�ƽ�����ɡ�
void CZTool::paintFittedPoints()
{
	bool    touchEnding = (pointsIndex != 5) ? true : false;	///< �жϻ����Ƿ����
	int     loopBound = touchEnding ? pointsIndex - 1 : 4;		///< �߶���Ŀ
	int     drawBound = touchEnding ? pointsIndex - 1 : 2;		///< ���λ������ĵ�ı��

	averagePointsBetween(2, loopBound);

	/// �������β����������ģ�������Ƶ�ͳ�����Ƶ㡣
	for (int i = 1; i < loopBound; i++) 
	{
		CZ3DPoint current = pointsToFit[i].anchorPoint;
		CZ3DPoint prev =	pointsToFit[i-1].anchorPoint;
		CZ3DPoint next =	pointsToFit[i+1].anchorPoint;

		CZ3DPoint delta = next - prev;
		delta.normalize();

		float inMagnitude = prev.distanceTo3DPoint(current) / 3.0f;
		float outMagnitude = next.distanceTo3DPoint(current) / 3.0f;

		CZ3DPoint in = current - ( delta * inMagnitude);
		CZ3DPoint out = current + (delta * outMagnitude);

		pointsToFit[i].inPoint = in;
		pointsToFit[i].outPoint = out;
	}

	/// ����һ���켣path
	path.reset();

	for (int i = 0; i <= drawBound; i++) 
	{
		path.addNode(pointsToFit[i]);

		if (i == 0 && accumulatedStrokePoints.size())
		{	/// ��ȥ���ֻ������Ľ��
			accumulatedStrokePoints.pop_back(); 
		}
		
		accumulatedStrokePoints.push_back(pointsToFit[i]);
	}

	paintPath(path);

	/// ����û�н�����������û�д������ǰ��
	if (!touchEnding) 
	{
		for (int i = 0; i < 3; i++) 
		{
			pointsToFit[i] = pointsToFit[i+2];
		}
		pointsIndex = 3;
	}
}

void CZTool::paintPath(CZPath &path) 
{
	path.ptrBrush = CZActiveState::getInstance()->getActiveBrush();
	path.color = CZActiveState::getInstance()->getPaintColor();
	path.action = eraseMode ? CZPathActionErase : CZPathActionPaint;

	if (clearBuffer)
	{
		// depends on the path's brush
		ptrRandomizer = path.getRandomizer();
		lastRemainder = 0.f;
	}

	path.remainder = lastRemainder;
	path.setClosed(false);
	
	CZRect pathBounds = ptrPainting->paintStroke(&path,ptrRandomizer,clearBuffer);

    strokeBounds = strokeBounds.unionWith(pathBounds);
	lastRemainder = path.remainder;

	//[canvas drawViewInRect:pathBounds];

	clearBuffer = false;
}