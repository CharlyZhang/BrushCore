
///  \file CZUtil.cpp
///  \brief This is the file implement a series of utilities.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-11
///  \note

#include "CZUtil.h"
#include "path/CZBezierSegment.h"
#include "basic/CZ3DPoint.h"

using namespace std;

/// 不同颜色模式的转换
void CZUtil::HSVtoRGB(float h, float s, float v, float &r, float &g, float &b)
{
    if (s == 0)
    {
        r = g = b = v;
    }
    else
    {
        float   f,p,q,t;
        int     i;
        
        h *= 360;
        
        if (h == 360.0f)
        {
            h = 0.0f;
        }
        
        h /= 60;
        i = floor(h);
        
        f = h - i;
        p = v * (1.0 - s);
        q = v * (1.0 - (s*f));
        t = v * (1.0 - (s * (1.0 - f)));
        
        switch (i) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: r = v; g = p; b = q; break;
        }
    }
}
void CZUtil::RGBtoHSV(float r, float g, float b, float &h, float &s, float &v)
{
    float max = Max(r, Max(g, b));
    float min = Min(r, Min(g, b));
    float delta = max - min;
    
    v = max;
    s = (max != 0.0f) ? (delta / max) : 0.0f;
    
    if (s == 0.0f)
    {
        h = 0.0f;
    }
    else
    {
        if (r == max) {
            h = (g - b) / delta;
        } else if (g == max) {
            h = 2.0f + (b - r) / delta;
        } else if (b == max) {
            h = 4.0f + (r - g) / delta;
        }
        
        h *= 60.0f;
        
        if (h < 0.0f)
        {
            h += 360.0f;
        }
    }
    
    h /= 360.0f;
}

/// 生UUID
char* CZUtil::generateUUID()
{
    char *buf = new char[37];
    
    const char *c = "89ab";
    char *p = buf;
    int n;
    for( n = 0; n < 16; ++n )
    {
        int b = rand()%255;
        switch( n )
        {
            case 6:
                sprintf(p, "4%x", b%15 );
                break;
            case 8:
                sprintf(p, "%c%x", c[rand()%strlen(c)], b%15 );
                break;
            default:
                sprintf(p, "%02x", b);
                break;
        }
        
        p += 2;
        switch( n )
        {
            case 3:
            case 5:
            case 7:
            case 9:
                *p++ = '-';
                break;
        }
    }
    *p = 0;
    return buf;
}

/// 将一连串结点打散，相邻结点用三次贝塞尔曲线连接
///
///		两个结点（nodes）形成一根三次贝塞尔曲线，再将曲线打散成若干个绘制点（points）
///
///		/param nodes		- 连续的三维结点
///		/param points		- 离散后得到的绘制点容器
///		/return				- 离散后得到的绘制点数目
unsigned int CZUtil::flattenNodes2Points(const vector<CZBezierNode> &nodes, bool closed, vector<CZ3DPoint> &points)
{
    int numNodes = (int)nodes.size();
    
    points.clear();
    
    if (numNodes == 1)
    {
        CZBezierNode lonelyNode = nodes.back();
        points.push_back(lonelyNode.anchorPoint);
        return 1;
    }
    
    int numSegs = closed ? numNodes : numNodes - 1;
    
    CZBezierSegment   *segment = NULL;
    for (int i = 0; i < numSegs; i++)
    {
        CZBezierNode a = nodes[i];
        CZBezierNode b = nodes[(i+1) % numNodes];
        
        segment = CZBezierSegment::segmentBetweenNodes(a,b);
        segment->flattenIntoArray(points);
        delete segment;
    }
    
    return (unsigned int)points.size();
}

/// 判断是否支持深度颜色
bool CZUtil::canUseHDTexture()
{
    return true;
}

void CZUtil::CZCheckGLError_(const char *file, int line)
{
    int    retCode = 0;
    GLenum glErr = glGetError();
    
    while (glErr != GL_NO_ERROR)
    {
        
#ifdef USE_OPENGL
        const GLubyte* sError = gluErrorString(glErr);
        
        if (sError)
            printf("GL Error #%d (%s) in File %s at line: %d\n",glErr,gluErrorString(glErr),file,line);
        else
            printf("GL Error #%d (no message available) in File %s at line: %d\n",glErr,file,line);
        
#elif defined(USE_OPENGL_ES)
        printf("GL Error in File %s at line: %d\n",file,line);
        switch (glErr) {
            case GL_INVALID_ENUM:
                printf("GL Error: Enum argument is out of range\n");
                break;
            case GL_INVALID_VALUE:
                printf("GL Error: Numeric value is out of range\n");
                break;
            case GL_INVALID_OPERATION:
                printf("GL Error: Operation illegal in current state\n");
                break;
                //        case GL_STACK_OVERFLOW:
                //            NSLog(@"GL Error: Command would cause a stack overflow");
                //            break;
                //        case GL_STACK_UNDERFLOW:
                //            NSLog(@"GL Error: Command would cause a stack underflow");
                //            break;
            case GL_OUT_OF_MEMORY:
                printf("GL Error: Not enough memory to execute command\n");
                break;
            case GL_NO_ERROR:
                if (1) {
                    LOG_ERROR("No GL Error\n");
                }
                break;
            default:
                printf("Unknown GL Error\n");
                break;
        }
#endif
        
        retCode = 1;
        glErr = glGetError();
    }
    //return retCode;
};

/// 正弦函数,由[0,1]到[0,1] -CZFreehandTool类引用
float CZUtil::sineCurve(float input)
{
    float result;
    
    input *= M_PI; // move from [0.0, 1.0] tp [0.0, Pi]
    input -= M_PI_2; // shift back onto a trough
    
    result = sin(input) + 1; // add 1 to put in range [0.0,2.0]
    result /= 2; // back to [0.0, 1.0];
    
    return result;
}

#define BITMAPFILEHEADERLENGTH 14   // The bmp FileHeader length is 14
#define BM 19778                    // The ASCII code for BM
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
bool CZUtil::loadBMP(char* fileName,unsigned char* &buf,long &width,long &height)
{
    if (fileName == NULL)
    {
        LOG_ERROR("fileName is NULL!\n");
        return false;
    }
    
    FILE *fp = fopen(fileName, "rb");
    
    if (fp == NULL)
    {
        LOG_ERROR("Open bmp failed!\n");
        return false;
    }
    
    unsigned short bfType = 0;
    unsigned int OffSet = 0;    // OffSet from Header part to Data Part
    
    /// test if the file is BMP type
    fseek(fp, 0L, SEEK_SET);
    fread(&bfType, sizeof(unsigned char), 2, fp);
    if (BM != bfType)
    {
        LOG_ERROR("This file is not bmp file!\n");
        fclose(fp);
        return false;
    }
    
    /// get the offset of header to data part
    fseek(fp, 10L, SEEK_SET);
    fread(&OffSet, sizeof(unsigned char), 4, fp);
    /// get the width and height
    fseek(fp, 18L, SEEK_SET);
    fread(&width, sizeof(unsigned char), 4, fp);
    fseek(fp, 22L, SEEK_SET);
    fread(&height, sizeof(unsigned char), 4, fp);
    
    /// get the data
    fseek(fp, OffSet, SEEK_SET);
    
    long rowIdx;
    long row_size = width*3;
    
//    long widthDW = WIDTHBYTES(width * 24);
    
    buf = (unsigned char*)malloc(height*row_size*sizeof(unsigned char));
    
    for(long row = 0; row < height; row++)
    {
        rowIdx=(long)row*row_size;
        fread((void*)(buf+rowIdx),row_size,1,fp);
        
        for(long i = 0; i < width; i++)
        {
            unsigned char tmp = buf[rowIdx+i*3];
            buf[rowIdx+i*3] = buf[rowIdx+i*3+2];
            buf[rowIdx+i*3+2] = tmp;
        }
    }
    
    fclose(fp);
    
    return true;
}

/// 绘制矩形
void CZUtil::drawRect(const CZRect &rect, const CZAffineTransform &transform)
{
    CZ2DPoint corners[4];
    GLuint  vbo = 0;
    
    corners[0] = rect.origin;
    corners[1] = CZ2DPoint(rect.getMaxX(), rect.getMinY());
    corners[2] = CZ2DPoint(rect.getMaxX(), rect.getMaxY());
    corners[3] = CZ2DPoint(rect.getMinX(), rect.getMaxY());
    
    for (int i = 0; i < 4; i++) 
    {
        corners[i] = transform.applyTo2DPoint(corners[i]);
    }
    
    const GLfloat quadVertices[] = 
    {
        corners[0].x, corners[0].y, 0.0, 0.0,
        corners[1].x, corners[1].y, 1.0, 0.0,
        corners[3].x, corners[3].y, 0.0, 1.0,
        corners[2].x, corners[2].y, 1.0, 1.0,
    };
    
    // create, bind, and populate VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, quadVertices, GL_STATIC_DRAW);
    
    // set up attrib pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)8);
    glEnableVertexAttribArray(1);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDeleteBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


