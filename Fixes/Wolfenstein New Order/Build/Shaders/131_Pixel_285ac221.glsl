#version 150
// Shadows
uniform float g_pixelEnabled;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;

void clip( float v ) { if ( v < 0.0 ) { discard; } }
void clip( vec2 v ) { if ( any( lessThan( v, vec2( 0.0 ) ) ) ) { discard; } }
void clip( vec3 v ) { if ( any( lessThan( v, vec3( 0.0 ) ) ) ) { discard; } }
void clip( vec4 v ) { if ( any( lessThan( v, vec4( 0.0 ) ) ) ) { discard; } }

float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
vec2 saturate( vec2 v ) { return clamp( v, 0.0, 1.0 ); }
vec3 saturate( vec3 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 saturate( vec4 v ) { return clamp( v, 0.0, 1.0 ); }

vec4 tex2D( sampler2D sampler, vec2 texcoord ) { return texture( sampler, texcoord.xy ); }
vec4 tex2D( sampler2DShadow sampler, vec3 texcoord ) { return vec4( texture( sampler, texcoord.xyz ) ); }

vec4 tex2D( sampler2D sampler, vec2 texcoord, vec2 dx, vec2 dy ) { return textureGrad( sampler, texcoord.xy, dx, dy ); }
vec4 tex2D( sampler2DShadow sampler, vec3 texcoord, vec2 dx, vec2 dy ) { return vec4( textureGrad( sampler, texcoord.xyz, dx, dy ) ); }

vec4 texCUBE( samplerCube sampler, vec3 texcoord ) { return texture( sampler, texcoord.xyz ); }
vec4 texCUBE( samplerCubeShadow sampler, vec4 texcoord ) { return vec4( texture( sampler, texcoord.xyzw ) ); }

vec4 tex1Dproj( sampler1D sampler, vec2 texcoord ) { return textureProj( sampler, texcoord ); }
vec4 tex2Dproj( sampler2D sampler, vec3 texcoord ) { return textureProj( sampler, texcoord ); }
vec4 tex3Dproj( sampler3D sampler, vec4 texcoord ) { return textureProj( sampler, texcoord ); }

vec4 tex1Dbias( sampler1D sampler, vec4 texcoord ) { return texture( sampler, texcoord.x, texcoord.w ); }
vec4 tex2Dbias( sampler2D sampler, vec4 texcoord ) { return texture( sampler, texcoord.xy, texcoord.w ); }
vec4 tex3Dbias( sampler3D sampler, vec4 texcoord ) { return texture( sampler, texcoord.xyz, texcoord.w ); }
vec4 texCUBEbias( samplerCube sampler, vec4 texcoord ) { return texture( sampler, texcoord.xyz, texcoord.w ); }

vec4 tex1Dlod( sampler1D sampler, vec4 texcoord ) { return textureLod( sampler, texcoord.x, texcoord.w ); }
vec4 tex2Dlod( sampler2D sampler, vec4 texcoord ) { return textureLod( sampler, texcoord.xy, texcoord.w ); }
vec4 tex3Dlod( sampler3D sampler, vec4 texcoord ) { return textureLod( sampler, texcoord.xyz, texcoord.w ); }
vec4 texCUBElod( samplerCube sampler, vec4 texcoord ) { return textureLod( sampler, texcoord.xyz, texcoord.w ); }

float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0.0, 1.0 ), b ); }
float tex2Ddepth ( sampler2D image, vec2 texcoord ) { return tex2D( image, texcoord ).x; }
float shadow2Ddepth ( sampler2DShadow image, vec3 texcoord ) { return tex2D( image, texcoord ).x; }
vec2 screenPosToTexcoord ( vec2 pos, vec4 bias_scale ) { return ( pos * bias_scale.zw + bias_scale.xy ); }
uniform vec4 _fa_ [9];
uniform sampler2D samp_viewdepthmap;
uniform sampler2D samp_dither256map;
uniform sampler2DShadow samp_dimshadowdepthmap;
uniform sampler2D samp_viewcolormap;

in vec4 gl_FragCoord;

out vec4 out_FragColor0;

void main() {
	vec3 window;
	window.xy = screenPosToTexcoord( gl_FragCoord.xy, _fa_[0 ] );
	vec2 texcoord = screenPosToTexcoord( gl_FragCoord.xy, _fa_[1 ] );
	window.z = tex2Ddepth( samp_viewdepthmap, texcoord );
	vec4 wclip = vec4( window.xyz, 1.0 );
	vec4 light;
	light.x = dot4( wclip, _fa_[2 ] );
	light.y = dot4( wclip, _fa_[3 ] );
	light.z = dot4( wclip, _fa_[4 ] );
	light.w = dot4( wclip, _fa_[5 ] );
	light.xyz /= light.w;


mat4 inverseMVP = mat4(_fa_[2], _fa_[3], _fa_[4], _fa_[5]);
mat4 MVP = inverse(inverseMVP);
light = vec4(wclip * inverseMVP);
light /= light.w;
	
light.w = normalize(light.w);
vec4 WP_temp = light * MVP;
WP_temp.x -= g_eye * g_eye_separation * (WP_temp.w - g_convergence) * 0.5;
light = vec4(WP_temp * inverseMVP);
light /= light.w;	


	light = ( light * 0.5 ) + 0.5;
	light.z = min( light.z, 0.9999999 );
	vec2 tc = gl_FragCoord.xy * vec2( 1.0/128, 1.0/128 ) + _fa_[6 ].xy;
	vec2 dither = tex2D( samp_dither256map, tc ).xw;
	float stepSize = _fa_[7 ].y;
	vec3 center;
	center.xy = light.xy + stepSize + dither * vec2( stepSize, stepSize );
	center.z = light.z;
	float shadow;
	shadow = shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -2*stepSize, -2*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -1*stepSize, -2*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( 0*stepSize, -2*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -3*stepSize, -2*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -2*stepSize, -1*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -1*stepSize, -1*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( 0*stepSize, -1*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -3*stepSize, -1*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -2*stepSize, 0*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -1*stepSize, 0*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( 0*stepSize, 0*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -3*stepSize, 0*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -2*stepSize, -3*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -1*stepSize, -3*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( 0*stepSize, -3*stepSize, 0 ) );
	shadow += shadow2Ddepth( samp_dimshadowdepthmap, center + vec3( -3*stepSize, -3*stepSize, 0 ) );
	shadow *= 0.0625;
	float acceptShadow = tex2D( samp_viewcolormap, texcoord ).w;
	shadow = ( 1.0 - shadow ) * acceptShadow * _fa_[8 ].x;
	out_FragColor0 = vec4( 0.0, 0.0, 0.0, shadow );
 if(g_pixelEnabled < 1.0)
{out_FragColor0 = vec4(0.0);}

}