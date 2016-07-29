
///  \file CZLayer.h
///  \brief This is the file declare the class CZLayer.
///
///		
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-15
///  \note

#ifndef _CZLAYER_H_
#define _CZLAYER_H_

#include "../CZDefine.h"
#include "../basic/CZImage.h"
#include "../basic/CZColor.h"
#include "../basic/CZMat4.h"
#include "../basic/CZAffineTransform.h"
#include "../CZColorBalance.h"
#include "../CZHueSaturation.h"
#include "../basic/CZAffineTransform.h"
#include "../serialization/CZCoding.h"
#include "../serialization/CZDataProvider.h"
#include "../CZColorBalance.h"
#include "../CZHueSaturation.h"
#include "../graphic/CZTexture.h"
#include "../graphic/CZGLContext.h"

class CZPainting;
class CZRect;

typedef	enum BlendMode				///< 混合模式
{
	kBlendModeNormal,
	kBlendModeMultiply,
	kBlendModeScreen,
	kBlendModeExclusion
} BlendMode;

class CZPaintingFragment		///< 绘制片段
{
public:
	CZPaintingFragment(CZImage *data_,CZRect &rect):data(data_),bounds(rect){};
	~CZPaintingFragment()	{delete data;}
	CZImage *data;
	CZRect  bounds;
};

/// 图层类
class CZLayer : public CZCoding
{
public:
	friend class CZPainting;
    friend class CZLayer;
    
	/// 图层的图像数据
	CZImage *imageData();
	/// 生成特定矩形区域的图像数据
	CZImage *imageDataInRect(const CZRect &rect);
	
	/// 绘制图层
	void basicBlit(CZMat4 &projection);
	/// 绘制图层（考虑移动转换、颜色调整等）
	void blit(CZMat4 &projection);
	/// 叠加擦除纹理
	void blit(CZMat4 &projection, CZTexture *maskTexture);
	/// 叠加绘制纹理
	void blit(CZMat4 &projection, CZTexture *maskTexture, CZColor &bgColor);
	
	/// 将绘制的笔画合并到当前图层
	void commitStroke(CZRect &bounds, CZColor &color, bool erase, bool undoable);
	
	/// 调整颜色
	void commitColorAdjustments();
	
	/// 合并另以图层
	bool merge(CZLayer *layer);
    
    /// clear
    bool clear();

	/// 将图像经过变换后绘制
	void renderImage(CZImage* img, CZAffineTransform &trans);
    /// render background image
    void renderBackground(CZImage *img);
    /// transform layer
    bool transform(CZAffineTransform &trans, bool undoable);
    
    CZLayer* duplicate();
    
    void enableLinearInterprolation(bool flag);
    
	/// 设置绘制指针
	void setPainting(CZPainting *painting);
	/// 设置转换矩阵（没发生改变，则返回false）
	bool setTransform(CZAffineTransform &trans);
	/// 获取转换矩阵
	CZAffineTransform & getTransform();
	/// 设置混合模式（没发生改变，则返回false）
	bool setBlendMode(BlendMode &bm);
	/// 获取混合模式
	BlendMode getBlendMode();
	/// 设置不透明度（没发生改变，则返回false）
	bool setOpacity(float o);
	/// 获取不透明度
	float getOpacity();
	/// 设置调整颜色（没发生改变，则返回false）
	bool setColorBalance(CZColorBalance *cb);
	/// 设置色调调整
	bool setHueSaturation(CZHueSaturation *hs);
	/// 设置图层图像
	/// 
	///		\param img - 设置的图像
	///		\ret	   - 若img不为空，则设置陈宫
	///		\note 调用此函数将覆盖之前对该层的所有绘制
	bool setImage(CZImage *img);
	/// 撤销操作
	bool undoAction();
	/// 重做操作
	bool redoAction();

	/// 切换可见性
	void toggleVisibility();
	/// 切换alpha锁定
	void toggleAlphaLocked();
	/// 切换图层锁定
	void toggleLocked();
	/// 设置可见性
	void setVisiblility(bool flag);
	/// 设置alpha锁定
	void setAlphaLocked(bool flag);
	/// 设置图层锁定
	void setLocked(bool flag);
	/// 是否被锁住图层
	bool isLocked();
	/// 是否被锁住alpha
	bool isAlphaLocked();
	/// 是否可见
	bool isVisible();
	/// 是否可编辑
	bool isEditable();

	/// 获取编号
	char* getUUID();

	/// 填充
	bool fill(CZColor &c, CZ2DPoint &p);
    
    /// get thumbnail image
    CZImage *getThumbnailImage();

	/// 实现coding的接口
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);

private:
	/// 只能由友元类CZPainting生成和释放
	CZLayer(CZPainting *painting_);
	~CZLayer();
	///配置混合模式 
	void configureBlendMode();
	/// 将纹理转换后绘制			
	void blit(CZMat4 &projection, const CZAffineTransform &transform);
	/// 注册撤销操作
	void registerUndoInRect(CZRect &rect);

    void clearThumbnailImage();
    
public:
	bool canRedo,canUndo;
	CZRect modifiedRect;

private:
	bool visible;								///< 是否可见
	bool alphaLocked;							///< alpha是否锁定
	bool locked;								///< 图层锁定
	bool clipWhenTransformed;					///< 图层变换时是否剪切
	CZColorBalance *colorBalance;				///< 调整颜色
	CZHueSaturation *hueSaturation;				///< 调整色调饱和度
	BlendMode blendMode;						///< 混合模式
	CZAffineTransform transformMat;				///< 变换矩阵
	float opacity;								///< 不透明度[0.0, 1.0]			

	CZPainting *ptrPainting;					///< 某次绘制
	CZImage *image;								///< 该图层图像（用于建立初始图层纹理）
	CZSaveStatus isSaved;						///< 图层存储状态
	CZTexture *myTexture;						///< 自身纹理
	CZTexture *hueChromaLuma;					///
	CZPaintingFragment *undoFragment, *redoFragment;
												///< 撤销和重做的片段

	CZGLContext *ptrGLContext;					///< gl上下文
	char* uuid;									///< 编号
    
    CZImage *thumbnailImg;                      ///< thumbnail
};

#endif