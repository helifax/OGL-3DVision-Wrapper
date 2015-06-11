#version 150

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

vec4 h4tex2D ( sampler2D image, vec2 texcoord ) { return tex2D( image, texcoord ); }
vec2 screenPosToTexcoord ( vec2 pos, vec4 bias_scale ) { return ( pos * bias_scale.zw + bias_scale.xy ); }
uniform vec4 _fa_ [2];
uniform sampler2D samp_glaremap;

in vec4 gl_FragCoord;

out vec4 out_FragColor;

void main() {
	float bloom0 = 0.134598;
	float bloom1 = 0.127325;
	float bloom2 = 0.107778;
	float bloom3 = 0.081638;
	float bloom4 = 0.055335;
	float bloom5 = 0.033562;
	float bloom6 = 0.018216;
	float bloom7 = 0.008847;
	vec2 tc = screenPosToTexcoord( gl_FragCoord.xy, _fa_[0 ] );
	vec2 step = _fa_[1 ].xy * _fa_[0 ].zw;
	vec2 dir = step;
	vec4 total;
	total = h4tex2D( samp_glaremap, tc ) * bloom0;
	total += ( h4tex2D( samp_glaremap, tc + dir ) + h4tex2D( samp_glaremap, tc - dir ) ) * bloom1;
	dir += step;
	total += ( h4tex2D( samp_glaremap, tc + dir ) + h4tex2D( samp_glaremap, tc - dir ) ) * bloom2;
	dir += step;
	total += ( h4tex2D( samp_glaremap, tc + dir ) + h4tex2D( samp_glaremap, tc - dir ) ) * bloom3;
	dir += step;
	total += ( h4tex2D( samp_glaremap, tc + dir ) + h4tex2D( samp_glaremap, tc - dir ) ) * bloom4;
	dir += step;
	total += ( h4tex2D( samp_glaremap, tc + dir ) + h4tex2D( samp_glaremap, tc - dir ) ) * bloom5;
	dir += step;
	total += ( h4tex2D( samp_glaremap, tc + dir ) + h4tex2D( samp_glaremap, tc - dir ) ) * bloom6;
	dir += step;
	total += ( h4tex2D( samp_glaremap, tc + dir ) + h4tex2D( samp_glaremap, tc - dir ) ) * bloom7;
	out_FragColor = total * 2.0;
}