
///  \file CZCanvas.h
///  \brief This is the file declare the class CZCanvas.
///
///	
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-15
///  \note

#ifndef _CZCANVAS_H_
#define _CZCANVAS_H_

#include "basic/CZRect.h"
#include "basic/CZ2DPoint.h"
#include "basic/CZMat4.h"
#include "basic/CZImage.h"
#include "basic/CZAffineTransform.h"


class CZPainting;
class CZCanvas;
class CZTexture;

/// protocol CZView
class CZView
{
public:
    virtual ~CZView(){  ptrCanvas = nullptr; };
    virtual void setCanvas(CZCanvas* c) = 0;
    virtual void setPaiting(CZPainting* p) = 0;
    virtual void drawView() = 0;
    
    virtual CZRect& getFrame() = 0;
protected:
    CZCanvas *ptrCanvas;
};

class CZCanvas
{
public:
	CZCanvas(CZView *view_);
	~CZCanvas();

	/// 设置绘制
	bool setPaiting(CZPainting* p);
	/// 绘制视图  draw the view
	void drawView();
    
    /// Methods for CanvasView
    void setScale(float s);
    float getScale();
    void setCanvasFrame(const CZRect &frame);
    void rebuildViewTransformAndRedraw(bool flag);
    CZ2DPoint transformToPainting(const CZ2DPoint &pt);
    void pinchBegin(CZ2DPoint &pt);
    void pinchChanged(CZ2DPoint &pt, bool touchCountChanged = false);
    void resetTransform();
    void draw();
    
    /// Methods for photo placement
    void beginPlacePhoto(CZImage *photo, CZAffineTransform &trans);
    void setPhotoTransform(CZAffineTransform &trans);
    int endPlacePhoto(bool renderToLayer = true);
    
    CZMat4 getTransformMatrix();

private:
	/// 在一定区域绘制视图
	void drawViewInRect(CZRect &rect);
    void drawWhiteBackground(CZMat4& proj);
    void renderPhoto(CZMat4 &proj, CZAffineTransform &transform);
    
public:
	CZPainting *ptrPainting;
	bool		isDirty;			///< just used in win

private:
	CZRect visibleRect;
	CZView *view;
    CZRect canvasFrame;                           ///< canvas frame, base on view's frame
    CZMat4 projMat;
    bool photoPlacementMode;
    CZTexture *photoTexture;
    CZAffineTransform photoTransform;
    CZImage *photoImage;                         ///< for rendering to layer
    
    CZ2DPoint                 userSpacePivot;
    CZ2DPoint                 deviceSpacePivot;
    CZ2DPoint                 correction;         ///< used for correcting pivot if touch numbers change when pinching
    float                     canvasScale;        ///< used for displaying
    CZAffineTransform         canvasTransform;    ///< from Painting Space to Device Space
};

#endif