
///  \file CZBrushPreview.h
///  \brief This is the file declare the Class CZBrushPreview.
///
///		这是一个单例，展现笔刷的预览图。
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-16
///  \note

#include "../CZNotificationCenter.h"
#include "../graphic/CZGLContext.h"
#include "../graphic/CZShader.h"
#include "../graphic/CZTexture.h"
#include "../graphic/CZFbo.h"
#include "../basic/CZImage.h"
#include "../basic/CZMat4.h"
#include "../basic/CZSize.h"

class CZPath;
class CZBrush;

class CZBrushPreview : public CZObserver
{
public:
	/// 完成单例获取函数
	static CZBrushPreview * getInstance()
	{
		if(!ptrInstance) ptrInstance = new CZBrushPreview;
		return ptrInstance; 
	}

	static bool destroy() 
	{
		if(ptrInstance) delete ptrInstance;
		return true;
	}

	/// 生成指定尺寸大小预览图
	///		
	///		这里根据指定的大小，绘制出相应的图像。主要包括内部资源的建立以及画笔轨迹的绘制。
	///		不处理缩，窗口缩放由外部控制，这里只以参数中的尺寸为准。
	///		主要采用OpenGL的FBO和Shader。
	/// 
	///		/param size - 绘制预览的尺寸大小
	///		/return		- 绘制得到的笔刷轨迹图
	///		/note		使用之前应该先调用setBrush设定笔刷。
	CZImage *previewWithSize(const CZSize size_);
	/// 设置画刷
	void setBrush(CZBrush *brush_);
	/// 设置设备屏幕分辨率倍数
	void setMainScreenScale(float s);

	/// 实现Observer接口
	void updateObserver(std::string &notificationName, void* data = NULL);

private:
	CZBrushPreview();
	CZBrushPreview(const CZBrushPreview &);
	CZBrushPreview & operator = (const CZBrushPreview &);
	~CZBrushPreview();

	/// 启动新预览图（配置绘制器，生成绘制的轨迹）
	void setup(const CZSize &size_);
	/// 构建轨迹（绘制一条sin曲线）
	void buildPath();
	/// 配置笔刷
	void configureBrush();

public:
	/// 成员变量
	CZPath *path;							///< 绘制的轨迹 
	float backingWidth, backingHeight;
    float			mainScreenScale;		///< 设备屏幕的分辨率倍数，与PPI（每英寸像素数）相关

private:
	static CZBrushPreview *ptrInstance;    
	CZBrush			*ptrBrush;				///< 仅引用，不负责建立和销毁
	CZGLContext *	glContext;				///< gl上下文
	CZShader		*brushShader;
	CZTexture		*brushTexture;
	CZFbo			*fbo;					
	CZMat4			projMat;				///< 绘制的投影矩阵
};