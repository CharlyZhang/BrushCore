
///  \file CZFreehandTool.cpp
///  \brief This is the file implements the Class CZFreehandTool.
///
///		The CZFreehandTool is designed for input of drawing. 
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-28
///  \note

#include "CZFreehandTool.h"


CZFreehandTool::CZFreehandTool(bool supportPressure /* = false */):CZTool(supportPressure)
{
	eraseMode = false;
}