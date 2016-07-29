//
//  CZViewImpl.cpp
//  HYDrawing
//
//  Created by CharlyZhang on 15/8/12.
//  Copyright (c) 2015年 Founder. All rights reserved.
//

#include "CZViewImpl.h"

////////////////////CanvasView////////////////////
CZViewImpl::CZViewImpl(const CZRect& rect)
{
    frame = rect;
    realView = [[CanvasView alloc]initWithFrame:CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height)];
}

CZViewImpl::~CZViewImpl()
{
    realView.canvas = nullptr;
    [realView setPainting:nil];
}

void CZViewImpl::setCanvas(CZCanvas* c)
{
    if (c != nullptr)
    {
        ptrCanvas = c;
        realView.canvas = (void*)c;
    }
    else
        LOG_WARN("param 'canvas' is NULL\n");
}
void CZViewImpl::setPaiting(CZPainting* p) { [realView setPainting:(void*)p];}
void CZViewImpl::drawView() { [realView drawView];}

CZRect& CZViewImpl::getFrame()
{
    return frame;
}


