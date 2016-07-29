
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
} StorageMode;			///< 声明图像存储模式

class CZImage
{
public:
	CZImage(int w_=0, int h_=0, StorageMode mode_ = DEFAULT_STORAGEMODE, const void* data_ = NULL);
	~CZImage();

	StorageMode getMode() { return mode;}

	/// 深度拷贝函数
	CZImage* copy();
	/// 填充（采用广搜）
	///		\ret - 返回填充区域的图片数据
	///		\note - 该方法会修改本图像的大小
	CZImage* modifyDataFrom(int x,int y, float red, float green, float blue, float alpha, CZRect &modifiedRect);
	CZImage* modifyDataFrom1(int x,int y, float red, float green, float blue, float alpha, CZRect &modifiedRect);
	/// 扫描线算法(封闭)
	void ScanLineFill(int x,int y, float r, float g, float b, float a);
    
    ///< whether really has alpha data, which means the information in the 4th channel is not all 255 or 1.0f
    bool hasReallyAlpha();
    
    bool saveToFile(FILE *fp);
    
    static CZImage* createFromFile(FILE *fp);
    
private:
	/// 修改
	void modifyData(int x,int y, float fillcolor[]);
	void modifyArea(CZImage * &backupImg,CZRect rect, float fillColor[]);
	/// 是否颜色相同
	inline bool isSameColorAt(int x,int y, float compareColor[]);
	/// 获取某点颜色
	bool getColorAt(int x, int y, float color[]);

public:
	int			width,height;
	void		*data;              ///< 具体类型由mode决定
    bool        hasAlpha;           /// whether has alpha data

private:
	StorageMode	mode;
	bool		*flag;              ///< 用于填充的辅助数组
};

#endif