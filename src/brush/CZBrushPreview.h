
///  \file CZBrushPreview.h
///  \brief This is the file declare the Class CZBrushPreview.
///
///		����һ��������չ�ֱ�ˢ��Ԥ��ͼ��
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
	/// ��ɵ�����ȡ����
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

	/// ����ָ���ߴ��СԤ��ͼ
	///		
	///		�������ָ���Ĵ�С�����Ƴ���Ӧ��ͼ����Ҫ�����ڲ���Դ�Ľ����Լ����ʹ켣�Ļ��ơ�
	///		���������������������ⲿ���ƣ�����ֻ�Բ����еĳߴ�Ϊ׼��
	///		��Ҫ����OpenGL��FBO��Shader��
	/// 
	///		/param size - ����Ԥ���ĳߴ��С
	///		/return		- ���Ƶõ��ı�ˢ�켣ͼ
	///		/note		ʹ��֮ǰӦ���ȵ���setBrush�趨��ˢ��
	CZImage *previewWithSize(const CZSize size_);
	/// ���û�ˢ
	void setBrush(CZBrush *brush_);
	/// �����豸��Ļ�ֱ��ʱ���
	void setMainScreenScale(float s);

	/// ʵ��Observer�ӿ�
	void updateObserver(std::string &notificationName, void* data = NULL);

private:
	CZBrushPreview();
	CZBrushPreview(const CZBrushPreview &);
	CZBrushPreview & operator = (const CZBrushPreview &);
	~CZBrushPreview();

	/// ������Ԥ��ͼ�����û����������ɻ��ƵĹ켣��
	void setup(const CZSize &size_);
	/// �����켣������һ��sin���ߣ�
	void buildPath();
	/// ���ñ�ˢ
	void configureBrush();

public:
	/// ��Ա����
	CZPath *path;							///< ���ƵĹ켣 
	float backingWidth, backingHeight;
    float			mainScreenScale;		///< �豸��Ļ�ķֱ��ʱ�������PPI��ÿӢ�������������

private:
	static CZBrushPreview *ptrInstance;    
	CZBrush			*ptrBrush;				///< �����ã���������������
	CZGLContext *	glContext;				///< gl������
	CZShader		*brushShader;
	CZTexture		*brushTexture;
	CZFbo			*fbo;					
	CZMat4			projMat;				///< ���Ƶ�ͶӰ����
};