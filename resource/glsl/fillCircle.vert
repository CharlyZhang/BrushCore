#ifdef GL_ES
precision highp float;
#endif

uniform mat4 mvpMat;
uniform vec2 center;

#if __VERSION__ >= 140
in vec4  inPosition;  
out vec2 outPosition;
out vec2 centerPos;
#else
attribute vec4 inPosition; 
varying vec2 outPosition; 
varying vec2 centerPos;
#endif

void main (void) 
{
	gl_Position	= mvpMat * inPosition;
	outPosition = gl_Position.xy;
	
	vec4 cPos = mvpMat * vec4(center,0,1);
	centerPos = cPos.xy;
}

