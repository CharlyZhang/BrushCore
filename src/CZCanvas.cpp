
///  \file CZCanvas.cpp
///  \brief This is the file implements the class CZCanvas.
///
///	
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-15
///  \note

#include "CZCanvas.h"
#include "CZDefine.h"
#include "painting/CZPainting.h"
#include "basic/CZMat4.h"
#include "CZActiveState.h"
#include "CZUtil.h"
#include "graphic/CZTexture.h"

/// implemention of CZCanvas
CZCanvas::CZCanvas(CZView *view_)
{
    ptrPainting = NULL;
    if (!view_) {
        LOG_ERROR("view is NULL\n");
    }
	view = view_;
    view->setCanvas(this);
    
    canvasTransform = CZAffineTransform::makeIdentity();
    canvasScale = 1.f;
    
    setCanvasFrame(view->getFrame());
    deviceSpacePivot = CZ2DPoint(canvasFrame.size.width / 2.f, canvasFrame.size.height / 2.f);
    
    photoPlacementMode = false;
    photoTexture = nullptr;
    photoImage = nullptr;
}

CZCanvas::~CZCanvas()
{
	delete view;
    if(photoTexture)    { delete photoTexture;  photoTexture = nullptr;}
}

/// set painting
bool CZCanvas::setPaiting(CZPainting *p)
{
    if (!p) {
        LOG_ERROR("painting is NULL\n");
        return false;
    }
    
    ptrPainting = p;
    p->setCanvas(this);
    CZActiveState::getInstance()->setPainting(p);
    view->setPaiting(p);
    
    CZSize size = ptrPainting->getDimensions();
    userSpacePivot = CZ2DPoint(size.width / 2.0f, size.height / 2.0f);
    
    rebuildViewTransformAndRedraw(false);
    
    return true;
}

/// 绘制视图
void CZCanvas::drawView()
{
//    if (!ptrPainting) {
//        LOG_ERROR("painting is NULL\n");
//        return;
//    }

    view->drawView();
	//drawViewInRect(visibleRect);
}


/// Methods for CanvasView
void CZCanvas::setScale(float s)
{
    canvasScale = s;
}

float CZCanvas::getScale()
{
    return canvasScale;
}

void CZCanvas::setCanvasFrame(const CZRect &frame)
{
    canvasFrame = frame;
    CZSize size = frame.size;
    projMat.SetOrtho(0,size.width, 0, size.height, -1.0f, 1.0f);
}

void CZCanvas::rebuildViewTransformAndRedraw(bool flag)
{
    float screenScale = CZActiveState::getInstance()->mainScreenScale;
    
    canvasTransform = CZAffineTransform::makeIdentity();
    
    canvasTransform.translate(deviceSpacePivot.x, deviceSpacePivot.y);
    canvasTransform.scale(canvasScale / screenScale, -canvasScale / screenScale);
    canvasTransform.translate(-userSpacePivot.x, -userSpacePivot.y);
    
    canvasTransform.tx = roundf(canvasTransform.tx);
    canvasTransform.ty = roundf(canvasTransform.ty);
    
    if(flag)    view->drawView();
}

CZ2DPoint CZCanvas::transformToPainting(const CZ2DPoint &pt)
{
//    CZAffineTransform iTx = canvasTransform.getInvert();

    CZMat4 mat;
    mat.LoadFromAffineTransform(canvasTransform);
    mat.Invert();
    VECTOR3D p = mat * VECTOR3D(pt.x,pt.y,0.f);
    VECTOR3D po = mat * p;
    CZ2DPoint ret(p.x,p.y);
    return ret;
}

void CZCanvas::pinchBegin(CZ2DPoint &pt)
{
    deviceSpacePivot = pt;
    userSpacePivot = transformToPainting(pt);
    
    correction = CZ2DPoint(0.f,0.f);
}

void CZCanvas::pinchChanged(CZ2DPoint &pt,  bool touchCountChanged /* = false */)
{
    if (touchCountChanged)
    {
        correction = CZ2DPoint(deviceSpacePivot.x - pt.x, deviceSpacePivot.y - pt.y);
    }
    
    deviceSpacePivot = CZ2DPoint(pt.x + correction.x, pt.y + correction.y);
}

void CZCanvas::resetTransform()
{
    if (!ptrPainting)
    {
        LOG_ERROR("ptrPainting is NULL\n");
        return;
    }
    
    deviceSpacePivot = CZ2DPoint(canvasFrame.size.width / 2.f, canvasFrame.size.height / 2.f);

    CZSize size = ptrPainting->getDimensions();
    userSpacePivot = CZ2DPoint(size.width / 2.0f, size.height / 2.0f);
    
    rebuildViewTransformAndRedraw(true);
}

// 
void CZCanvas::draw()
{
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT );
    
    CZMat4 effectiveProj = getTransformMatrix();
    CZMat4 finalMat = projMat * effectiveProj;
    
    drawWhiteBackground(finalMat);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    ptrPainting->blit(finalMat);
    
    if (photoPlacementMode)
    {
        renderPhoto(finalMat, photoTransform);
    }
}

CZMat4 CZCanvas::getTransformMatrix()
{
    CZMat4 effectiveProj;
    effectiveProj.LoadFromAffineTransform(canvasTransform);
    return effectiveProj;
}

void CZCanvas::beginPlacePhoto(CZImage *photo, CZAffineTransform &trans)
{
    photoPlacementMode = true;
    photoTransform = trans;
    photoImage = photo;
    photoTexture = CZTexture::produceFromImage(photo);
    
    drawView();
}

void CZCanvas::setPhotoTransform(CZAffineTransform &trans)
{
    photoTransform = trans;
    drawView();
}

int CZCanvas::endPlacePhoto(bool renderToLayer /* = true */)
{
    int ret = ptrPainting->getActiveLayerIndex();
    
    if (renderToLayer)
    {
        ret = ptrPainting->addNewLayer();
        if (ret < 0)
        {
            photoPlacementMode = false;
            delete photoTexture;
            photoTexture = nullptr;
            delete photoImage;
            photoImage = nullptr;
            return ret;
        }
        
        ptrPainting->getActiveLayer()->renderImage(photoImage, photoTransform);
    }
    
    photoPlacementMode = false;
    delete photoTexture;
    photoTexture = nullptr;
    delete photoImage;
    photoImage = nullptr;
    drawView();
    
    return ret;
}

/////////////////////////////////////////////////////////////

/// 在一定区域绘制视图
void CZCanvas::drawViewInRect(CZRect &rect)
{
    
}

void CZCanvas::drawWhiteBackground(CZMat4& proj)
{
    if (!ptrPainting) {
        LOG_ERROR("ptrPainting is NULL!]\n");
        return ;
    }
    
    CZShader *shader = ptrPainting->getShader("simple");
    
    shader->begin();
    
    glUniformMatrix4fv(shader->getUniformLocation("mvpMat"), 1, GL_FALSE, proj);
    glUniform4f(shader->getUniformLocation("color"), 1, 1, 1, 1);
    
    GL_BIND_VERTEXARRAY(ptrPainting->getQuadVAO());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    GL_BIND_VERTEXARRAY(0);
}

void CZCanvas::renderPhoto(CZMat4 &proj, CZAffineTransform &transform)
{
    CZShader *blitShader = ptrPainting->getShader("nonPremultipliedBlit");
    
    blitShader->begin();
    
    glUniformMatrix4fv(blitShader->getUniformLocation("mvpMat"), 1, GL_FALSE, proj);
    glUniform1i(blitShader->getUniformLocation("texture"), 0);
    glUniform1f(blitShader->getUniformLocation("opacity"), 1.0f);
    
    glActiveTexture(GL_TEXTURE0);
    // Bind the texture to be used
    glBindTexture(GL_TEXTURE_2D, photoTexture->texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    CZRect rect(0, 0, photoTexture->width, photoTexture->height);
    CZUtil::drawRect(rect, transform);
    CZCheckGLError();
}
