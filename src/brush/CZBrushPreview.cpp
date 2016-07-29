
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

/// ��ʼ������
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

/// ���ٺ���
CZBrushPreview::~CZBrushPreview()
{
    if(path)			{	delete path; path = NULL; }
    
    glContext->setAsCurrent();
    if (brushShader)	{	delete brushShader; brushShader = NULL; }
    if (brushTexture)	{	delete brushTexture;brushTexture = NULL;}
    if (fbo)			{	delete fbo;			fbo = NULL;			}
    
    delete glContext;
}

/// ������Ԥ��ͼ�����û����������ɻ��ƵĹ켣��
void CZBrushPreview::setup(const CZSize &size_)
{
    if (backingWidth == size_.width && backingHeight == size_.height) return;
    
    backingWidth = size_.width;
    backingHeight = size_.height;
    
    /// ����·��
    if(path) delete path;
    
    buildPath();
    
    /// ����ͶӰ����
    projMat.SetOrtho(0.0f, (float)backingWidth, 0.0f, (float)backingHeight, -1.0f, 1.0f);
    CZMat4 scaleMat;
    scaleMat.SetScale(VECTOR3D(mainScreenScale,mainScreenScale,1.0f));
    projMat = projMat * scaleMat;
    
    fbo->setColorRenderBuffer(backingWidth,backingHeight);
}

/// �����켣������һ��sin���ߣ�
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
        float amplitude  = 10.0f;			///< ���
        
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

/// ���ñ�ˢ
void CZBrushPreview::configureBrush()
{
    if(brushTexture == NULL)
    {
        /// ��ȡ��ˢͼ��
        CZStampGenerator *gen = ptrBrush->getGenerator();
        CZImage *img = gen->getStamp(true);		///< get the small stamp;
        glContext->setAsCurrent();
        brushTexture = CZTexture::produceFromImage(img);
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,brushTexture->texId);
}

/// ����ָ���ߴ��СԤ��ͼ
CZImage* CZBrushPreview::previewWithSize(const CZSize size_)
{
	if(ptrBrush == NULL)
	{
		LOG_ERROR("ptrBrush is NULL\n");
		return NULL;
	}

	/// �����豸�ֱ��ʽ��е���
	CZSize size = size_ * mainScreenScale;

	glContext->setAsCurrent();
	/// �����¹켣
	setup(size);
	/// ���ñ�ˢ
	configureBrush();

	/// ���ù켣����
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

	/// ���ƹ켣
	path->paint(path->getRandomizer());

	brushShader->end();

    CZImage *ret = fbo->produceImageForCurrentState();
    
	fbo->end();

	return ret;
}

/// ���û�ˢ
void CZBrushPreview::setBrush(CZBrush *brush_)
{
    /// ���ˢ���������ı��ˣ����ˢͼ���ı�
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

/// �����豸��Ļ�ֱ��ʱ���
void CZBrushPreview::setMainScreenScale(float s)
{
    mainScreenScale = s;
}

/// ʵ��Observer�ӿ�
void CZBrushPreview::updateObserver(std::string &notificationName, void* data /* = NULL */)
{
    // 	if (notificationName == CZBrushGeneratorChanged)
    // 	{
    // 		render.clearBrush();
    // 	}
}
