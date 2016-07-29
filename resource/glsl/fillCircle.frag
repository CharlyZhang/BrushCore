#ifdef GL_ES
precision highp float;
#endif

#if __VERSION__ >= 140
in vec2      outPosition;
in vec2		 centerPos;
out vec4     fragColor;
#else
varying vec2 outPosition;
varying vec2 centerPos;
#endif 

uniform float radius;
uniform float gray;

void main (void)
{
	float g = clamp(gray, 0.0, 1.0);
	float factor = length(outPosition-centerPos);

	if (factor <= radius)				gl_FragColor = vec4(g,g,g,1.0);
    else								gl_FragColor = vec4(0.0,0.0,0.0,0.0) ;

}
