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

float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0, 1 ), b ); }
const vec4 matrixCoCg1YtoRGB1X = vec4( 1.0, -1.0, 0.0, 1.0 );
const vec4 matrixCoCg1YtoRGB1Y = vec4( 0.0, 1.0, -0.50196078, 1.0 );
const vec4 matrixCoCg1YtoRGB1Z = vec4( -1.0, -1.0, 1.00392156, 1.0 );
vec3 VirtualToPhysical2 ( sampler2D virtToPhysPage, sampler2D physPageToXform0, vec2 virtCoords ) {
	vec2 physPage = tex2D( virtToPhysPage, virtCoords).xy;
	vec4 xform = tex2D( physPageToXform0, physPage );
	vec3 physCoord;
	physCoord.xy = virtCoords * xform.x + xform.zw;
	physCoord.z = xform.y;
	return physCoord;
}
uniform vec4 _fa_ [2];
uniform sampler2D samp_pagetablemap;
uniform sampler2D samp_physicalvmtrmappingsmap0;
uniform sampler2D samp_physicalvmtrpagesmap1;
uniform sampler2D samp_physicalvmtrpagesmap2;

in vec4 vofi_Color;
in vec4 vofi_TexCoord1;

out vec4 out_FragColor;

void main() {
	vec2 texcoord = saturate( vofi_TexCoord1.xy ) * _fa_[0 ].xy + _fa_[0 ].zw;
	vec3 physCoord = VirtualToPhysical2( samp_pagetablemap, samp_physicalvmtrmappingsmap0, texcoord);
	vec4 YCoCg = tex2D( samp_physicalvmtrpagesmap1, physCoord.xy );
	vec4 bumpPage = tex2D( samp_physicalvmtrpagesmap2, physCoord.xy );
	YCoCg.z = ( YCoCg.z * 31.875 ) + 1.0;
	YCoCg.z = 1.0 / YCoCg.z;
	YCoCg.xy *= YCoCg.z;
	vec4 diffuse;
	diffuse.x = dot4( YCoCg, matrixCoCg1YtoRGB1X );
	diffuse.y = dot4( YCoCg, matrixCoCg1YtoRGB1Y );
	diffuse.z = dot4( YCoCg, matrixCoCg1YtoRGB1Z );
	diffuse.w = bumpPage.w;
	float scale = 1.0 - _fa_[1 ].x;
	float bias = _fa_[1 ].x;
	diffuse *= vofi_Color;
	out_FragColor.xyz = saturate( diffuse.xyz * scale + bias ) * diffuse.w;
	out_FragColor.w = diffuse.w;
}