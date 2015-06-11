#define LOWP
 #define MEDP
uniform float g_vertexEnabled;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;
uniform vec4 g_custom_params;
varying vec4 animLerpVposVarying;
varying vec4 uvVarying;
varying vec4 colorVarying;
uniform mat4 g_mVertexPositions;
uniform vec2 g_vParticlePivot;
uniform vec4 g_vColor;
uniform mat4 g_mViewProj;
uniform vec3 g_vParticleToWorld;
uniform vec3 g_vScale;
attribute vec4 color;
attribute vec4 scaleIncandVid;
attribute vec4 positionNormAge;
void main ()
{
  vec4 vColor_1;
  vec3 vPos_2;
  int tmpvar_3;
  tmpvar_3 = int(scaleIncandVid.w);
  vec2 tmpvar_4;
  tmpvar_4 = g_mVertexPositions[tmpvar_3].xy;
  vec3 tmpvar_5;
  tmpvar_5.z = 0.000000;
  tmpvar_5.xy = (tmpvar_4 + g_vParticlePivot);
  vPos_2.z = tmpvar_5.z;
  vPos_2.x = (tmpvar_5.x * scaleIncandVid.x);
  vPos_2.y = ((tmpvar_5.y * scaleIncandVid.x) * scaleIncandVid.y);
  vec4 tmpvar_6;
  tmpvar_6 = (color * g_vColor);
  vColor_1.w = tmpvar_6.w;
  vColor_1.xyz = (tmpvar_6.xyz + scaleIncandVid.z);
  vec4 tmpvar_7;
  tmpvar_7.w = 1.00000;
  tmpvar_7.xyz = ((((vPos_2 + positionNormAge.xyz) * g_vScale) + g_vParticleToWorld) * 300.000);
  gl_Position = (tmpvar_7 * g_mViewProj);
  gl_Position.x += g_eye * g_eye_separation * g_custom_params.x;

 if(g_vertexEnabled < 1.0)
{gl_Position = vec4(0.0);}

  vec4 tmpvar_8;
  tmpvar_8.zw = vec2(0.000000, 0.000000);
  tmpvar_8.xy = (tmpvar_4 + 0.500000);
  uvVarying = tmpvar_8;
  colorVarying = vColor_1;
  vec4 tmpvar_9;
  tmpvar_9.w = 0.000000;
  tmpvar_9.x = 0.000000;
  tmpvar_9.yz = vec2(0.000000, 0.000000);
  animLerpVposVarying = tmpvar_9;
}

