
///  \file CZFreehandTool.h
///  \brief This is the file declare the Class CZFreehandTool.
///
///		The CZFreehandTool is designed for input of drawing. 
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-28
///  \note It has no consideration of scaling and transfering of the canvas. 

#ifndef _CZFREEHANDTOOL_H_
#define _CZFREEHANDTOOL_H_

#include "CZTool.h"

/// 手绘工具类
class CZFreehandTool : public CZTool
{
public:
	CZFreehandTool(bool supportPressure = false);
};

#endif