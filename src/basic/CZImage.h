
///  \file CZImage.h
///  \brief This is the file declare the Class CZImage.
///
///		This file includes CZImage
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-09
///  \note

#ifndef _CZIMAGE_H_
#define _CZIMAGE_H_

#include "../CZDefine.h"
#include "CZRect.h"
#include <stdio.h>

typedef enum StorageMode 
{
	NOT_SET,RGB_BYTE, RGBA_BYTE, RGB_FLOAT, RGBA_FLOAT
} StorageMode;			///< ����ͼ��洢ģʽ

class CZImage
{
public:
	CZImage(int w_=0, int h_=0, StorageMode mode_ = DEFAULT_STORAGEMODE, const void* data_ = NULL);
	~CZImage();

	StorageMode getMode() { return mode;}

	/// ��ȿ�������
	CZImage* copy();
	/// ��䣨���ù��ѣ�
	///		\ret - ������������ͼƬ����
	///		\note - �÷������޸ı�ͼ��Ĵ�С
	CZImage* modifyDataFrom(int x,int y, float red, float green, float blue, float alpha, CZRect &modifiedRect);
	CZImage* modifyDataFrom1(int x,int y, float red, float green, float blue, float alpha, CZRect &modifiedRect);
	/// ɨ�����㷨(���)
	void ScanLineFill(int x,int y, float r, float g, float b, float a);
    
    ///< whether really has alpha data, which means the information in the 4th channel is not all 255 or 1.0f
    bool hasReallyAlpha();
    
    bool saveToFile(FILE *fp);
    
    static CZImage* createFromFile(FILE *fp);
    
private:
	/// �޸�
	void modifyData(int x,int y, float fillcolor[]);
	void modifyArea(CZImage * &backupImg,CZRect rect, float fillColor[]);
	/// �Ƿ���ɫ��ͬ
	inline bool isSameColorAt(int x,int y, float compareColor[]);
	/// ��ȡĳ����ɫ
	bool getColorAt(int x, int y, float color[]);

public:
	int			width,height;
	void		*data;              ///< ����������mode����
    bool        hasAlpha;           /// whether has alpha data

private:
	StorageMode	mode;
	bool		*flag;              ///< �������ĸ�������
};

#endif