
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

/// ����״̬��
class CZActiveState
{
public:
	/// ��ȡ����
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

	/// ���û���ģʽ
	void setEraseMode(bool flag);
	/// ��ȡ����ģʽ
	bool isEraseMode();
	/// ���û�����ɫ
	void setPaintColor(CZColor &pc);
	void setPaintColor(float r, float g, float b, float a = 1.0f);
	/// ��ȡ������ɫ
	CZColor getPaintColor();
	
	/// ����»�ˢ
	///		
	///		\param idx - ��ӵĻ�ˢ������ţ���idxΪ����ʱ��������
	///		\ret  ����ӵĻ�ˢ���
	///		\note ������ɻ�ˢ������ڵ�ǰ����Ļ�ˢ֮��
	int addNewBrush(int idx = -1);
	/// ���õ�ǰ���ˢ
	///
	///		\param idx - ���û�ˢ����ţ����Ƿ�ʱĬ��Ϊ0
	///		\ret	   - ԭ���ļ��ˢ���
	///		\note	��ǰ���õĻ�ˢ������ģʽ����
	int setActiveBrush(int idx);
	/// ��ȡ��ǰ��ˢ
	/// 
	///		\note ��ˢ�����ɵ�ǰ��״̬eraseMode����
	CZBrush *getActiveBrush();
	/// ɾ����ǰ��ˢ
	bool deleteActiveBrush();
    ///
    int getBrushesNumber();
    /// set active brush's stamp
    bool setActiveBrushStamp(CZImage* stampImg);

	/// ��ȡ��ǰ����
	/// 
	///		\note ���������ɵ�ǰ��״̬eraseMode����
	CZTool *getActiveTool();
	/// ���õ�ǰ����
	///
	///		\note ��Ҫ��Ϊ��ͳһ�ı乤�����Ӧ�Ļ���
	bool setPainting(CZPainting *p);
	

	/// ��ȡһ����ˢ������
	///
	///		\param idx - ��ӵĻ�ˢ������ţ���idxΪ����ʱ��������
	CZStampGenerator * getGenerator(int idx = -1);
    
    
    void setSwatch(CZColor* color, int idx);
    CZColor* getSwatch(int idx);
    bool setPaintColorAsSwatch(int idx);

private:
	CZActiveState();
	CZActiveState(const CZActiveState &);
	CZActiveState & operator = (const CZActiveState &);
	~CZActiveState();

	/// ����������
	int setUpGenerators();
	/// ��ʼ����ˢ
	int initBrushes();
    int initSwatches();
    
public:
    bool colorFillMode;                                 ///< whether stay in color fill mode
    bool colorPickMode;                                 ///< whether stay in color pick mode
    float mainScreenScale;                              ///< screen scale
    
private:
	static CZActiveState* ptrInstance;
	std::vector<CZBrush*> brushes;						///< ���п��õĻ�ˢ����������
	std::vector<CZTool*> tools;							///< ���ù��ߣ�ĿǰΪ���ƻ�ˢ�Ͳ���ˢ����
	std::map<std::string,CZStampGenerator*>	generators;	///< ���п��õĻ�ˢ����������������
    std::vector<CZColor*> swatches;
	bool eraseMode;										///< ����Ƿ��ڲ���ģʽ
	int paintBrushIdx;									///< �����ñ�ˢ���
	int eraseBrushIdx;									///< �����ñ�ˢ���

	CZColor paintColor;									///< ������ɫ
	CZGLContext *stampGLContext;						///< stamp��gl������
};

#endif