
///  \file CZTexture.cpp
///  \brief This is the file implement the Class CZImage.
///
///		This file includes CZImage.
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-09
///  \note

#include "CZImage.h"
#include <string.h>
#include <queue>
#include <stack>
#include <cmath>

using namespace std;

typedef struct Position {int x,y; Position(int x_, int y_) {x = x_; y = y_;}} Position;

CZImage::CZImage(int w_/* =0 */, int h_/* =0 */, StorageMode mode_ /* = DEFAULT_STORAGEMODE */, const void* data_ /*= NULL*/)
{
	width = w_;
	height = h_;
	mode = mode_;

	int n, type = 0;
	switch (mode)
	{
	case RGB_BYTE:		n = 3;	type = 0;	break;
	case RGB_FLOAT:		n = 3;	type = 1;	break;
	case RGBA_BYTE:		n = 4;	type = 0;	break;
	case RGBA_FLOAT:	n = 4;	type = 1;	break;
	default:
		LOG_ERROR("ImageMode is illegal!\n");
		n = 0;
	}
    
	if(type == 0)
	{
		data = (void*) new unsigned char[n*width*height];
		if(data_)	memcpy(data,data_,width*height*n*sizeof(unsigned char));
	}
	else if(type ==1)
	{
		data = (void*) new float[n*width*height];
		if(data_)	memcpy(data,data_,width*height*n*sizeof(float));
	}
	else
		data = NULL;
    
    hasAlpha = false;
}

CZImage::~CZImage()
{
	if(data != NULL)
	{
        switch (mode)
        {
            case RGB_BYTE:
            case RGBA_BYTE:
                delete [] (unsigned char*)data;
                break;
            case RGB_FLOAT:
            case RGBA_FLOAT:
                delete [] (float*)data;
                break;
            default:
                break;
        }

		data = NULL;
	}
}

CZImage* CZImage::copy()
{
    CZImage *ret = new CZImage(width, height, mode, data);
    return ret;
}

void CZImage::ScanLineFill(int x,int y, float r, float g, float b, float a)

{   
	int xl,xr;

	bool spanNeedFill;

	stack<Position> Stack;
	float compareColor[4],fillColor[4];
	getColorAt(x,y,compareColor);
	fillColor[0] = r; fillColor[1] = g; fillColor[2] = b; fillColor[3] = a;

	Stack.push(Position(x,y));

	while(!Stack.empty())
	{ 
		Position pos = Stack.top();
		Stack.pop();

		x = pos.x;	y = pos.y;
		while(x < width && isSameColorAt(x,y,compareColor)) //向右填充
		{ 
			modifyData(x,y,fillColor);
			x++;
		}

		xr = x-1;

		x = pos.x-1;
		while(x >= 0 && isSameColorAt(x,y,compareColor)) //向左填充
		{ 
			modifyData(x,y,fillColor);
			x--;
		}

		xl = x+1;

		//处理上面一条扫描线
		x = xl;	y = y+1;
		while(y<height && x<=xr)
		{  
			spanNeedFill=false;
			while(x < xr && isSameColorAt(x,y,compareColor))
			{ 
				spanNeedFill=true;
				x++;
			}

			if(spanNeedFill)
			{
				if(isSameColorAt(x,y,compareColor) && x==xr)	Stack.push(Position(x,y));
				else											Stack.push(Position(x-1,y));
				spanNeedFill=false;
			}

			int xspan = 0;
			while(xspan+x<=xr)
			while(x<=xr && !isSameColorAt(x,y,compareColor)) x++;
		}//End of while(i<xr)

		//处理下面一条扫描线，代码与处理上面一条扫描线类似
		x = xl;	y = y-2;
		while(y>=0 && x<=xr)
		{
			spanNeedFill=false;
			while(x < xr && isSameColorAt(x,y,compareColor))
			{ 
				spanNeedFill=true;
				x++;
			}

			if(spanNeedFill)
			{
				if(isSameColorAt(x,y,compareColor) && x==xr)	Stack.push(Position(x,y));
				else										Stack.push(Position(x-1,y));
				spanNeedFill=false;
			}

			while(x<=xr && isSameColorAt(x,y,compareColor)) x++;
		}//End of while(i<xr)

	}//End of while(!isstackempty())

}

/// check whether has really alpha data
bool CZImage::hasReallyAlpha()
{
    if (!hasAlpha) {
        return false;
    }
    
    bool reallyHasAlpha = false;
    unsigned int rowOffset = 0;
    int rowByteSize = width * 4;
    
    if (mode == RGBA_BYTE) {
        unsigned char *values = (unsigned char*)data;
        for (int r = 0; (r < height) && !reallyHasAlpha; r++)
        {
            for (int c = 3; c < rowByteSize; c += 4)
            {
                if (values[rowOffset + c] < 255) {
                    reallyHasAlpha = true;
                    break;
                }
            }
            rowOffset += rowByteSize;
        }
    }
    else if (mode == RGBA_FLOAT){
        float *values = (float*)data;
        for (int r = 0; (r < height) && !reallyHasAlpha; r++)
        {
            for (int c = 3; c < rowByteSize; c += 4)
            {
                if (values[rowOffset + c] < 1.0f) {
                    reallyHasAlpha = true;
                    break;
                }
            }
            rowOffset += rowByteSize;
        }
    }
    
    return reallyHasAlpha;
}

bool CZImage::saveToFile(FILE *fp)
{
    if (fp == nullptr)
    {
        LOG_ERROR("fp is NULL\n");
        return false;
    }
    
    fwrite((void*)&width, sizeof(int), 1, fp);
    fwrite((void*)&height, sizeof(int), 1, fp);
    fwrite((void*)&mode, sizeof(StorageMode), 1, fp);
    
    int n, type = 0;
    switch (mode)
    {
        case RGB_BYTE:		n = 3;	type = 0;	break;
        case RGB_FLOAT:		n = 3;	type = 1;	break;
        case RGBA_BYTE:		n = 4;	type = 0;	break;
        case RGBA_FLOAT:	n = 4;	type = 1;	break;
        default:
            LOG_ERROR("ImageMode is illegal!\n");
            n = 0;
    }
    
    if(type == 0)       fwrite(data, sizeof(unsigned char), width*height*n, fp);
    else if(type ==1)   fwrite(data, sizeof(float), width*height*n, fp);
   
    return true;
}

CZImage* CZImage::createFromFile(FILE *fp)
{
    if (fp == nullptr)
    {
        LOG_ERROR("fp is NULL\n");
        return nullptr;
    }
    
    int w,h;
    fread((void*)&w, sizeof(int), 1, fp);
    fread((void*)&h, sizeof(int), 1, fp);
    StorageMode m;
    fread((void*)&m, sizeof(StorageMode), 1, fp);
    
    int n, type = 0;
    void *d;
    switch (m)
    {
        case RGB_BYTE:		n = 3;	type = 0;	break;
        case RGB_FLOAT:		n = 3;	type = 1;	break;
        case RGBA_BYTE:		n = 4;	type = 0;	break;
        case RGBA_FLOAT:	n = 4;	type = 1;	break;
        default:
            LOG_ERROR("ImageMode is illegal!\n");
            n = 0;
    }
    
    if(type == 0)
    {
        d = (void*) new unsigned char[n*w*h];
        fread(d, sizeof(unsigned char), w*h*n, fp);
    }
    else if(type ==1)
    {
        d = (void*) new float[n*w*h];
        fread(d, sizeof(float), w*h*n, fp);
    }
    
    CZImage *newImage = new CZImage(w,h,m,d);
    
    if (type == 0) delete [] (unsigned char*)d;
    else           delete [] (float*)d;
    
    return newImage;
}

/// 填充（采用广搜）
CZImage* CZImage::modifyDataFrom1(int x,int y, float r, float g, float b, float a, CZRect &modifiedRect)
{
	//ScanLineFill(x,y,r,g,b,a);
	//return true;
	int minX,minY,maxX,maxY;
	queue<Position> myQueue;
	float compareColor[4],fillColor[4];
	getColorAt(x,y,compareColor);
	fillColor[0] = r; fillColor[1] = g; fillColor[2] = b; fillColor[3] = a;

	flag = new bool[width*height];		///< 表示可以修改的像素
	memset(flag,0,width*height*sizeof(bool));

	for(int i=0; i<width; i++)
		for(int j=0; j<height; j++){
			if (isSameColorAt(i,j,compareColor)) flag[j*width+i] = true;
		}

	minX = maxX = x;
	minY = maxY = y;

	myQueue.push(Position(x,y));
	modifyData(x,y,fillColor);
	flag[y*width+x] = false;
	static long step = 0;
	while(!myQueue.empty())
	{
		step ++;
		Position pos = myQueue.front();	myQueue.pop();
	
		for(int i=pos.x+1; i<width; i++)	
			if(flag[pos.y*width+i]) 
			{
				modifyData(i,pos.y,fillColor);
				flag[pos.y*width+i] = false;
				myQueue.push(Position(i,pos.y));
				maxX = i;
			}
			else break;
		for(int i=pos.x-1; i>=0;	i--)
			if(flag[pos.y*width+i])
			{
				modifyData(i,pos.y,fillColor);
				flag[pos.y*width+i] = false;
				myQueue.push(Position(i,pos.y));
				minX = i;
			}
			else break;
		for(int j=pos.y+1; j<height;j++)
			if(flag[j*width+pos.x])
			{
				modifyData(pos.x,j,fillColor);
				flag[j*width+pos.x] = false;
				myQueue.push(Position(pos.x,j));
				maxY = j;
			}
			else break;
		for(int j=pos.y-1; j>=0;j--)
			if(flag[j*width+pos.x])
			{
				modifyData(pos.x,j,fillColor);
				flag[j*width+pos.x] = false;
				myQueue.push(Position(pos.x,j));
				minY = j;
			}
			else break;
	}

	LOG_DEBUG("total steps %ld\n",step);
	delete [] flag;

	CZImage *ret = NULL;
	modifiedRect.origin = CZ2DPoint(minX,minY);
	modifiedRect.size = CZSize(maxX-minX, maxY-minY);
	return ret;
}

CZImage* CZImage::modifyDataFrom(int x,int y, float r, float g, float b, float a, CZRect &modifiedRect)
{
	//ScanLineFill(x,y,r,g,b,a);
	//return true;
	int minX,minY,maxX,maxY;
	queue<Position> myQueue;
	float compareColor[4],fillColor[4];
	getColorAt(x,y,compareColor);
	fillColor[0] = r; fillColor[1] = g; fillColor[2] = b; fillColor[3] = a;

	flag = new bool[width*height];		///< 表示是否修改过的像素
	memset(flag,0,width*height*sizeof(bool));

	minX = maxX = x;
	minY = maxY = y;

	myQueue.push(Position(x,y));
	modifyData(x,y,fillColor);
	flag[y*width+x] = true;
	static long step = 0;

	while(!myQueue.empty())
	{
		step ++;
		Position pos = myQueue.front();	myQueue.pop();

		for(int i=pos.x+1; i<width; i++)	
			if(!flag[pos.y*width+i] && isSameColorAt(i,pos.y,compareColor)) 
			{
				//modifyData(i,pos.y,fillColor);
				flag[pos.y*width+i] = true;
				myQueue.push(Position(i,pos.y));
				if(i>maxX)	maxX = i;
			}
			else break;
		for(int i=pos.x-1; i>=0;	i--)
			if(!flag[pos.y*width+i] && isSameColorAt(i,pos.y,compareColor))
			{
				//modifyData(i,pos.y,fillColor);
				flag[pos.y*width+i] = true;
				myQueue.push(Position(i,pos.y));
				if(i<minX)	minX = i;
			}
			else break;
		for(int j=pos.y+1; j<height;	j++)
			if(!flag[j*width+pos.x] && isSameColorAt(pos.x,j,compareColor))
			{
				//modifyData(pos.x,j,fillColor);
				flag[j*width+pos.x] = true;
				myQueue.push(Position(pos.x,j));
				if(j>maxY)	maxY = j;
			}
			else break;
		for(int j=pos.y-1; j>=0;	j--)
			if(!flag[j*width+pos.x] && isSameColorAt(pos.x,j,compareColor))
			{
				//modifyData(pos.x,j,fillColor);
				flag[j*width+pos.x] = true;
				myQueue.push(Position(pos.x,j));
				if(j<minY)	minY = j;
			}
			else break;
	}


	LOG_DEBUG("total steps %ld\n",step);

	modifiedRect.origin = CZ2DPoint(minX,minY);
	modifiedRect.size = CZSize(maxX-minX+1, maxY-minY+1);

	CZImage *ret = NULL;
	modifyArea(ret,modifiedRect,fillColor);
	
	delete [] flag;		///< ! used in modifyArea()
	return ret;
}

/// 修改
void CZImage::modifyArea(CZImage * &backupImg,CZRect rect, float fillColor[])
{
	int x = rect.origin.x;
	int y = rect.origin.y;
	int w = rect.size.width;
	int h = rect.size.height;

	int n, type = 0;
	switch (mode)
	{
	case RGB_BYTE:		n = 3;	type = 0;	break;
	case RGB_FLOAT:		n = 3;	type = 1;	break;
	case RGBA_BYTE:		n = 4;	type = 0;	break;
	case RGBA_FLOAT:	n = 4;	type = 1;	break;
	default:
		LOG_ERROR("ImageMode is illegal!\n");
		n = 0;
	}

	if(type == 0)
	{	/// unsigned char
		unsigned char *newData = new unsigned char[n*w*h];
		unsigned char *backupData = new unsigned char[n*w*h];
		unsigned char *originalData = (unsigned char*)data;
		unsigned char *colors = new unsigned char[n];
		for(int i=0; i<n; i++) colors[i] = (unsigned char)(fillColor[i]*255);

		for(int i=0; i<h; i++)
		{
			memcpy((void*)(newData+i*w*n),(void*)(originalData+((i+y)*width+x)*n),w*n*sizeof(unsigned char));
			memcpy((void*)(backupData+i*w*n),(void*)(originalData+((i+y)*width+x)*n),w*n*sizeof(unsigned char));

			for(int j=0; j<w; j++)
			{
				if(flag[(i+y)*width+x+j])
					for(int k=0; k<n; k++)	newData[(i*w+j)*n+k] = colors[k];
			}
		}

		delete [] (unsigned char*)data;
		data = (void*)newData;
		width = w; height = h;
		
		backupImg = new CZImage(w,h,mode,(void*)backupData);
		delete [] backupData;
		delete [] colors;
	}
	else if(type ==1)
	{	/// float
		float *newData = new float[n*w*h];
		float *backupData = new float[n*w*h];
		float *originalData = (float*)data;
		float *colors = new float[n];
		for(int i=0; i<n; i++) colors[i] = fillColor[i];

		long step = 0;
		for(int i=0; i<h; i++)
		{
			memcpy((void*)(&newData[i*w*n]),(void*)(&originalData[((i+y)*width+x)*n]),w*n*sizeof(float));
			memcpy((void*)(&backupData[i*w*n]),(void*)(&originalData[((i+y)*width+x)*n]),w*n*sizeof(float));

			for(int j=0; j<w; j++)
			{
				if(flag[(i+y)*width+x+j])
				{
					for(int k=0; k<n; k++)	newData[(i*w+j)*n+k] = colors[k];
					step ++;
				}
			}
		}

		LOG_DEBUG("new step is %ld\n",step);

		delete [] (float*)data;
		data = (void*)newData;
		width = w; height = h;

		backupImg = new CZImage(w,h,mode,(void*)backupData);
		delete [] backupData;
		delete [] colors;
	}
}

void CZImage::modifyData(int x,int y, float fillcolor[])
{
	float			*f_data = NULL;
	unsigned char	*b_data = NULL;

	switch (mode)
	{
	case RGB_BYTE:	
		b_data = &((unsigned char*)data)[(y*width+x)*3];
		for(int i=0; i<3; i++) b_data[i] = (unsigned char)(fillcolor[i]*255);
		break;
	case RGB_FLOAT:
		f_data = &((float*)data)[(y*width+x)*3];
		for(int i=0; i<3; i++) f_data[i] = fillcolor[i];
		break;
	case RGBA_BYTE:		
		b_data = &((unsigned char*)data)[(y*width+x)*4];
		for(int i=0; i<4; i++) b_data[i] = (unsigned char)(fillcolor[i]*255);
		break;
	case RGBA_FLOAT:
		f_data = &((float*)data)[(y*width+x)*4];
		for(int i=0; i<4; i++) f_data[i] = fillcolor[i];
		break;
	default:
		LOG_ERROR("ImageMode is illegal!\n");
	}
}

static float	epslon = 1.0f / 256;
/// 是否颜色相同
bool CZImage::isSameColorAt(int x,int y, float compareColor[])
{
	float	color[4];
	
	getColorAt(x,y,color);

	for(int i=0; i<3; i++)						if(fabs(color[i]-compareColor[i]) >= epslon) return false;
	if(mode == RGBA_FLOAT || mode == RGBA_BYTE) if(fabs(color[3]-compareColor[3]) >= epslon) return false;
	return true;
}

/// 获取某点颜色
bool CZImage::getColorAt(int x, int y, float color[])
{
	float			*f_data = NULL;
	unsigned char	*b_data = NULL;

	switch (mode)
	{
	case RGB_BYTE:	
		b_data = &((unsigned char*)data)[(y*width+x)*3];
		for(int i=0; i<3; i++)	color[i] = b_data[i] / 255.0f;
		color[3] = 0.0f;
		break;
	case RGB_FLOAT:
		f_data = &((float*)data)[(y*width+x)*3];
		for(int i=0; i<3; i++)	color[i] = f_data[i];
		color[3] = 0.0f;
		break;
	case RGBA_BYTE:		
		b_data = &((unsigned char*)data)[(y*width+x)*4];
		for(int i=0; i<4; i++)	color[i] = b_data[i] / 255.0f;
		break;
	case RGBA_FLOAT:
		f_data = &((float*)data)[(y*width+x)*4];
		for(int i=0; i<4; i++)	color[i] = f_data[i];
		break;
	default:
		LOG_ERROR("ImageMode is illegal!\n");
		return false;
	}

	return true;
}