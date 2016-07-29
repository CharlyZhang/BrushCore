
///  \file CZTexture.h
///  \brief This is the file declare the Class CZTexture.
///
///		This file includes CZTexture, and all are about 2D.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-19
///  \note

#ifndef _CZTEXTURE_H_
#define _CZTEXTURE_H_

#include "../basic/CZImage.h"
#include "../CZDefine.h"

class CZTexture
{
public:
	CZTexture(int width_, int height_, StorageMode mode_ = DEFAULT_STORAGEMODE, void *data = 0);
	~CZTexture();
	/// 由CZImage生成纹理
	static CZTexture* produceFromImage(CZImage *img);
    
	/// 用图片数据修改纹理
    //  \note w or h = - 1 means using the width or height of img
	bool modifyWith(CZImage *img,int x = 0, int y = 0, int w = -1, int h = -1);

	/// 获取图片模式
	StorageMode getMode() {return mode;};

	/// 获取其对应的图像数据
	CZImage *toImage();
	/// 开关线性差值
	void enableLinearInterprolation(bool flag);

public:
	unsigned int texId;
	int width,height;
	StorageMode mode;
};
#endif