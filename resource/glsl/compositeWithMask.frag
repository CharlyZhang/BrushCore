#ifdef GL_ES
precision highp float;
#endif

#if __VERSION__ >= 140
in vec2      varTexcoord;
out vec4     fragColor;
#else
varying vec2 varTexcoord;
#endif

uniform sampler2D texture;
uniform sampler2D mask;
uniform vec4 color;
uniform bool lockAlpha;

void main (void)
{
    vec4 dst = texture2D(texture, varTexcoord.st, 0.0);
    float srcAlpha = color.a * texture2D(mask, varTexcoord.st, 0.0).a;
    
    float outAlpha = srcAlpha + dst.a * (1.0 - srcAlpha);
    
	if(outAlpha != 0.0)
		gl_FragColor.rgb = (color.rgb * srcAlpha + dst.rgb * dst.a * (1.0 - srcAlpha)) / outAlpha;
	else
		gl_FragColor.rgb = vec3(0,0,0);

    gl_FragColor.a = lockAlpha ? dst.a : outAlpha;
}
