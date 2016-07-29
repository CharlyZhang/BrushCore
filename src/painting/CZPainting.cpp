
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

const int iMaxLayerNumber = 10;		///< ֧�ֵ����ͼ����Ŀ


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
    /// ɾ����ɫ��
    for(map<string,CZShader*>::iterator itr = shaders.begin(); itr!=shaders.end(); itr++)
        if(itr->second)
        {
            delete itr->second;
            itr->second = NULL;
        }
    
    shaders.clear();
    /// ɾ��ͼ��
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

/// ��ͼ����Ƴ�����û��FBO��
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

/// ��������ͼ���ͼ�񣨲�������ǰ���Ƶıʻ���
CZImage *CZPainting::imageWithSize(CZSize &size, CZColor *backgroundColor /*= NULL*/)
{
    /// �����������Ҫ������
    int w = size.width;
    int h = size.height;
    CZMat4 projection;
    projection.SetOrtho(0,dimensions.width,0,dimensions.height,-1.0f,1.0f);
    
    /// ��ʼ����
    glContext->setAsCurrent();
    
    fbo->setColorRenderBuffer(w,h);
    
    fbo->begin();
    
    // �ñ�����ɫ�������
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

/// ���ɵ�ǰ״̬��ͼ��
CZImage *CZPainting::imageForCurrentState(CZSize &size, CZColor *backgroundColor)
{
    int w = size.width;
    int h = size.height;
    
    glContext->setAsCurrent();
    
    fbo->setColorRenderBuffer(w, h);
    
    fbo->begin();
    
    // �ñ�����ɫ�������
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

/// ����һ���켣�����Ƶ�����
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
    
    // ����fbo
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
    /// ���ƹ켣
    CZRect pathBounds = path_->paint(randomizer);
    
    shader->end();
    
    // �ر���fbo
    fbo->end();
    
    if(ptrCanvas)   ptrCanvas->drawView();
    else            LOG_ERROR("ptrCanvas is NULL!\n");
    
    return pathBounds;
}

/// ���÷�Χ
void CZPainting::setDimensions(const CZSize &size)
{
    if(dimensions == size) return;
    
    dimensions = size;
    projMat.SetOrtho(0,size.width,0,size.height,-1.0f,1.0f);
    
    glContext->setAsCurrent();
    if(activePaintTexture) delete activePaintTexture;
    activePaintTexture = generateTexture();
}

/// ���õ�ǰ����ͼ��
///
///		\param idx - ��ǰ��Ҫ�����ͼ�����
///		\ret	   - ԭ�������ͼ�����
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

/// ͨ��UID��ȡͼ��
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

/// �����ͼ��
///
///		\ret		 - ������ͼ���е���š�������������-1������ͼ��ʧ�ܣ�-2
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

/// ɾ��ͼ��
///
///		\note ��layer����ס��ʱ���ܱ�ɾ��
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

/// �ƶ�ͼ��
///
///		\param fromIdx - ��Ҫ�ƶ���ͼ�����
///		\param toIdx   - �ƶ�����λ��
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

/// ���ºϲ���ǰͼ��
///
///		\ret - �Ƿ�ϲ��ɹ�
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


/// ���ü���켣
void CZPainting::setActivePath(CZPath *path)
{
    ptrActivePath = path;
}

/// ��ȡ����켣
CZPath* CZPainting::getActivePath()
{
    return ptrActivePath;
}

/// ��ȡ����ͼ��
CZLayer* CZPainting::getActiveLayer()
{
    return layers.at(activeLayerInd);
}

/// get layer number
int CZPainting::getLayersNumber()
{
    return (int)layers.size();
}

/// ��ȡ��ɫ��
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

/// ����quadVAO
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

/// ��ȡ��Χ
CZSize& CZPainting::getDimensions()
{
    return dimensions;
}

/// ��ȡ���ƾ���
CZRect& CZPainting::getBounds()
{
    bounds = CZRect(0,0,dimensions.width,dimensions.height);
    return bounds;
}

/// ��ȡgl������
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
    
    // �ñ�����ɫ�������
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

/// ʵ��CZCoding�ӿ�
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
    /// ��ˢshader
    vector<string> attributes;
    attributes.push_back("inPosition");
    attributes.push_back("inTexcoord");
    attributes.push_back("alpha");
    vector<string> uniforms;
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    
    CZShader *shader = new CZShader("brush","brush",attributes,uniforms);
    shaders.insert(make_pair("brush",shader));
    
    /// ��ͼ��ͻ��Ʊʻ��������Ļ
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
    
    /// ��ͼ��Ͳ����켣���
    uniforms.pop_back();
    uniforms.pop_back();
    
    shader = new CZShader("blit","blitWithEraseMask",attributes,uniforms);
    shaders.insert(make_pair("blitWithEraseMask",shader));
    
    /// ��ͼ��������Ƴ���
    uniforms.clear();
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    uniforms.push_back("opacity");
    
    shader = new CZShader("blit","blit",attributes,uniforms);
    shaders.insert(make_pair("blit",shader));
    
    /// �ϲ����Ʊʻ���ͼ��
    uniforms.clear();
    uniforms.push_back("mvpMat");
    uniforms.push_back("texture");
    uniforms.push_back("mask");
    uniforms.push_back("color");
    uniforms.push_back("lockAlpha");
    
    shader = new CZShader("blit","compositeWithMask",attributes,uniforms);
    shaders.insert(make_pair("compositeWithMask",shader));
    
    /// �ϲ������ʻ���ͼ��
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

/// ���û�ˢ
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
