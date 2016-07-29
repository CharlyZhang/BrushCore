//
//  CZFileManager.h
//  BrushesCore
//
//  Created by CharlyZhang on 15/11/24.
//  Copyright © 2015年 CharlyZhang. All rights reserved.
//

#ifndef _CZFILEMANAGER_H_
#define _CZFILEMANAGER_H_

#include <stdio.h>
#include <string>

class CZPainting;

class CZFileManager
{
public:
    static CZFileManager * getInstance()
    {
        static CZFileManager instance;
        return &instance;
    }
    
    bool savePainting(CZPainting *painting, const char *filepath);
    
    bool loadPainting(const char* filepath, CZPainting* painting);
    
    bool removePainting(const char* filepath);
    
private:
    CZFileManager();
    ~CZFileManager();
    
};

#endif /* _CZFILEMANAGER_H_ */
