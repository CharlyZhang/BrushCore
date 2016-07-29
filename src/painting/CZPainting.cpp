
///  \file CZPainting.cpp
///  \brief This is the file implements the class CZPainting.
///
///
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-26
///  \note

#include "CZPainting.h"
#include "../CZUtil.h"
#include "../CZCanvas.h"
#include "../CZActiveState.h"

#define MAX_THUMBNAIL_DIMMENSION 360

using namespace  std;

const int iMaxLayerNumber = 10;		///< 支持的最大图层数目


static std::string CZActiveLayerKey = "activeLayer";
static std::string CZWidthKey = "width";
static std::string CZHeightKey = "height";
static std::string CZLayersKey = "layers";

CZPainting::CZPainting(const CZSize &size, bool addDefaultLayer /* = true */)
{
    type = kNormalPainting;
    
    flattenMode = false;
    
    colors.clear();
    brushPtrs.clear();
    layers.clear();
    undoBrushPtrs.clear();
    strokeCount = 0;
    
    activeLayerInd = -1;
    ptrActivePath = NULL;
    ptrLastBrush = NULL;
    ptrCanvas = NULL;
    uuid = CZUtil::generateUUID();
    
    /// set up gl context
    glContext = new CZGLContext;
    glContext->setAsCurrent();
    loadShaders();
    fbo = new CZFbo;
    quadVAO = quadVBO = 0;
    activePaintTexture = NULL;
    brushStampTex = NULL;
    
    /// set the dimesion
    setDimensions(size);
    
    /// add the default blank layer
    if(addDefaultLayer) activeLayerInd = addNewLayer();
    else                activeLayerInd = -1;
    
    lastDeletedLayer = nullptr;
}
CZPainting::~CZPainting()
{
    colors.clear();
    brushPtrs.clear();
    undoBrushPtrs.clear();
    
    glContext->setAsCurrent();
    /// 删除着色器
    for(map<string,CZShader*>::iterator itr = shaders.begin(); itr!=shaders.end(); itr++)
        if(itr->second)
        {
            delete itr->second;
            itr->second = NULL;
        }
    
    shaders.clear();
    /// 删除图层
    for(vector<CZLayer*>::iterator itr = layers.begin(); itr != layers.end(); itr++)
        delete *itr;
    layers.clear();
    
    if (quadVBO) { glDeleteBuffers(1,&quadVBO); quadVBO = 0;}
    if (quadVAO) { GL_DEL_VERTEXARRAY(1, &quadVAO);	quadVAO = 0;}
    if (activePaintTexture) { delete activePaintTexture; activePaintTexture = NULL;}
    if (brushStampTex) { delete brushStampTex; brushStampTex = NULL;}
    if (fbo) { delete fbo; fbo = NULL;}
    if (lastDeletedLayer) { delete lastDeletedLayer; lastDeletedLayer = nullptr;}
    if (uuid)	{	delete []uuid; uuid = NULL;	}
    CZCheckGLError();
    
    delete glContext;
}

/// 将图像绘制出来（没绑定FBO）
void CZPainting::blit(CZMat4 &projection)
{
    if (flattenMode)
    {
        //[self blitFlattenedTexture:projection];
        return;
    }
    
    for (int idx = 0; idx < layers.size(); idx++)
    {
        CZLayer *layer = layers.at(idx);
        if (!layer->isVisible()) continue;
        
        if (activeLayerInd == idx && ptrActivePath)
        {
            if (ptrActivePath->action == CZPathActionErase)
                layer->blit(projection,activePaintTexture);
            else
                layer->blit(projection,activePaintTexture,ptrActivePath->color);
        } else
            layer->blit(projection);
    }
}

/// 生成所有图层的图像（不包括当前绘制的笔画）
CZImage *CZPainting::imageWithSize(CZSize &size, CZColor *backgroundColor /*= NULL*/)
{
    /// 获得运行所需要的数据
    int w = size.width;
    int h = size.height;
    CZMat4 projection;
    projection.SetOrtho(0,dimensions.width,0,dimensions.height,-1.0f,1.0f);
    
    /// 开始绘制
    glContext->setAsCurrent();
    
    fbo->setColorRenderBuffer(w,h);
    
    fbo->begin();
    
    // 用背景颜色清除缓存
    if (backgroundColor)
        glClearColor(backgroundColor->red, backgroundColor->green, backgroundColor->blue, backgroundColor->alpha);
    else
        glClearColor(0,0,0,0);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (vector<CZLayer*>::iterator itr = layers.begin(); itr != layers.end(); itr++)
    {
        CZLayer *layer = *itr;
        if (!layer->isVisible()) continue;
        
        layer->blit(projection);
    }
    
    CZImage *ret = fbo->produceImageForCurrentState();
    
    fbo->end();
    
    return ret;
}

/// 生成当前状态的图像
CZImage *CZPainting::imageForCurrentState(CZSize &size, CZColor *backgroundColor)
{
    int w = size.width;
    int h = size.height;
    
    glContext->setAsCurrent();
    
    fbo->setColorRenderBuffer(w, h);
    
    fbo->begin();
    
    // 用背景颜色清除缓存
    if (backgroundColor)
        glClearColor(backgroundColor->red, backgroundColor->green, backgroundColor->blue, backgroundColor->alpha);
    else
        glClearColor(0,0,0,0);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int idx = 0; idx < layers.size(); idx++)
    {
        CZLayer *layer = layers.at(idx);
        if (!layer->isVisible()) continue;
        
        if (activeLayerInd == idx && ptrActivePath)
        {
            if (ptrActivePath->action == CZPathActionErase)
                layer->blit(projMat,activePaintTexture);
            else
                layer->blit(projMat,activePaintTexture,*backgroundColor);
        } else
            layer->blit(projMat);
    }
    
    CZImage *ret = fbo->produceImageForCurrentState();
    
    fbo->end();
    
    return ret;
}

/// produce thumbnail image
CZImage* CZPainting::thumbnailImage(bool addPaintingPath /* = false*/)
{
    float aspectRatio = dimensions.width / dimensions.height;
    
    GLuint width,height;
    
    // figure out the width and height of the thumbnail
    if (aspectRatio > 1.0)
    {
        width = (GLuint) MAX_THUMBNAIL_DIMMENSION;
        height = floorf(1.0 / aspectRatio * width);
    }
    else
    {
        height = (GLuint) MAX_THUMBNAIL_DIMMENSION;
        width = floorf(aspectRatio * height);
    }
    
    float s = CZActiveState::getInstance()->mainScreenScale;
    width *= s;
    height *= s;
    
    CZSize size(width,height);
    CZImage *thumbnailImg = nullptr;
    if(addPaintingPath) thumbnailImg = imageForCurrentState(size);
    else                thumbnailImg = imageWithSize(size);
    return thumbnailImg;
}

/// 绘制一条轨迹（绘制到纹理）
CZRect CZPainting::paintStroke(CZPath *path_, CZRandom *randomizer, bool clearBuffer /* = false */)
{
    if (path_ == NULL || randomizer == NULL)
    {
        LOG_ERROR("either path or randomizer is null!\n");
        return CZRect();
    }
    
    ptrActivePath = path_;
    
    glContext->setAsCurrent();
    
    fbo->setTexture(activePaintTexture);
    
    // 开启fbo
    fbo->begin();
    
    if (clearBuffer)
    {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    configureBrush(path_->ptrBrush);
    
    // use shader program
    CZShader *shader = shaders["brush"];
    shader->begin();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brushStampTex->texId);
    
    glUniform1i(shader->getUniformLocation("texture"),0);
    glUniformMatrix4fv(shader->getUniformLocation("mvpMat"), 1, GL_FALSE, projMat);
    CZCheckGLError();
    
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    /// 绘制轨迹
    CZRect pathBounds = path_->paint(randomizer);
    
    shader->end();
    
    // 关闭启fbo
    fbo->end();
    
    if(ptrCanvas)   ptrCanvas->drawView();
    else            LOG_ERROR("ptrCanvas is NULL!\n");
    
    return pathBounds;
}

/// 设置范围
void CZPainting::setDimensions(const CZSize &size)
{
    if(dimensions == size) return;
    
    dimensions = size;
    projMat.SetOrtho(0,size.width,0,size.height,-1.0f,1.0f);
    
    glContext->setAsCurrent();
    if(activePaintTexture) delete activePaintTexture;
    activePaintTexture = generateTexture();
}

/// 设置当前激活图层
///
///		\param idx - 当前需要激活的图层序号
///		\ret	   - 原来激活的图层序号
int CZPainting::setActiveLayer(int idx)
{
    int oldIndex = activeLayerInd;
    
    if (idx < 0 || idx >= layers.size())
    {
        LOG_ERROR("idx is out of range\n");
        return oldIndex;
    }
    
    activeLayerInd = idx;
    
    return oldIndex;
    
}

/// 通过UID获取图层
CZLayer *CZPainting::layerWithUID(unsigned int uid_)
{
    // 	int num = layers.size();
    // 	for(int i = 0; i < num; i ++)
    // 		if(uid_ == layers[i]->getUID()) return layers[i];
    
    return NULL;
}

CZLayer *CZPainting::getLayer(int idx)
{
    if (idx < 0 || idx >= layers.size())
    {
        LOG_ERROR("idx is out of range\n");
        return NULL;
    }
    
    return layers[idx];
}

/// 添加新图层
///
///		\ret		 - 在所有图层中的序号。超过最大层数：-1，生成图层失败：-2
int CZPainting::addNewLayer()
{
    if(layers.size() > iMaxLayerNumber)
    {
        LOG_ERROR("painting has reached the max number of Layers\n");
        return -1;
    }
    
    CZLayer *layer = new CZLayer(this);
    if(layer == NULL)
    {
        LOG_ERROR("create layer failed\n");
        return -2;
    }
    
    int newIdx = activeLayerInd + 1;
    if(newIdx < 0 || newIdx > layers.size())
    {
        LOG_ERROR("newIdx is out of range\n");
        return -1;
    }
    
    layers.insert(layers.begin()+newIdx,layer);
    
    activeLayerInd = newIdx;
    
    return newIdx;
}

/// 删除图层
///
///		\note 当layer被锁住的时候不能被删除
bool CZPainting::deleteActiveLayer()
{
    // \note deleting is not allowed when there exist only one layer, for `restoreDeletedLayer`
    if (layers.size() <= 1) return false;
    
    CZLayer* layer = layers.at(activeLayerInd);
    bool needsAddDefaultLayer = false;
    
    if(layer->isLocked())
    {
        LOG_ERROR("activeLayer is locked\n");
        return false;
    }
    
    lastDeletedLayerIdx = activeLayerInd;
    
    if (layers.size()> 1)
    {
        // choose another layer to be active before we remove it
        if (activeLayerInd >= 1)	activeLayerInd --;
        else						activeLayerInd = 0;
    }
    else needsAddDefaultLayer = true;
    
    // do this before decrementing index
    for(vector<CZLayer*>::iterator itr=layers.begin(); itr!=layers.end(); itr++)
        if(*itr == layer)
        {
            delete lastDeletedLayer;
            lastDeletedLayer = layer;
            layers.erase(itr);
            break;
        }
    
    if (needsAddDefaultLayer)
    {
        activeLayerInd = -1;
        activeLayerInd = addNewLayer();
    }
    
    return true;
}

/// undo last deleted layer
bool CZPainting::restoreDeletedLayer()
{
    if (lastDeletedLayer == nullptr) return false;
    
    layers.insert(layers.begin()+lastDeletedLayerIdx,lastDeletedLayer);
    
    activeLayerInd = lastDeletedLayerIdx;
    
    lastDeletedLayer = nullptr;
    return true;
}

///
int CZPainting::duplicateActiveLayer()
{
    if(layers.size() > iMaxLayerNumber)
    {
        LOG_ERROR("painting has reached the max number of Layers\n");
        return -1;
    }
    
    CZLayer *layer = layers[activeLayerInd]->duplicate();
    if(layer == NULL)
    {
        LOG_ERROR("duplicate layer failed\n");
        return -2;
    }
    
    int newIdx = activeLayerInd + 1;
    if(newIdx < 0 || newIdx > layers.size())
    {
        LOG_ERROR("newIdx is out of range\n");
        return -1;
    }
    
    layers.insert(layers.begin()+newIdx,layer);
    
    activeLayerInd = newIdx;
    
    return newIdx;
}

/// 移动图层
///
///		\param fromIdx - 需要移动的图层序号
///		\param toIdx   - 移动到的位置
bool CZPainting::moveLayer(int fromIdx, int toIdx)
{
    if(fromIdx >= layers.size() || fromIdx < 0 ||
       toIdx  >= layers.size() || toIdx < 0)
    {
        LOG_ERROR("idx out of range!\n");
        return false;
    }
    
    if(fromIdx == toIdx) return true;
    
    CZLayer *layer = layers.at(fromIdx);
    
    /// remove from original pos
    for(vector<CZLayer*>::iterator itr=layers.begin(); itr!=layers.end(); itr++)
        if(*itr == layer)
        {
            layers.erase(itr);
            break;
        }
    /// move to the destination pos
    layers.insert(layers.begin()+toIdx,layer);
    
    activeLayerInd = toIdx;
    
    return true;
    
}

/// 向下合并当前图层
///
///		\ret - 是否合并成功
bool CZPainting::mergeActiveLayerDown()
{
    /// in case the active layer is at bottom
    if(activeLayerInd <= 0)
    {
        LOG_ERROR("active layer is NULL or at bottom\n");
        return false;
    }
    
    /// in case there's only one layer
    if(layers.size() <= 1)
    {
        LOG_ERROR("layers number is no more than 1\n");
        return false;
    }
    
    CZLayer *bottomLayer = layers[activeLayerInd - 1];
    
    /// in case the layer to merged with is not editable
    if(!bottomLayer->isEditable())
    {
        LOG_ERROR("layer to be merged with is not editable\n");
        return false;
    }
    
    CZLayer *activeLayer = layers.at(activeLayerInd);
    bool ret = bottomLayer->merge(activeLayer);
    deleteActiveLayer();
    
    return ret;
}


/// 设置激活轨迹
void CZPainting::setActivePath(CZPath *path)
{
    ptrActivePath = path;
}

/// 获取激活轨迹
CZPath* CZPainting::getActivePath()
{
    return ptrActivePath;
}

/// 获取激活图层
CZLayer* CZPainting::getActiveLayer()
{
    return layers.at(activeLayerInd);
}

/// get layer number
int CZPainting::getLayersNumber()
{
    return (int)layers.size();
}

/// 获取着色器
CZShader* CZPainting::getShader(string name)
{
    glContext->setAsCurrent();
    map<string,CZShader*>::iterator itr = shaders.find(name);
    if (itr == shaders.end())
    {
        return NULL;
    }
    
    return itr->second;
}

///
CZTexture* CZPainting::generateTexture(CZImage* img /* = NULL */)
{
    glContext->setAsCurrent();
    CZTexture *ret = NULL;
    if (img)	ret = CZTexture::produceFromImage(img);
    else		ret = new CZTexture(dimensions.width,dimensions.height);
    
    ret->enableLinearInterprolation(false);
    return ret;
}

/// 返回quadVAO
GLuint CZPainting::getQuadVAO()
{
    if(!quadVAO)
    {
        glContext->setAsCurrent();
        
        const GLfloat vertices[] =
        {
            0.0, 0.0, 0.0, 0.0,
            (GLfloat)dimensions.width, 0.0, 1.0, 0.0,
            0.0, (GLfloat)dimensions.height, 0.0, 1.0,
            (GLfloat)dimensions.width, (GLfloat)dimensions.height, 1.0, 1.0,
        };
        
        GL_GEN_VERTEXARRAY(1,&quadVAO);
        GL_BIND_VERTEXARRAY(quadVAO);
        // create, bind, and populate VBO
        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, vertices, GL_STATIC_DRAW);
        
        // set up attrib pointers
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)8);
        glEnableVertexAttribArray(1);
        
        glBindBuffer(GL_ARRAY_BUFFER,0);
        
        GL_BIND_VERTEXARRAY(0);
    }
    
    return quadVAO;
}

/// 获取范围
CZSize& CZPainting::getDimensions()
{
    return dimensions;
}

/// 获取绘制矩形
CZRect& CZPainting::getBounds()
{
    bounds = CZRect(0,0,dimensions.width,dimensions.height);
    return bounds;
}

/// 获取gl上下文
CZGLContext *CZPainting::getGLContext()
{
    return glContext;
};

/// set canvas
bool CZPainting::setCanvas(CZCanvas* c)
{
    ptrCanvas = c;
    return true;
}

bool CZPainting::shouldPreventPaint()
{
    CZLayer *layer = getActiveLayer();
//    printf("isLocked:%d isVisible:%d",layer->isLocked(),layer->isVisible());
    return layer->isLocked() || !layer->isVisible();
}

/// pick the color
CZColor CZPainting::pickColor(int x, int y)
{
    if (x<0 || x>= dimensions.width || y<0 || y>= dimensions.height) {
        LOG_WARN("position(%d,%d) is out of dimensions of painting.\n",x,y);
        return CZColor::blackColor();
    }
    
    glContext->setAsCurrent();
    
    fbo->setColorRenderBuffer(dimensions.width, dimensions.height);
    
    fbo->begin();
    
    // 用背景颜色清除缓存
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT );
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    blit(projMat);
    
    CZColor ret = fbo->getColor(x, y);
    
    fbo->end();
    
    return ret;
}

/// restore the painting
bool CZPainting::restore(bool addDefaultLayer /* = true */)
{
    type = kNormalPainting;
    
    for(vector<CZLayer*>::iterator itr = layers.begin(); itr != layers.end(); itr++)    delete *itr;
    layers.clear();
    activeLayerInd = -1;
    if (lastDeletedLayer) { delete lastDeletedLayer; lastDeletedLayer = nullptr;}
    if(addDefaultLayer) addNewLayer();
    
    glContext->setAsCurrent();
    fbo->setTexture(activePaintTexture);
    fbo->begin();
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    fbo->end();
    
    if (addDefaultLayer)
    {
        CZLayer *layer = getActiveLayer();
        return layer->clear();
    }
    
    return true;
}

/// 实现CZCoding接口
void CZPainting::update(CZDecoder *decoder_, bool deep /*= false*/)
{
    
};
void CZPainting::encode(CZCoder *coder_, bool deep /*= false*/)
{
    if(coder_ == nullptr)
    {
        LOG_ERROR("coder is NULL\n");
        return;
    }
    coder_->encodeUint((unsigned int)activeLayerInd, CZActiveLayerKey);
    coder_->encodeFloat(dimensions.width, CZWidthKey);
    coder_->encodeFloat(dimensions.height, CZHeightKey);
    if (deep)
    {
        // layers
        vector<CZCoding*> tempArray;
        for (vector<CZLayer*>::iterator itr = layers.begin(); itr != layers.end(); itr++)
        {
            tempArray.push_back(*itr);
        }
        
        coder_->encodeArray(tempArray, CZLayersKey);
    }
    
};

void CZPainting::loadShaders()
{
#if 0
    NSString        *shadersJSONPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"Shaders.json"];
    NSData          *JSONData = [NSData dataWithContentsOfFile:shadersJSONPath];
    NSError         *error = nil;
    NSDictionary    *shaderDict = [NSJSONSerialization JSONObjectWithData:JSONData options:0 error:&error];
    
    if (!shaderDict) {
        WDLog(@"Error loading 'Shaders.json': %@", error);
        return;
    }
    
    NSMutableDictionary *tempShaders = [NSMutableDictionary dictionary];
    
    for (NSString *key in shaderDict.keyEnumerator) {
        NSDictionary *description = shaderDict[key];
        NSString *vertex = description[@"vertex"];
        NSString *fragment = description[@"fragment"];
        NSArray *attributes = description[@"attributes"];
        NSArray *uniforms = description[@"uniforms"];
        
        WDShader *shader = [[WDShader alloc] initWithVertexShader:vertex
                                                   fragmentShader:fragment
                                                  attributesNames:attributes
                                                     uniformNames:uniforms];
        tempShaders[key] = shader;
    }
    WDCheckGLError();
    
    shaders = tempShaders;
#endif
    /// 笔刷shader
    vector<string> attributes;
    attributes.push_back("inPosition");
    attributes.push_back("inTexcoord");
    attributes.push_back("alpha");
    vector<string> uniforms;
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    
    CZShader *shader = new CZShader("brush","brush",attributes,uniforms);
    shaders.insert(make_pair("brush",shader));
    
    /// 将图层和绘制笔画输出到屏幕
    attributes.clear();
    attributes.push_back("inPosition");
    attributes.push_back("inTexcoord");
    uniforms.clear();
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    uniforms.push_back("opacity");
    uniforms.push_back("mask");
    uniforms.push_back("color");
    uniforms.push_back("lockAlpha");
    
    shader = new CZShader("blit","blitWithMask",attributes,uniforms);
    shaders.insert(make_pair("blitWithMask",shader));
    
    /// 将图层和擦除轨迹输出
    uniforms.pop_back();
    uniforms.pop_back();
    
    shader = new CZShader("blit","blitWithEraseMask",attributes,uniforms);
    shaders.insert(make_pair("blitWithEraseMask",shader));
    
    /// 将图层纹理绘制出来
    uniforms.clear();
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    uniforms.push_back("opacity");
    
    shader = new CZShader("blit","blit",attributes,uniforms);
    shaders.insert(make_pair("blit",shader));
    
    /// 合并绘制笔画到图层
    uniforms.clear();
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    uniforms.push_back("mask");
    uniforms.push_back("color");
    uniforms.push_back("lockAlpha");
    
    shader = new CZShader("blit","compositeWithMask",attributes,uniforms);
    shaders.insert(make_pair("compositeWithMask",shader));
    
    /// 合并擦除笔画到图层
    uniforms.clear();
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    uniforms.push_back("mask");
    uniforms.push_back("lockAlpha");
    
    shader = new CZShader("blit","compositeWithEraseMask",attributes,uniforms);
    shaders.insert(make_pair("compositeWithEraseMask",shader));
    
    /// nonPremultipliedBlit
    attributes.clear();
    attributes.push_back("inPosition");
    attributes.push_back("inTexcoord");
    uniforms.clear();
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    uniforms.push_back("opacity");
    
    shader = new CZShader("blit","nonPremultipliedBlit",attributes,uniforms);
    shaders.insert(make_pair("nonPremultipliedBlit", shader));
    
    ///  unPremultipliedBlit
    shader = new CZShader("blit","unPremultipliedBlit",attributes,uniforms);
    shaders.insert(make_pair("unPremultipliedBlit", shader));
    
    /// simple blit
    attributes.clear();
    attributes.push_back("inPosition");
    uniforms.clear();
    uniforms.push_back("mvpMat");
    uniforms.push_back("color");
    shader = new CZShader("simple","simple",attributes,uniforms);
    shaders.insert(make_pair("simple", shader));
    
    CZCheckGLError();
}

/// 配置画刷
void CZPainting::configureBrush(CZBrush* brush_)
{
    if(brush_ != ptrLastBrush)
    {
        ptrLastBrush = brush_;
        
        if (brushStampTex) { delete brushStampTex; brushStampTex = NULL;}
        
        CZImage *img = brush_->getStampImage();
        glContext->setAsCurrent();
        brushStampTex = CZTexture::produceFromImage(img);
        
    }
}
