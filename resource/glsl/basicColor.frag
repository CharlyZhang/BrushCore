#ifdef GL_ES
precision highp float;
#endif

uniform float intensity;

void main (void)
{
	float i = clamp(intensity, 0.0, 1.0);
    gl_FragColor = vec4(i,i,i,i);
}
