

#include "CZGLContext.h"
#include "../CZUtil.h"
#include "glDef.h"

struct CZGLContext::Impl
{
#if USE_OPENGL_ES
	EAGLContext *realContext;
	Impl()
	{
		realContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	}
	~Impl()
	{
		if(realContext) [realContext release ];
		realContext = NULL;
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
#if USE_OPENGL_ES
	[EAGLContext setCurrentContext:nil];
#endif
}

bool CZGLContext::setAsCurrent()
{
#if USE_OPENGL_ES
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
#if USE_OPENGL_ES
	return (void*)impl->realContext;
#endif
	return NULL;
}