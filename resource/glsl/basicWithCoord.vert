#ifdef GL_ES
precision highp float;
#endif

uniform mat4 mvpMat;

#if __VERSION__ >= 140
in vec4  inPosition;  
out vec2 outPosition;
#else
attribute vec4 inPosition; 
varying vec2 outPosition; 
#endif

void main (void) 
{
	gl_Position	= mvpMat * inPosition;
	outPosition = gl_Position.xy;
}

