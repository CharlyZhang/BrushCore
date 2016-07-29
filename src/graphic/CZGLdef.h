#ifndef _CZGLDEF_H_
#define _CZGLDEF_H_

//// iOS
#if defined(__APPLE__)
//# import <OpenGLES/ES1/gl.h>
//# import <OpenGLES/ES1/glext.h>
# import <OpenGLES/ES2/gl.h>
# import <OpenGLES/ES2/glext.h>

# define GL_GEN_VERTEXARRAY(n,arr)	glGenVertexArraysOES(n, arr)
# define GL_BIND_VERTEXARRAY(id)	glBindVertexArrayOES(id)
# define GL_DEL_VERTEXARRAY(n,arr)	glDeleteVertexArraysOES(n,arr)

# define USE_OPENGL_ES

//// Windows
#elif defined(_WIN32)
//# include <gl/GL.h>
//# include <gl/GLU.h>
# include "glew.h"
# include "glut.h"

# define USE_OPENGL

//// Android
#elif defined(__ANDROID__)
# include <GLES2/gl2.h>
# define GL_GLEXT_PROTOTYPES
# include <GLES2/gl2ext.h>

# define GL_GEN_VERTEXARRAY(n,arr)
# define GL_BIND_VERTEXARRAY(id)
# define GL_DEL_VERTEXARRAY(n,arr)

# define USE_OPENGL_ES
#endif

//////// different definitions of GL and GLES

#if defined USE_OPENGL

/// functions
# define GL_GEN_VERTEXARRAY(n,arr)	glGenVertexArrays(n, arr)
# define GL_BIND_VERTEXARRAY(id)	glBindVertexArray(id)
# define GL_DEL_VERTEXARRAY(n,arr)	glDeleteVertexArrays(n,arr)
# define GL_DRAW_BUF(arr)           glDrawBuffer(arr)
# define GL_PUSH_ATTR(arr)          glPushAttrib(arr)
# define GL_POP_ATTR()              glPopAttrib()

# define DEFAULT_STORAGEMODE		RGBA_FLOAT

#elif defined USE_OPENGL_ES

/// type
# define GL_RGB8					GL_RGB8_OES
# define GL_RGBA8					GL_RGBA8_OES
# define DEFAULT_STORAGEMODE		RGBA_BYTE

/// functions
# define GL_DRAW_BUF(arr)
# define GL_PUSH_ATTR(arr)
# define GL_POP_ATTR()
#endif

#endif