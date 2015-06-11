#define LOWP
 #define MEDP
uniform float g_vertexEnabled;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;
uniform vec4 g_custom_params;
varying vec2 uvVarying;
varying vec4 colorVarying;
attribute vec4 color;
attribute vec2 uv;
attribute vec4 position;
void main ()
{
  gl_Position = position;
gl_Position.x += g_eye * g_eye_separation * g_custom_params.x;

 if(g_vertexEnabled < 1.0)
{gl_Position = vec4(0.0);}

  uvVarying = uv;
  colorVarying = color;
}

