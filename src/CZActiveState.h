
///  \file CZActiveState.h
///  \brief This is the file declare the CZActiveState Class.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-28
///  \note		It is a Singleton

#ifndef _CZACTIVESTATE_H_
#define _CZACTIVESTATE_H_

#include "basic/CZColor.h"
#include <string>
#include <vector>
#include <map>

class CZStampGenerator;
class CZTool;
class CZBrush;
class CZPainting;
class CZColor;
class CZGLContext;
class CZImage;

typedef enum ToolType {
    kEraser = 0,
    kPencil,
    kMarker,
    kWatercolorPen,
    kCrayon,
    
    kToolNumber
} ToolType;

/// 激活状态类
class CZActiveState
{
public:
	/// 获取单例
	static CZActiveState * getInstance()
	{
		if(!ptrInstance) ptrInstance = new CZActiveState;
		return ptrInstance; 
	}

	static bool destroy() 
	{
		if(ptrInstance) delete ptrInstance;
		return true;
	}

	/// 设置绘制模式
	void setEraseMode(bool flag);
	/// 获取绘制模式
	bool isEraseMode();
	/// 设置绘制颜色
	void setPaintColor(CZColor &pc);
	void setPaintColor(float r, float g, float b, float a = 1.0f);
	/// 获取绘制颜色
	CZColor getPaintColor();
	
	/// 添加新画刷
	///		
	///		\param idx - 添加的画刷类型序号，当idx为负数时随机化序号
	///		\ret  新添加的画刷序号
	///		\note 随机生成画刷，添加在当前激活的画刷之后
	int addNewBrush(int idx = -1);
	/// 设置当前激活画刷
	///
	///		\param idx - 可用画刷的序号，当非法时默认为0
	///		\ret	   - 原来的激活画刷序号
	///		\note	当前设置的画刷种类由模式决定
	int setActiveBrush(int idx);
	/// 获取当前画刷
	/// 
	///		\note 画刷种类由当前的状态eraseMode决定
	CZBrush *getActiveBrush();
	/// 删除当前画刷
	bool deleteActiveBrush();
    ///
    int getBrushesNumber();
    /// set active brush's stamp
    bool setActiveBrushStamp(CZImage* stampImg);

	/// 获取当前工具
	/// 
	///		\note 工具种类由当前的状态eraseMode决定
	CZTool *getActiveTool();
	/// 设置当前绘制
	///
	///		\note 主要是为了统一改变工具类对应的绘制
	bool setPainting(CZPainting *p);
	

	/// 获取一个笔刷生成器
	///
	///		\param idx - 添加的画刷类型序号，当idx为负数时随机化序号
	CZStampGenerator * getGenerator(int idx = -1);
    
    
    void setSwatch(CZColor* color, int idx);
    CZColor* getSwatch(int idx);
    bool setPaintColorAsSwatch(int idx);

private:
	CZActiveState();
	CZActiveState(const CZActiveState &);
	CZActiveState & operator = (const CZActiveState &);
	~CZActiveState();

	/// 建立生成器
	int setUpGenerators();
	/// 初始化画刷
	int initBrushes();
    int initSwatches();
    
public:
    bool colorFillMode;                                 ///< whether stay in color fill mode
    bool colorPickMode;                                 ///< whether stay in color pick mode
    float mainScreenScale;                              ///< screen scale
    
private:
	static CZActiveState* ptrInstance;
	std::vector<CZBrush*> brushes;						///< 所有可用的画刷，负责销毁
	std::vector<CZTool*> tools;							///< 可用工具，目前为绘制画刷和擦除刷两种
	std::map<std::string,CZStampGenerator*>	generators;	///< 所有可用的画刷生成器，负责销毁
    std::vector<CZColor*> swatches;
	bool eraseMode;										///< 标记是否处在擦除模式
	int paintBrushIdx;									///< 绘制用笔刷序号
	int eraseBrushIdx;									///< 擦除用笔刷序号

	CZColor paintColor;									///< 绘制颜色
	CZGLContext *stampGLContext;						///< stamp的gl上下文
};

#endif