#ifdef GL_ES
precision highp float;
#endif

#if __VERSION__ >= 140
in vec2      outPosition;
out vec4     fragColor;
#else
varying vec2 outPosition;
#endif

uniform float hardness;

void main (void)
{
	float h = clamp(hardness, 0.0, 1.0);
	float factor = length(outPosition);

	if (factor <= h)				gl_FragColor = vec4(1.0,1.0,1.0,1.0);
	else if(factor <= 1.0)			gl_FragColor = vec4(1.0,1.0,1.0,1.0) * (1.0-factor)/(1.0-h);
    else							gl_FragColor = vec4(0.0,0.0,0.0,0.0) ;

}
