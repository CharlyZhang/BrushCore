
///  \file CZEraserTool.cpp
///  \brief This is the file implements the Class CZEraserTool.
///
///		The CZEraserTool is designed for input of drawing. 
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-29
///  \note

#include "CZEraserTool.h"

CZEraserTool::CZEraserTool(bool supportPressure /*= false*/):CZTool(supportPressure)
{
	eraseMode = true;
}