
///  \file CZPainting.h
///  \brief This is the file declare the class CZPainting.
///
///		
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-26
///  \note

#ifndef _CZPAINTING_H_
#define _CZPAINTING_H_

#include "../CZDefine.h"
#include "../basic/CZSize.h"
#include "../basic/CZRect.h"
#include "../basic/CZColor.h"
#include "../basic/CZImage.h"
#include "../basic/CZRandom.h"
#include "../basic/CZMat4.h"
#include "../path/CZPath.h"
#include "../brush/CZBrush.h"
#include "../serialization/CZCoding.h"
#include "../graphic/CZGLContext.h"
#include "../graphic/CZShader.h"
#include "../graphic/CZTexture.h"
#include "../graphic/CZFbo.h"
#include "CZLayer.h"

#include <string>
#include <vector>
#include <map>

class CZCanvas;

/// 绘制类
class CZPainting :public CZCoding
{
public:
    typedef enum Type {
        kNormalPainting = 0,        //< normal type
        kFixedBGLayerPainting       //< the painting with a fixed layer for background
    } Type;
    
	CZPainting(const CZSize &size, bool addDefaultLayer = true);
	~CZPainting();
	
	/// 将图像绘制出来（没绑定FBO）
	void blit(CZMat4 &projection);
	/// 绘制一条轨迹（绘制到纹理）
	CZRect paintStroke(CZPath *path_, CZRandom *randomizer, bool clearBuffer = false);

	/// 生成所有图层的图像（不包括当前绘制的笔画）
	CZImage *imageWithSize(CZSize &size, CZColor *backgroundColor = NULL);
	/// 生成当前状态的图像
	CZImage *imageForCurrentState(CZSize &size, CZColor *backgroundColor = NULL);
    /// produce thumbnail of default size
    CZImage *thumbnailImage(bool addPaintingPath = false);
	
    /// Type
    void setType(Type t) { type = t;};
    Type getType() { return type; };
    
	/// 设置范围（让render的范围与其保持一致）
	void setDimensions(const CZSize &size);
	/// set canvas
	bool setCanvas(CZCanvas* c);
	/// 设置激活轨迹
	void setActivePath(CZPath *path);

	/// 设置当前激活图层
	///
	///		\param idx - 当前需要激活的图层序号
	///		\ret	   - 原来激活的图层序号
	int setActiveLayer(int idx);
    int getActiveLayerIndex(){return activeLayerInd;};

	/// 通过UID获取图层
	/// 
	///		\note 不存在该UID的图层则返回NULL
	CZLayer *layerWithUID(unsigned int uid_);
    
    ///
    CZLayer *getLayer(int idx);
	
	/// 添加新图层
	/// 
	///		\ret		 - 在所有图层中的序号。超过最大层数：-1，生成图层失败：-2
	int addNewLayer();
	
	/// 删除图层
	/// 
	///		\note 当layer被锁住的时候不能被删除 
	bool deleteActiveLayer();
    
    /// restore last deleted layer
    bool restoreDeletedLayer();
    
    ///
    int duplicateActiveLayer();

	/// 移动图层
	/// 
	///		\param fromIdx - 需要移动的图层序号
	///		\param toIdx   - 移动到的位置
	bool moveLayer(int fromIdx, int toIdx);
	
	/// 向下合并当前图层
	/// 
	///		\ret - 是否合并成功
	bool mergeActiveLayerDown();

	/// 获取激活轨迹
	CZPath* getActivePath();

	/// 获取激活图层
	CZLayer* getActiveLayer();
    /// get layer number
    int getLayersNumber();

	/// 获取着色器
	CZShader* getShader(std::string name);

	/// 生成纹理
	CZTexture* generateTexture(CZImage* img = NULL);

	/// 返回quadVAO
	GLuint getQuadVAO();
	
	/// 获取范围
	CZSize& getDimensions();
	/// 获取绘制矩形
	CZRect& getBounds();

	/// 获取gl上下文
	CZGLContext *getGLContext();
    
    /// clear last brush
    void clearLastBrush(){ptrLastBrush = NULL;}
    
    /// determine whether to prevent paint
    bool shouldPreventPaint();
    
    /// pick the color
    CZColor pickColor(int x, int y);
    
    /// restore the painting
    bool restore(bool addDefaultLayer = true);
    
	/// 实现coding的接口
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);

private:
	/// 载入着色器
	void loadShaders();
	/// 配置画刷
	void configureBrush(CZBrush*);

	/// 图册操作
	int removeLayer(int idx);
	int insertLayer(CZLayer* layer, int idx);

public:
	CZTexture *activePaintTexture;			///< 绘制用的纹理

private:
    Type type;
	CZSize dimensions;				///< 绘制范围
    CZRect bounds;
	CZPath *ptrActivePath;			///< 激活的路径，此处仅为引用
	int		activeLayerInd;			///< 活动的图层序号
    CZCanvas *ptrCanvas;            ///< point to Canvas for displaying the stroke

	bool flattenMode;
	std::vector<CZLayer*>	layers;	///< 绘制的画层
	std::vector<CZColor*>	colors;	
	std::vector<CZBrush*>	brushPtrs;
	std::vector<CZBrush*>	undoBrushPtrs;
	int						strokeCount;	///< 笔画数目
	char*					uuid;

	CZMat4					projMat;		///< 投影矩阵
	CZBrush					*ptrLastBrush;	///< 上一把画刷
	CZGLContext				*glContext;		///< gl上下文
	std::map<std::string,CZShader*>	shaders;///< 着色器
	GLuint quadVAO,quadVBO;					///< 绘制矩形的VAO、VBO
	CZFbo					*fbo;			
	CZTexture				*brushStampTex;	///< 画刷笔触纹理
    
    CZLayer                 *lastDeletedLayer;  ///< for undo the operation of `layer-deleteing`
    int                     lastDeletedLayerIdx;
};
#endif