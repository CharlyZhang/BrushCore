

#include "CZGLContext.h"
#include "../CZUtil.h"
#include "CZGLdef.h"
#include <OpenGLES/EAGL.h>

struct CZGLContext::Impl
{
#if defined(__APPLE__)
    EAGLContext *realContext;
    Impl()
    {
        realContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    }
    ~Impl()
    {
        //if(realContext) [realContext release ];
        realContext = NULL;
        [EAGLContext setCurrentContext:nil];
    }
#endif
};

CZGLContext::CZGLContext()
{
    impl = new Impl;
}

CZGLContext::~CZGLContext()
{
    delete impl;
}

bool CZGLContext::setAsCurrent()
{
#if defined(__APPLE__)
    if (!impl->realContext || ![EAGLContext setCurrentContext:impl->realContext]) {
        LOG_ERROR("context cannot be set as current!\n");
        return false;
    }
#endif
    
	// configure some default GL state
	glDisable(GL_DITHER);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

//	glEnable(GL_BLEND);
//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    CZCheckGLError();
    
	return true;
}

void* CZGLContext::getRealContext()
{
#if defined(__APPLE__)
    return (__bridge void*)impl->realContext;
#endif
}