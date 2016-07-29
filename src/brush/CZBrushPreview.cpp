
///  \file CZBrushPreview.cpp
///  \brief This is the file mlass CZBrushPreview Preview.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-16
///  \note

#include "CZBrushPreview.h"
#include "../CZDefine.h"
#include "../path/CZBezierNode.h"
#include "../path/CZPath.h"
#include "../basic/CZ2DPoint.h"
#include "../stamp/CZStampGenerator.h"
#include "../CZUtil.h"
#include "CZBrush.h"

#include <vector>
#include <string>
#include <cmath>

using namespace std;

CZBrushPreview *CZBrushPreview::ptrInstance = NULL;

/// 初始化函数
CZBrushPreview::CZBrushPreview()
{
    path = NULL;
    ptrBrush = NULL;
    brushTexture = NULL;
    backingWidth = backingHeight = 0.0f;
    mainScreenScale = 1.0f;
    
    glContext = new CZGLContext;
    glContext->setAsCurrent();
    /// set up shader
    vector<string> tmp1,tmp2;
    tmp1.push_back("inPosition");
    tmp1.push_back("inTexcoord");
    tmp1.push_back("alpha");
    tmp2.push_back("mvpMat");
    tmp2.push_back("texture");
    brushShader = new CZShader("brush","brush",tmp1, tmp2);
    /// set up fbo
    fbo = new CZFbo;
    
    /// configure some default GL state
    glDisable(GL_DITHER);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

/// 销毁函数
CZBrushPreview::~CZBrushPreview()
{
    if(path)			{	delete path; path = NULL; }
    
    glContext->setAsCurrent();
    if (brushShader)	{	delete brushShader; brushShader = NULL; }
    if (brushTexture)	{	delete brushTexture;brushTexture = NULL;}
    if (fbo)			{	delete fbo;			fbo = NULL;			}
    
    delete glContext;
}

/// 启动新预览图（配置绘制器，生成绘制的轨迹）
void CZBrushPreview::setup(const CZSize &size_)
{
    if (backingWidth == size_.width && backingHeight == size_.height) return;
    
    backingWidth = size_.width;
    backingHeight = size_.height;
    
    /// 创建路径
    if(path) delete path;
    
    buildPath();
    
    /// 设置投影环境
    projMat.SetOrtho(0.0f, (float)backingWidth, 0.0f, (float)backingHeight, -1.0f, 1.0f);
    CZMat4 scaleMat;
    scaleMat.SetScale(VECTOR3D(mainScreenScale,mainScreenScale,1.0f));
    projMat = projMat * scaleMat;
    
    fbo->setColorRenderBuffer(backingWidth,backingHeight);
}

/// 构建轨迹（绘制一条sin曲线）
void CZBrushPreview::buildPath()
{
    if (path) 	return;
    path = new CZPath;
    path->limitBrushSize = true;
    
    float scale = mainScreenScale;
    
    // build a nice little sin curve
    {
        CZ2DPoint start( 30, backingHeight / (2.0f * scale) );
        float   width = (backingWidth / scale) - 2.0f * 30;
        float amplitude  = 10.0f;			///< 振幅
        
        float kNumLineSegments = 100;
        for (int i = 0; i < kNumLineSegments; i++)
        {
            float fraction = (float)i / (kNumLineSegments - 1);
            CZ2DPoint pt(start.x + width * fraction, start.y + sin(fraction * 2 * M_PI) * amplitude );
            CZBezierNode node(CZ3DPoint(pt.x, pt.y, fraction));
            path->addNode(node);
        }
    }
}

/// 配置笔刷
void CZBrushPreview::configureBrush()
{
    if(brushTexture == NULL)
    {
        /// 获取笔刷图像
        CZStampGenerator *gen = ptrBrush->getGenerator();
        CZImage *img = gen->getStamp(true);		///< get the small stamp;
        glContext->setAsCurrent();
        brushTexture = CZTexture::produceFromImage(img);
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,brushTexture->texId);
}

/// 生成指定尺寸大小预览图
CZImage* CZBrushPreview::previewWithSize(const CZSize size_)
{
	if(ptrBrush == NULL)
	{
		LOG_ERROR("ptrBrush is NULL\n");
		return NULL;
	}

	/// 根据设备分辨率进行调整
	CZSize size = size_ * mainScreenScale;

	glContext->setAsCurrent();
	/// 生成新轨迹
	setup(size);
	/// 配置笔刷
	configureBrush();

	/// 设置轨迹参数
	path->ptrBrush = ptrBrush;
	path->remainder = 0.0f;
	path->setClosed(false);

	fbo->begin();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		///!!!
	glClear(GL_COLOR_BUFFER_BIT);

	brushShader->begin();

	glUniform1i(brushShader->getUniformLocation("texture"),0);
	glUniformMatrix4fv(brushShader->getUniformLocation("mvpMat"),1,GL_FALSE,projMat);
	CZCheckGLError();

	/// 绘制轨迹
	path->paint(path->getRandomizer());

	brushShader->end();

    CZImage *ret = fbo->produceImageForCurrentState();
    
	fbo->end();

	return ret;
}

/// 设置画刷
void CZBrushPreview::setBrush(CZBrush *brush_)
{
    /// 如果刷子生成器改变了，则笔刷图案改变
    if(ptrBrush && brush_ && ptrBrush->getGenerator()->isEqual(brush_->getGenerator()))
    {
        if(brushTexture) 
        {
            glContext->setAsCurrent();
            delete brushTexture; brushTexture=NULL;
        }
    }
    
    ptrBrush = brush_;
}

/// 设置设备屏幕分辨率倍数
void CZBrushPreview::setMainScreenScale(float s)
{
    mainScreenScale = s;
}

/// 实现Observer接口
void CZBrushPreview::updateObserver(std::string &notificationName, void* data /* = NULL */)
{
    // 	if (notificationName == CZBrushGeneratorChanged)
    // 	{
    // 		render.clearBrush();
    // 	}
}
