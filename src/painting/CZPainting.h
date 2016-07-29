
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

/// ������
class CZPainting :public CZCoding
{
public:
    typedef enum Type {
        kNormalPainting = 0,        //< normal type
        kFixedBGLayerPainting       //< the painting with a fixed layer for background
    } Type;
    
	CZPainting(const CZSize &size, bool addDefaultLayer = true);
	~CZPainting();
	
	/// ��ͼ����Ƴ�����û��FBO��
	void blit(CZMat4 &projection);
	/// ����һ���켣�����Ƶ�����
	CZRect paintStroke(CZPath *path_, CZRandom *randomizer, bool clearBuffer = false);

	/// ��������ͼ���ͼ�񣨲�������ǰ���Ƶıʻ���
	CZImage *imageWithSize(CZSize &size, CZColor *backgroundColor = NULL);
	/// ���ɵ�ǰ״̬��ͼ��
	CZImage *imageForCurrentState(CZSize &size, CZColor *backgroundColor = NULL);
    /// produce thumbnail of default size
    CZImage *thumbnailImage(bool addPaintingPath = false);
	
    /// Type
    void setType(Type t) { type = t;};
    Type getType() { return type; };
    
	/// ���÷�Χ����render�ķ�Χ���䱣��һ�£�
	void setDimensions(const CZSize &size);
	/// set canvas
	bool setCanvas(CZCanvas* c);
	/// ���ü���켣
	void setActivePath(CZPath *path);

	/// ���õ�ǰ����ͼ��
	///
	///		\param idx - ��ǰ��Ҫ�����ͼ�����
	///		\ret	   - ԭ�������ͼ�����
	int setActiveLayer(int idx);
    int getActiveLayerIndex(){return activeLayerInd;};

	/// ͨ��UID��ȡͼ��
	/// 
	///		\note �����ڸ�UID��ͼ���򷵻�NULL
	CZLayer *layerWithUID(unsigned int uid_);
    
    ///
    CZLayer *getLayer(int idx);
	
	/// �����ͼ��
	/// 
	///		\ret		 - ������ͼ���е���š�������������-1������ͼ��ʧ�ܣ�-2
	int addNewLayer();
	
	/// ɾ��ͼ��
	/// 
	///		\note ��layer����ס��ʱ���ܱ�ɾ�� 
	bool deleteActiveLayer();
    
    /// restore last deleted layer
    bool restoreDeletedLayer();
    
    ///
    int duplicateActiveLayer();

	/// �ƶ�ͼ��
	/// 
	///		\param fromIdx - ��Ҫ�ƶ���ͼ�����
	///		\param toIdx   - �ƶ�����λ��
	bool moveLayer(int fromIdx, int toIdx);
	
	/// ���ºϲ���ǰͼ��
	/// 
	///		\ret - �Ƿ�ϲ��ɹ�
	bool mergeActiveLayerDown();

	/// ��ȡ����켣
	CZPath* getActivePath();

	/// ��ȡ����ͼ��
	CZLayer* getActiveLayer();
    /// get layer number
    int getLayersNumber();

	/// ��ȡ��ɫ��
	CZShader* getShader(std::string name);

	/// ��������
	CZTexture* generateTexture(CZImage* img = NULL);

	/// ����quadVAO
	GLuint getQuadVAO();
	
	/// ��ȡ��Χ
	CZSize& getDimensions();
	/// ��ȡ���ƾ���
	CZRect& getBounds();

	/// ��ȡgl������
	CZGLContext *getGLContext();
    
    /// clear last brush
    void clearLastBrush(){ptrLastBrush = NULL;}
    
    /// determine whether to prevent paint
    bool shouldPreventPaint();
    
    /// pick the color
    CZColor pickColor(int x, int y);
    
    /// restore the painting
    bool restore(bool addDefaultLayer = true);
    
	/// ʵ��coding�Ľӿ�
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);

private:
	/// ������ɫ��
	void loadShaders();
	/// ���û�ˢ
	void configureBrush(CZBrush*);

	/// ͼ�����
	int removeLayer(int idx);
	int insertLayer(CZLayer* layer, int idx);

public:
	CZTexture *activePaintTexture;			///< �����õ�����

private:
    Type type;
	CZSize dimensions;				///< ���Ʒ�Χ
    CZRect bounds;
	CZPath *ptrActivePath;			///< �����·�����˴���Ϊ����
	int		activeLayerInd;			///< ���ͼ�����
    CZCanvas *ptrCanvas;            ///< point to Canvas for displaying the stroke

	bool flattenMode;
	std::vector<CZLayer*>	layers;	///< ���ƵĻ���
	std::vector<CZColor*>	colors;	
	std::vector<CZBrush*>	brushPtrs;
	std::vector<CZBrush*>	undoBrushPtrs;
	int						strokeCount;	///< �ʻ���Ŀ
	char*					uuid;

	CZMat4					projMat;		///< ͶӰ����
	CZBrush					*ptrLastBrush;	///< ��һ�ѻ�ˢ
	CZGLContext				*glContext;		///< gl������
	std::map<std::string,CZShader*>	shaders;///< ��ɫ��
	GLuint quadVAO,quadVBO;					///< ���ƾ��ε�VAO��VBO
	CZFbo					*fbo;			
	CZTexture				*brushStampTex;	///< ��ˢ�ʴ�����
    
    CZLayer                 *lastDeletedLayer;  ///< for undo the operation of `layer-deleteing`
    int                     lastDeletedLayerIdx;
};
#endif