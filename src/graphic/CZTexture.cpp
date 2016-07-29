
///  \file CZTexture.cpp
///  \brief This is the file implement the Class CZTexture and other related classes.
///
///		This file includes CZImage, CZTexture.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-19
///  \note

#include "CZTexture.h"
#include "../CZUtil.h"

static bool powerOf2(int x)
{
    return (x & (x - 1)) == 0;
}

/// !WARN: we should confirm the original type of data is suitable for imgMode
CZTexture::CZTexture(int width_, int height_, StorageMode mode_ /* = DEFAULT_STORAGEMODE */, void *data /* = NULL */)
{
    width = width_;
    height = height_;
    
    mode = mode_;
    
    bool emptyData = false;
    if(!data) {
        emptyData = true;
        data = new float[width_*height_*4];
        memset(data, 0, width_*height_*4*sizeof(float));
    }
    
    glGenTextures (1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    
#ifdef USE_OPENGL
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);  // 设置自动生成的最大层数
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
    
    switch(mode)
    {
        case RGB_BYTE:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE,(void*)data);
            break;
        case RGBA_BYTE:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE,(void*)data);
            break;
        case RGB_FLOAT:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0,GL_RGB, GL_FLOAT,(void*)data);
            break;
        case RGBA_FLOAT:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0,GL_RGBA, GL_FLOAT,(void*)data);
            break;
        default:
            LOG_ERROR("illegal imgMode!\n");
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
#elif defined(USE_OPENGL_ES)
    /// painting 里的生成纹理
    // Set up filter and wrap modes for this texture object
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CZCheckGLError();
//    GLenum      type = supportColor ? GL_HALF_FLOAT_OES : GL_UNSIGNED_BYTE;
    //NSUInteger  bytesPerPixel = supportColor ? 8 : 4;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);
    CZCheckGLError();
    
    if(powerOf2(width) && powerOf2(height)) /// in fact, both width and height should be power of two
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); ///! originally GL_LINEAR
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    CZCheckGLError();
    glBindTexture(GL_TEXTURE_2D, 0);
#endif
    
    if(emptyData) delete [] (float*)data;
    CZCheckGLError();
    
}

CZTexture::~CZTexture()
{
    glDeleteTextures(1, &texId);
}

CZTexture* CZTexture::produceFromImage(CZImage *img)
{
    if(img == NULL)
    {
        LOG_ERROR("img is NULL!\n");
        return NULL;
    }
    
    CZTexture *ret = new CZTexture(img->width,img->height,img->getMode(),img->data);
    return ret;
}

/// 用图片数据修改纹理
bool CZTexture::modifyWith(CZImage *img,int x /*= 0*/, int y /*= 0*/, int w /*= -1*/, int h /*= -1*/)
{
    if(img == NULL)
    {
        LOG_ERROR("img is NULL!\n");
        return false;
    }
    
    if (img->getMode() != mode)
    {
        LOG_ERROR("img's mode doesn't match texture mode!\n");
        return false;
    }
    
    if (w == -1)    w = img->width;
    if (h == -1)    h = img->height;
    
    glBindTexture(GL_TEXTURE_2D,texId);
    
    switch(mode)
    {
        case RGB_BYTE:
            glTexSubImage2D(GL_TEXTURE_2D,0,x,y,w,h,GL_RGB,GL_UNSIGNED_BYTE,img->data);
            break;
        case RGBA_BYTE:
            glTexSubImage2D(GL_TEXTURE_2D,0,x,y,w,h,GL_RGBA,GL_UNSIGNED_BYTE,img->data);
            break;
        case RGB_FLOAT:
            glTexSubImage2D(GL_TEXTURE_2D,0,x,y,w,h,GL_RGB,GL_FLOAT,img->data);
            break;
        case RGBA_FLOAT:
            glTexSubImage2D(GL_TEXTURE_2D,0,x,y,w,h,GL_RGBA,GL_FLOAT,img->data);
            break;
        default:
            LOG_ERROR("illegal imgMode!\n");
    }
    
    return true;
}

/// 获取其对应的图像数据
CZImage *CZTexture::toImage()
{
    return NULL;
}

/// 开关线性差值
void CZTexture::enableLinearInterprolation(bool flag)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, flag ? GL_LINEAR : GL_NEAREST);
}
