//
//  CZViewImpl.h
//  HYDrawing
//
//  cpp function which contains oc variable
//
//  Created by CharlyZhang on 15/8/12.
//  Copyright (c) 2015年 Founder. All rights reserved.
//

#ifndef __HYDrawing__CZViewImpl__
#define __HYDrawing__CZViewImpl__

#include "BrushesCore.h"
#import "CanvasView.h"

/// CZView, ∆ΩÃ®œ‡πÿµƒ ”Õº
class CZViewImpl : public CZView
{
public:
    CanvasView *realView;
    
    CZViewImpl(const CZRect& rect);
    
    ~CZViewImpl();
    void setCanvas(CZCanvas* c);
    void setPaiting(CZPainting* p);
    void drawView();
    
    CZRect& getFrame();
    
private:
    CZRect frame;
};

#endif /* defined(__HYDrawing__CZViewImpl__) */
