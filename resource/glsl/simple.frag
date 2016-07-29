#ifdef GL_ES
// define default precision for float, vec, mat.
precision highp float;
#endif

#if __VERSION__ >= 140
in vec4  colorVarying;
#else
varying vec4 colorVarying;
#endif

void main()
{
    gl_FragColor = colorVarying;
}
