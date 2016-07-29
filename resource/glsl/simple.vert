uniform mat4 mvpMat;
uniform vec4 color;

#if __VERSION__ >= 140
in vec4  inPosition;
out vec4 colorVarying;
#else
attribute vec4 inPosition;
varying vec4 colorVarying;
#endif

void main()
{
    gl_Position = mvpMat * inPosition;
    colorVarying = color;
}