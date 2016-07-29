
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
	/// ���캯��
	CZFbo();
	~CZFbo();
	/// ���û��������������Ƶ�����
	void setTexture(CZTexture *tex_);
	/// ���û��ƻ��������������߻��ƣ�
	void setColorRenderBuffer(int w_, int h_);
    /// set render buffer with gl context (for ios)
    void setRenderBufferWithContext(void* ctx,void* layer);
	/// ��ʼFBO��������������棩
	void begin();
	/// ����FBO
	void end();
	/// ��������Ƶ���Ļ
	void showTextureOnScreen(int x,int y,int width_ = 128,int height_ = 128);
    unsigned int getRenderBufferId(){ return renderId;}

	/// ���ɵ�ǰ״̬��ͼ����Ҫ��begin��end֮ǰ���ã�
	CZImage* produceImageForCurrentState();
	CZImage* produceImageForCurrentState(CZRect &rect);
    CZColor getColor(int x,int y);

public:
	int width,height;

private:
	/// ���״̬
	int checkFramebufferStatus();

	unsigned int fboId;
	int preFbo;
	FboState isReady;			///< FBO״̬
	unsigned int renderId;		///< ��Ⱦ����
	CZTexture *tex;				///< ֻ�����ã������𴴽�������
	StorageMode mode;
};

#endif