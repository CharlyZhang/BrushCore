  
///  \file CZActiveState.cpp
///  \brief This is the file implements the CZActiveState Class.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-28
///  \note		It is a Singleton

#include "CZActiveState.h"
#include "CZDefine.h"
#include "basic/CZImage.h"
#include "brush/CZBrush.h"
#include "stamp/CZSpiralGenerator.h"
#include "stamp/CZRoundGenerator.h"
#include "stamp/CZBristleGenerator.h"
#include "tool/CZFreehandTool.h"
#include "tool/CZEraserTool.h"
#include "CZColor.h"
#include "CZUtil.h"
#include "graphic/CZGLContext.h"

using namespace std;

CZActiveState *CZActiveState::ptrInstance = NULL;

CZActiveState::CZActiveState()
{
	eraseMode = false;
	paintColor = CZColor(82/255.0f,178/255.0f,228/255.0f,1.0f);
	stampGLContext = new CZGLContext;
	setUpGenerators();
	initBrushes();
    initSwatches();

	tools.push_back(new CZFreehandTool);
	tools.push_back(new CZEraserTool);
    
    colorFillMode = false;
    colorPickMode = false;
}
CZActiveState::~CZActiveState()
{
	for(vector<CZBrush*>::iterator itr = brushes.begin(); itr != brushes.end(); itr++)
		delete *itr;
	brushes.clear();

	for(vector<CZTool*>::iterator itr = tools.begin(); itr != tools.end(); itr++)
		delete *itr;
	tools.clear();

	for(map<string,CZStampGenerator*>::iterator itr = generators.begin(); itr != generators.end(); itr++)
		delete itr->second;
	generators.clear();
    
    for(vector<CZColor*>::iterator itr = swatches.begin(); itr != swatches.end(); itr++)
        delete *itr;
    swatches.clear();

	delete stampGLContext;
}

/// 设置绘制模式
void CZActiveState::setEraseMode(bool flag)
{
	eraseMode = flag;
}

/// 获取绘制模式
bool CZActiveState::isEraseMode()
{
	return eraseMode;
}

/// 设置绘制颜色
void CZActiveState::setPaintColor(CZColor &pc)
{
	paintColor = pc;
}
void CZActiveState::setPaintColor(float r, float g, float b, float a /* = 1.0f */)
{
	paintColor = CZColor(r,g,b,a);
}

/// 获取绘制颜色
CZColor CZActiveState::getPaintColor()
{
	return paintColor;
}

/// 添加新画刷
/// 
///		\ret  新添加的画刷序号
///		\note 随机生成画刷，添加在当前激活的画刷之后
int CZActiveState::addNewBrush(int idx /* = -1*/)
{
	CZStampGenerator *gen= getGenerator(idx)->copy();
	gen->randomize();

	CZBrush* random = new CZBrush(gen);
	//random->weight.value = CZUtil::RandomFloat() * 56 + 44;
	//random->intensity.value = 0.15f;
	//random->spacing.value = 0.02;

	int activeBrushIdx = eraseMode ? eraseBrushIdx : paintBrushIdx;

	brushes.insert(brushes.begin()+(activeBrushIdx+1),random);
	
	/// update eraseBrushIdx & paintBrushIdx and keep activeBrushIdx unchanged 
	if(paintBrushIdx>activeBrushIdx)	paintBrushIdx++;
	if(eraseBrushIdx>activeBrushIdx)	eraseBrushIdx++;

	return activeBrushIdx+1;
}

/// 设置当前激活画刷
///
///		\param idx - 可用画刷的序号，当非法时默认为0
///		\ret	   - 原来的激活画刷序号
///		\note	当前设置的画刷种类由模式决定
int CZActiveState::setActiveBrush(int idx)
{
	int oldIdx = eraseMode ? eraseBrushIdx : paintBrushIdx;

	if (idx < 0 || idx >= brushes.size()) 
	{
		LOG_WARN("idx is out of range!\n");
		return oldIdx;
	}

//	if (eraseMode)	eraseBrushIdx = idx;
//	else			paintBrushIdx = idx;
    
    eraseBrushIdx = paintBrushIdx = idx;

	return oldIdx;
}

/// 获取当前画刷
/// 
///		\note 画刷种类由当前的状态eraseMode决定
CZBrush *CZActiveState::getActiveBrush()
{
	CZBrush * ret = eraseMode ? brushes[eraseBrushIdx] : brushes[paintBrushIdx];

	return ret;
}

/// 删除当前画刷
bool CZActiveState::deleteActiveBrush()
{
	if(brushes.size() == 1) 
	{
		LOG_WARN("there must be at least one brush!\n");
		return false;
	}
	
	int activeBrushIdx = eraseMode ? eraseBrushIdx : paintBrushIdx;
	vector<CZBrush*>::iterator itr = brushes.begin() + activeBrushIdx;

	/// in case that the last brush has been deleted
	if(activeBrushIdx == brushes.size())	activeBrushIdx --;
	/// update eraseBrushIdx & paintBrushIdx 
	if(paintBrushIdx>=activeBrushIdx)	paintBrushIdx--;
	if(eraseBrushIdx>=activeBrushIdx)	eraseBrushIdx--;

	delete *itr;
	brushes.erase(itr);

	return true;
}

///
int CZActiveState::getBrushesNumber()
{
    return (int)brushes.size();
}

bool CZActiveState::setActiveBrushStamp(CZImage* stampImg)
{
    if(!stampImg)   return false;
    CZBrush *activeBrush = getActiveBrush();
    activeBrush->setStampImage(stampImg);
    return true;
}

/// 获取当前工具
/// 
///		\note 工具种类由当前的状态eraseMode决定
CZTool* CZActiveState::getActiveTool()
{
	return eraseMode ? tools[1] : tools[0];
}

/// 设置当前绘制
///
///		\note 主要是为了统一改变工具类对应的绘制
bool CZActiveState::setPainting(CZPainting *p)
{
	tools[0]->ptrPainting = tools[1]->ptrPainting = p;
	return true;
}

/// 获取一个笔刷生成器
///
///		\param idx - 添加的画刷类型序号，当idx为负数时随机化序号
CZStampGenerator * CZActiveState::getGenerator(int idx /* = -1*/)
{
	size_t genNum = generators.size();
	if (genNum > 0 && idx < genNum)
	{
		if(idx < 0)	idx = rand() % genNum;
		for(map<string,CZStampGenerator*>::iterator itr = generators.begin(); itr != generators.end(); itr++)
            if(idx-- == 0)  return itr->second;
        
    }

	LOG_ERROR("generator candidates is none or idx is larger than genNum!\n");
	return NULL;
}

void CZActiveState::setSwatch(CZColor *color, int idx)
{
    if (idx < 0 || idx >= 30) {
        LOG_ERROR("idx is out of range!\n");
        return;
    }
    
    swatches[idx] = color;
}

CZColor* CZActiveState::getSwatch(int idx)
{
    if (idx >= swatches.size() || idx < 0) return NULL;
    return swatches[idx];
}

bool CZActiveState::setPaintColorAsSwatch(int idx)
{
    CZColor *color = new CZColor(paintColor.red,paintColor.green,paintColor.blue,paintColor.alpha);
    setSwatch(color, idx);
    return true;
}

/// 建立生成器
int CZActiveState::setUpGenerators()
{
	generators["spiral"] = new CZSpiralGenerator(stampGLContext);
	generators["round"] = new CZRoundGenerator(stampGLContext);
	generators["bristle"] = new CZBristleGenerator(stampGLContext);

	return (int)generators.size();
}

/// 初始化画刷
int CZActiveState::initBrushes()
{
    int brushNum = ToolType::kToolNumber;

	eraseBrushIdx = paintBrushIdx = -1;
    
    CZStampGenerator *gen = NULL;
    for(int i=0; i<brushNum; i++)	{
        CZBrush* random = NULL;
        ToolType type = (ToolType)i;
        switch(type){
            case kEraser:
                gen = generators["round"]->copy();
                gen->randomize();
                random = new CZBrush(gen);
                random->weight.value = 50;
                break;
            case kPencil:
                gen = generators["bristle"]->copy();
                gen->randomize();
                ((CZBristleGenerator*)gen)->bristleDensity.value = 0.29f;
                ((CZBristleGenerator*)gen)->bristleSize.value = 0.82f;
                random = new CZBrush(gen);
                random->weight.value = 4.24f;
                random->intensity.value = 0.49f;
                random->angle.value = 0;
                random->spacing.value = 0.4f;
                random->rotationalScatter.value =0;
                random->positionalScatter.value =0.17f;
                random->angleDynamics.value = 0;
                random->weightDynamics.value = -0.42f;
                random->intensityDynamics.value = -0.5f;
                break;
            case kMarker:
                gen = generators["round"]->copy();
                gen->randomize();
                random = new CZBrush(gen);
                random->weight.value = 20;
                random->intensity.value = 1.0f;
                random->angle.value = 166;
                random->spacing.value = 0;
                random->rotationalScatter.value =0.46f;
                random->positionalScatter.value =0;
                random->angleDynamics.value = 1.0f;
                random->weightDynamics.value = 0.07f;
                random->intensityDynamics.value = -0.5f;
                break;
            case kCrayon:
                gen = generators["bristle"]->copy();
                gen->randomize();
                ((CZBristleGenerator*)gen)->bristleDensity.value = 0.59f;
                ((CZBristleGenerator*)gen)->bristleSize.value = 1.0f;
                random = new CZBrush(gen);
                random->weight.value = 30;
                random->intensity.value = 0.39f;
                random->angle.value = 115.71f;
                random->spacing.value = 0.36f;
                random->rotationalScatter.value =0.27f;
                random->positionalScatter.value =0;
                random->angleDynamics.value = 1.0f;
                random->weightDynamics.value = 0.03f;
                random->intensityDynamics.value = -0.43f;
                break;
            case kWatercolorPen:
                random = new CZBrush();         ///< the stamp image is non-generated
                random->weight.value = 100;
                random->intensity.value = 0.15f;
                random->angle.value = 42.53f;
                random->spacing.value = 0.08f;
                random->rotationalScatter.value =0.31f;
                random->positionalScatter.value =0.05f;
                random->angleDynamics.value = -0.07f;
                random->weightDynamics.value = 0.35f;
                random->intensityDynamics.value = -0.75f;
                break;

                break;
            default:
                LOG_WARN("idx cannot specify one tool type!\n");
        }
    
        if(random)  brushes.insert(brushes.begin()+i,random);
    }
	
	eraseBrushIdx = paintBrushIdx = 0;
	
	return brushNum;
}

int CZActiveState::initSwatches()
{
    for (int i=0; i<30; i++) {  swatches.push_back(NULL); }
    
    // add some default swatches
    int total = 0;
    setSwatch(CZColor::CZColorWithHSV(180.0f / 360, 0.21f, 0.56f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(138.0f / 360, 0.36f, 0.71f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(101.0f / 360, 0.38f, 0.49f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(215.0f / 360, 0.34f, 0.87f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(207.0f / 360, 0.90f, 0.64f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(229.0f / 360, 0.59f, 0.45f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(331.0f / 360, 0.28f, 0.51f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(44.0f / 360, 0.77f, 0.85f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(15.0f / 360, 0.39f, 0.98f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(84.0f / 360, 0.15f, 0.9f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(59.0f / 360, 0.27f, 0.99f, 1), total);    total++;
    setSwatch(CZColor::CZColorWithHSV(51.0f / 360, 0.08f, 0.96f, 1), total);    total++;
    
    for (int i = 0; i <= 4; i++) {
        float w = i; w /= 4.0f;
        setSwatch(CZColor::CZColorWithHSV(0, 0, w, 1), total);    total++;
    }
    
    return total;
}

