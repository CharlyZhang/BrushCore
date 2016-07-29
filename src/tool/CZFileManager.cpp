//
//  CZFileManager.cpp
//  BrushesCore
//
//  Created by CharlyZhang on 15/11/24.
//  Copyright © 2015年 CharlyZhang. All rights reserved.
//

#include "CZFileManager.h"
#include "CZDefine.h"
#include "CZPainting.h"

using namespace std;

CZFileManager::CZFileManager()
{
    
}

CZFileManager::~CZFileManager()
{
}

bool CZFileManager::savePainting(CZPainting *painting, const char * filepath)
{
    if (painting == nullptr)
    {
        LOG_ERROR("painting is NULL\n");
        return false;
    }
    
    if (filepath == nullptr)
    {
        LOG_ERROR("filepath is NULL\n");
        return nullptr;
    }
    
    // open file
    FILE *fp = fopen(filepath, "wb");
    
    if (fp == NULL)
    {
        LOG_ERROR("file open failed\n");
        return false;
    }
    
    CZPainting::Type type = painting->getType();
    fwrite((void*)&type, sizeof(CZPainting::Type), 1, fp);
    
    int activeLayerInd = painting->getActiveLayerIndex();
    fwrite((void*)&activeLayerInd, sizeof(int), 1, fp);
    CZSize size = painting->getDimensions();
    fwrite((void*)&(size.width), sizeof(float), 1, fp);
    fwrite((void*)&(size.height), sizeof(float), 1, fp);
    
    // layers
    int layersNum = painting->getLayersNumber();
    fwrite((void*)&layersNum, sizeof(int), 1, fp);
    for (int i = 0; i <  layersNum; i++ )
    {
        CZLayer *layer = painting->getLayer(i);
        
        bool temp = layer->isVisible();
        fwrite((void*)&temp, sizeof(bool), 1, fp);
        temp = layer->isLocked();
        fwrite((void*)&temp, sizeof(bool), 1, fp);
        
        float opacity = layer->getOpacity();
        fwrite((void*)&opacity, sizeof(float), 1, fp);
        
        // image
        CZImage *img = layer->imageData();
        img->saveToFile(fp);
        delete img;
    }
    
    fclose(fp);
    
    return true;
}

bool CZFileManager::loadPainting(const char* filepath, CZPainting* painting)
{
    if (filepath == nullptr)
    {
        LOG_ERROR("urlStr is NULL\n");
        return nullptr;
    }
    
    if (painting == nullptr)
    {
        LOG_ERROR("painting is NULL\n");
        return nullptr;
    }
    
    // open file
    FILE *fp = fopen(filepath, "rb");
    
    if (fp == NULL)
    {
        LOG_WARN("no such a file exists\n");
        return nullptr;
    }
    
    CZPainting::Type type;
    int activeLayerInd;
    float width,height;
    fread((void*)&type, sizeof(CZPainting::Type), 1, fp);
    fread((void*)&activeLayerInd, sizeof(int), 1, fp);
    fread((void*)&width, sizeof(float), 1, fp);
    fread((void*)&height, sizeof(float), 1, fp);
    
    CZSize size = painting->getDimensions();
    if (width != size.width || height != size.height)
    {
        /// TO DO:
        LOG_ERROR("dimension doesn't match!\n");
        fclose(fp);
        return false;
    }
    
    painting->restore(false);
    
    painting->setType(type);
    
    // layers
    int layersNum;
    fread((void*)&layersNum, sizeof(int), 1, fp);
    int i;
    for (i = 0; i < layersNum; i++ )
    {
        painting->addNewLayer();
        CZLayer *layer = painting->getActiveLayer();
        
        bool temp;
        fread((void*)&temp, sizeof(bool), 1, fp);
        layer->setVisiblility(temp);
        fread((void*)&temp, sizeof(bool), 1, fp);
        layer->setLocked(temp);
        
        float opacity;
        fread((void*)&opacity, sizeof(float), 1, fp);
        layer->setOpacity(opacity);
        
        // image
        CZImage *img = CZImage::createFromFile(fp);
        layer->setImage(img);
    }
    
    painting->setActiveLayer(activeLayerInd);
    
    fclose(fp);
    
    return true;
}

bool CZFileManager::removePainting(const char* filepath)
{
    if (filepath == nullptr)
    {
        LOG_ERROR("urlStr is NULL\n");
        return nullptr;
    }
    
    if (remove(filepath) == 0)
    {
        LOG_INFO("Removed %s.\n",filepath);
        return true;
    }
    else
    {
        LOG_ERROR("remove failed\n");
        return false;
    }
}
