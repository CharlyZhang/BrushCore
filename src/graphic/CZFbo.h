
///  \file CZFbo.h
///  \brief This is the file implement the Class CZFbo.
///
///		This file includes FBO. It should be used with CZTexuture.
///		This FBO is detached from CZTexture and glColorRenderBuffer, you can change them by calling
///		the function setTexture and setColorRenderBuffer distinctly.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-23
///  \note	we only consider color render buffer in this FBO, and we don't consider depth buffer.

#ifndef _CZFBO_H_
#define _CZFBO_H_

#include "CZTexture.h"
#include "../basic/CZRect.h"
#include "../basic/CZColor.h"

class CZFbo
{
public:
	enum FboState {NONE=0, RENDER2TEX, OFFLINE_RENDER};
	/// 构造函数
	CZFbo();
	~CZFbo();
	/// 设置绘制纹理（用作绘制到纹理）
	void setTexture(CZTexture *tex_);
	/// 设置绘制缓冲区（用于离线绘制）
	void setColorRenderBuffer(int w_, int h_);
    /// set render buffer with gl context (for ios)
    void setRenderBufferWithContext(void* ctx,void* layer);
	/// 开始FBO（不负责清除缓存）
	void begin();
	/// 结束FBO
	void end();
	/// 将纹理绘制到屏幕
	void showTextureOnScreen(int x,int y,int width_ = 128,int height_ = 128);
    unsigned int getRenderBufferId(){ return renderId;}

	/// 生成当前状态的图像（需要在begin和end之前调用）
	CZImage* produceImageForCurrentState();
	CZImage* produceImageForCurrentState(CZRect &rect);
    CZColor getColor(int x,int y);

public:
	int width,height;

private:
	/// 检查状态
	int checkFramebufferStatus();

	unsigned int fboId;
	int preFbo;
	FboState isReady;			///< FBO状态
	unsigned int renderId;		///< 渲染缓存
	CZTexture *tex;				///< 只是引用，不负责创建和销毁
	StorageMode mode;
};

#endif