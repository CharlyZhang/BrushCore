#ifndef _CZMACRO_H_
#define _CZMACRO_H_

#include <stdio.h>
#include "tool/CZLog.h"
#include "graphic/CZGLdef.h"

#define DEFAULT_GLSL_DIR	"../../core/glsl/"


#ifndef NULL
# define NULL       0
#endif
#ifndef M_E
# define M_E        2.71828182845904523536
#endif

#ifndef M_PI
# define M_PI       3.14159265358979323846
#endif

#ifndef M_PI_2
# define M_PI_2     1.57079632679489661923
#endif

#ifndef M_PI_4
# define M_PI_4     0.785398163397448309616
#endif

#define CZ_DEBUG_GL		1		// for debugging

#define RENDER_TWO_RECT		0		///< 绘制两个矩形
#define RENDER_FULL_RECT	1		///< 绘制全屏纹理
#define RENDER_FREEHAND		1		///< 绘制手绘轨迹（不使用任何以下纹理）
#define RENDER_IMGAGE		0		///< render image onto active layer

#define STAMP_TEX		0		///< 笔触纹理
#define STROKE_TEX		0		///< 轨迹纹理

#endif