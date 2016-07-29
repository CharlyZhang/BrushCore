
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

typedef	enum BlendMode				///< ���ģʽ
{
	kBlendModeNormal,
	kBlendModeMultiply,
	kBlendModeScreen,
	kBlendModeExclusion
} BlendMode;

class CZPaintingFragment		///< ����Ƭ��
{
public:
	CZPaintingFragment(CZImage *data_,CZRect &rect):data(data_),bounds(rect){};
	~CZPaintingFragment()	{delete data;}
	CZImage *data;
	CZRect  bounds;
};

/// ͼ����
class CZLayer : public CZCoding
{
public:
	friend class CZPainting;
    friend class CZLayer;
    
	/// ͼ���ͼ������
	CZImage *imageData();
	/// �����ض����������ͼ������
	CZImage *imageDataInRect(const CZRect &rect);
	
	/// ����ͼ��
	void basicBlit(CZMat4 &projection);
	/// ����ͼ�㣨�����ƶ�ת������ɫ�����ȣ�
	void blit(CZMat4 &projection);
	/// ���Ӳ�������
	void blit(CZMat4 &projection, CZTexture *maskTexture);
	/// ���ӻ�������
	void blit(CZMat4 &projection, CZTexture *maskTexture, CZColor &bgColor);
	
	/// �����Ƶıʻ��ϲ�����ǰͼ��
	void commitStroke(CZRect &bounds, CZColor &color, bool erase, bool undoable);
	
	/// ������ɫ
	void commitColorAdjustments();
	
	/// �ϲ�����ͼ��
	bool merge(CZLayer *layer);
    
    /// clear
    bool clear();

	/// ��ͼ�񾭹��任�����
	void renderImage(CZImage* img, CZAffineTransform &trans);
    /// render background image
    void renderBackground(CZImage *img);
    /// transform layer
    bool transform(CZAffineTransform &trans, bool undoable);
    
    CZLayer* duplicate();
    
    void enableLinearInterprolation(bool flag);
    
	/// ���û���ָ��
	void setPainting(CZPainting *painting);
	/// ����ת������û�����ı䣬�򷵻�false��
	bool setTransform(CZAffineTransform &trans);
	/// ��ȡת������
	CZAffineTransform & getTransform();
	/// ���û��ģʽ��û�����ı䣬�򷵻�false��
	bool setBlendMode(BlendMode &bm);
	/// ��ȡ���ģʽ
	BlendMode getBlendMode();
	/// ���ò�͸���ȣ�û�����ı䣬�򷵻�false��
	bool setOpacity(float o);
	/// ��ȡ��͸����
	float getOpacity();
	/// ���õ�����ɫ��û�����ı䣬�򷵻�false��
	bool setColorBalance(CZColorBalance *cb);
	/// ����ɫ������
	bool setHueSaturation(CZHueSaturation *hs);
	/// ����ͼ��ͼ��
	/// 
	///		\param img - ���õ�ͼ��
	///		\ret	   - ��img��Ϊ�գ������ó¹�
	///		\note ���ô˺���������֮ǰ�Ըò�����л���
	bool setImage(CZImage *img);
	/// ��������
	bool undoAction();
	/// ��������
	bool redoAction();

	/// �л��ɼ���
	void toggleVisibility();
	/// �л�alpha����
	void toggleAlphaLocked();
	/// �л�ͼ������
	void toggleLocked();
	/// ���ÿɼ���
	void setVisiblility(bool flag);
	/// ����alpha����
	void setAlphaLocked(bool flag);
	/// ����ͼ������
	void setLocked(bool flag);
	/// �Ƿ���סͼ��
	bool isLocked();
	/// �Ƿ���סalpha
	bool isAlphaLocked();
	/// �Ƿ�ɼ�
	bool isVisible();
	/// �Ƿ�ɱ༭
	bool isEditable();

	/// ��ȡ���
	char* getUUID();

	/// ���
	bool fill(CZColor &c, CZ2DPoint &p);
    
    /// get thumbnail image
    CZImage *getThumbnailImage();

	/// ʵ��coding�Ľӿ�
	void update(CZDecoder *decoder_, bool deep = false);
	void encode(CZCoder *coder_, bool deep = false);

private:
	/// ֻ������Ԫ��CZPainting���ɺ��ͷ�
	CZLayer(CZPainting *painting_);
	~CZLayer();
	///���û��ģʽ 
	void configureBlendMode();
	/// ������ת�������			
	void blit(CZMat4 &projection, const CZAffineTransform &transform);
	/// ע�᳷������
	void registerUndoInRect(CZRect &rect);

    void clearThumbnailImage();
    
public:
	bool canRedo,canUndo;
	CZRect modifiedRect;

private:
	bool visible;								///< �Ƿ�ɼ�
	bool alphaLocked;							///< alpha�Ƿ�����
	bool locked;								///< ͼ������
	bool clipWhenTransformed;					///< ͼ��任ʱ�Ƿ����
	CZColorBalance *colorBalance;				///< ������ɫ
	CZHueSaturation *hueSaturation;				///< ����ɫ�����Ͷ�
	BlendMode blendMode;						///< ���ģʽ
	CZAffineTransform transformMat;				///< �任����
	float opacity;								///< ��͸����[0.0, 1.0]			

	CZPainting *ptrPainting;					///< ĳ�λ���
	CZImage *image;								///< ��ͼ��ͼ�����ڽ�����ʼͼ������
	CZSaveStatus isSaved;						///< ͼ��洢״̬
	CZTexture *myTexture;						///< ��������
	CZTexture *hueChromaLuma;					///
	CZPaintingFragment *undoFragment, *redoFragment;
												///< ������������Ƭ��

	CZGLContext *ptrGLContext;					///< gl������
	char* uuid;									///< ���
    
    CZImage *thumbnailImg;                      ///< thumbnail
};

#endif