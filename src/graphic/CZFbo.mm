
///  \file CZFbo.cpp
///  \brief This is the file implement the Class CZFbo.
///
///		This file includes FBO. It should be used with CZTexuture.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-23
///  \note
#include "CZFbo.h"
#include "../CZDefine.h"

#if defined(__APPLE__)
# include <QuartzCore/QuartzCore.h>
#endif

CZFbo::CZFbo()
{
    width = 0;
    height = 0;
    renderId = 0;
    tex = NULL;
    isReady = NONE;
    mode = DEFAULT_STORAGEMODE;
    
    glGenFramebuffers(1, &fboId);
}

CZFbo::~CZFbo()
{
    glDeleteFramebuffers(1,&fboId);
    if(renderId) glDeleteRenderbuffers(1,&renderId);
}

/// 设置绘制纹理
void CZFbo::setTexture(CZTexture *tex_)
{
    if (isReady == OFFLINE_RENDER)
    {
        if(renderId) glDeleteRenderbuffers(1,&renderId);
    }
    
    if(tex_)
    {
        tex = tex_;
        width = tex->width;
        height = tex->height;
        mode = tex->getMode();
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->texId, 0);
        //check status
        if (GL_FRAMEBUFFER_COMPLETE == checkFramebufferStatus()) isReady = RENDER2TEX;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
        LOG_ERROR("Tex is null \n");
}

/// 设置绘制缓冲区
void CZFbo::setColorRenderBuffer(int w_, int h_)
{
    if (isReady == OFFLINE_RENDER)
    {
        if(width == w_ && height == h_) return;
        
        if(renderId) glDeleteRenderbuffers(1,&renderId);
    }
    
    width = w_;
    height = h_;
    mode = DEFAULT_STORAGEMODE;
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    //申请绘制缓冲区
    glGenRenderbuffers(1,&renderId);
    glBindRenderbuffer(GL_RENDERBUFFER,renderId);
    
    switch(mode)
    {
        case RGB_BYTE:
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, width, height);
            break;
        case RGBA_BYTE:
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
            break;
        case RGB_FLOAT:
            glRenderbufferStorage(GL_RENDERBUFFER,GL_RGB,width,height);
            break;
        case RGBA_FLOAT:
            glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA,width,height);
            break;
        default:
            LOG_ERROR("illegal imgMode!\n");
    }
    
    //将颜色绘制缓冲与FBO绑定
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,renderId);
    //check status
    if (GL_FRAMEBUFFER_COMPLETE == checkFramebufferStatus()) isReady = OFFLINE_RENDER;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// set render buffer with gl context (for ios)
void CZFbo::setRenderBufferWithContext(void* ctx, void* layer)
{
    if(!renderId)   glGenRenderbuffers(1,&renderId);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    //申请绘制缓冲区
    glBindRenderbuffer(GL_RENDERBUFFER,renderId);
#if defined(__APPLE__)
    [(__bridge EAGLContext*)ctx renderbufferStorage:GL_RENDERBUFFER fromDrawable:(__bridge CAEAGLLayer*)layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
#endif
    //将颜色绘制缓冲与FBO绑定
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,renderId);
    //check status
    if (GL_FRAMEBUFFER_COMPLETE == checkFramebufferStatus()) isReady = OFFLINE_RENDER;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// 开始FBO
void CZFbo::begin()
{
    if(isReady == NONE)
    {
        LOG_ERROR("The FBO is not ready \n");
        return;
    }
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING,&preFbo);//获取当前的FBO，用于还原
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    GL_DRAW_BUF(GL_COLOR_ATTACHMENT0);
    
    GL_PUSH_ATTR(/*GL_ALL_ATTRIB_BITS*/GL_VIEWPORT_BIT);
    glViewport(0,0,width,height);
}

/// 结束FBO
void CZFbo::end()
{
    if(isReady == NONE) return;
    
    GL_POP_ATTR();
    glBindFramebuffer(GL_FRAMEBUFFER, preFbo);
}

/// 将纹理绘制到屏幕
void CZFbo::showTextureOnScreen(int x,int y,int width_ /*= 128*/,int height_ /*= 128*/)
{
    if(tex == NULL || isReady!=RENDER2TEX)
    {
        LOG_ERROR("Tex is NULL \n");
        return;
    }
#if defined(_WIN32)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(x,y,width_,height_);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D,tex->texId);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  0.0f);
    glEnd();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
#endif
}

/// 生成当前状态的图像（需要在begin和end之前调用）
CZImage* CZFbo::produceImageForCurrentState()
{
    CZImage *ret = new CZImage(width,height,mode);
    switch(mode)
    {
        case RGB_BYTE:
            glReadPixels(0, 0, width, height,GL_RGB, GL_UNSIGNED_BYTE,ret->data);
            break;
        case RGBA_BYTE:
            glReadPixels(0, 0, width, height,GL_RGBA, GL_UNSIGNED_BYTE,ret->data);
            break;
        case RGB_FLOAT:
            glReadPixels(0, 0, width, height,GL_RGB, GL_FLOAT,ret->data);
            break;
        case RGBA_FLOAT:
            glReadPixels(0, 0, width, height,GL_RGBA, GL_FLOAT,ret->data);
            break;
        default:
            LOG_ERROR("illegal imgMode!\n");
    }
    
    return ret;
}

CZColor CZFbo::getColor(int x,int y)
{
    CZColor ret;
    unsigned char *bData = NULL;
    float *fData = NULL;
    switch(mode)
    {
        case RGB_BYTE:
            bData = new unsigned char[3];
            glReadPixels(x, y, 1, 1,GL_RGB, GL_UNSIGNED_BYTE,bData);
            ret = CZColor(bData[0] / 255.0f, bData[1] / 255.0f, bData[2] / 255.0f,1.0f);
            break;
        case RGBA_BYTE:
            bData = new unsigned char[4];
            glReadPixels(x, y, 1, 1,GL_RGBA, GL_UNSIGNED_BYTE,bData);
            ret = CZColor(bData[0] / 255.0f, bData[1] / 255.0f, bData[2] / 255.0f,1.0f);
            break;
        case RGB_FLOAT:
            fData = new float[3];
            glReadPixels(x, y, 1, 1,GL_RGB, GL_FLOAT,fData);
            ret = CZColor(fData[0] / 255.0f, fData[1] / 255.0f, fData[2] / 255.0f,1.0f);
            break;
        case RGBA_FLOAT:
            fData = new float[4];
            glReadPixels(x, y, 1, 1,GL_RGBA, GL_FLOAT,fData);
            ret = CZColor(fData[0] / 255.0f, fData[1] / 255.0f, fData[2] / 255.0f,fData[3] / 255.0f);
            break;
        default:
            LOG_ERROR("illegal imgMode!\n");
    }
    
    delete [] bData;
    delete [] fData;
    
    return ret;

}

/// 检查状态
int CZFbo::checkFramebufferStatus()
{
    int status;
    status = (GLenum) glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOG_ERROR("Unsupported framebuffer format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOG_ERROR("Framebuffer incomplete, missing attachment\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOG_ERROR("Framebuffer incomplete, incomplete attachment\n");
            break;
            
#ifdef USE_OPENGL
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            LOG_ERROR("Framebuffer incomplete, attached images must have same dimensions\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            LOG_ERROR("Framebuffer incomplete, attached images must have same format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            LOG_ERROR("Framebuffer incomplete, missing draw buffer\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            LOG_ERROR("Framebuffer incomplete, missing read buffer\n");
            break;
#endif
        default:
            LOG_ERROR("Framebuffer incomplete, unknown error\n");
            break;
    }
    
    return status;
}