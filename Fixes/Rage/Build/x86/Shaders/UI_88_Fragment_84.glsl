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

float dot3 ( vec3 a, vec3 b ) { return dot( a, b ); }
float dot3 ( vec3 a, vec4 b ) { return dot( a, b.xyz ); }
float dot3 ( vec4 a, vec3 b ) { return dot( a.xyz, b ); }
float dot3 ( vec4 a, vec4 b ) { return dot( a.xyz, b.xyz ); }
float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0, 1 ), b ); }
vec4 h4tex2D ( sampler2D image, vec2 texcoord ) { return tex2D( image, texcoord ); }
vec4 h4tex2Dproj ( sampler2D image, vec3 texcoord ) { return tex2Dproj( image, texcoord ); }
vec4 texPhys ( sampler2D image, vec2 texcoord ) { return tex2D( image, texcoord ); }
vec4 texPhys ( sampler2D image, vec2 texcoord, vec2 dx, vec2 dy ) { return tex2D( image, texcoord, dx, dy ); }
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
vec3 ScaleSpecular ( vec3 unscaled, float factor ) {
	return ( unscaled * 8.0 ) / ( factor * 255.0 + 8.0 );
}
uniform vec4 _fa_ [12];
uniform sampler2D samp_pagetablemap;
uniform sampler2D samp_physicalvmtrmappingsmap0;
uniform sampler2D samp_physicalvmtrpagesmap0;
uniform sampler2D samp_physicalvmtrpagesmap1;
uniform sampler2D samp_physicalvmtrpagesmap2;
uniform sampler2D samp_lightprojectmap;
uniform sampler2D samp_lightfalloffmap;
uniform samplerCube samp_dynamicenvmap;

in vec4 vofi_TexCoord1;
in vec4 vofi_TexCoord2;
in vec4 vofi_TexCoord0;
in vec4 vofi_TexCoord3;
in vec4 vofi_TexCoord4;
in vec4 vofi_TexCoord6;

out vec4 out_FragColor;

void main() {
	vec4 shadow = vec4( 1.0);
	vec2 texcoord = vofi_TexCoord1.xy; if ( _fa_[0 ].x > 0 ) {
		texcoord = fract( texcoord ) * _fa_[1 ].xy + _fa_[1 ].zw;
	}
	vec3 physCoord = VirtualToPhysical2( samp_pagetablemap, samp_physicalvmtrmappingsmap0, texcoord);
	vec4 specularPage = texPhys( samp_physicalvmtrpagesmap0, physCoord.xy );
	vec4 YCoCg = texPhys( samp_physicalvmtrpagesmap1, physCoord.xy );
	vec4 bumpPage = texPhys( samp_physicalvmtrpagesmap2, physCoord.xy );
	specularPage.xyz = ScaleSpecular( specularPage.xyz, bumpPage.z );
	bumpPage.z = 0;
	vec3 localNormal = vec3( ( bumpPage.wy * 2.0 ) - 1.0, 0.0 );
	localNormal.z = sqrt( abs( 1.0 - dot3( localNormal, localNormal ) ) );
	vec3 globalNormal;
	globalNormal.x = dot3( localNormal, vofi_TexCoord3 );
	globalNormal.y = dot3( localNormal, vofi_TexCoord4 );
	globalNormal.z = dot3( localNormal, vofi_TexCoord6 );
	globalNormal = normalize( globalNormal );
	vec3 toLight = normalize( _fa_[2 ].xyz - vofi_TexCoord2.xyz );
	vec3 toViewer = normalize( _fa_[3 ].xyz - vofi_TexCoord2.xyz );
	vec3 light = vec3( dot3( toLight, globalNormal ));
	vec4 lpm = h4tex2Dproj( samp_lightprojectmap, vofi_TexCoord0.xyw );
	light *= lpm.xyz;
	vec2 falloffTC = vec2( vofi_TexCoord0.z / vofi_TexCoord0.w, 0.5 );
	vec4 lfm = h4tex2D( samp_lightfalloffmap, falloffTC );
	light *= lfm.xyz;
	light *= _fa_[4 ].xyz;
	shadow.x = ( shadow.x * _fa_[5 ].x ) + _fa_[5 ].y;
	light *= shadow.x;
	vec3 specular = vec3( 0.0);
	vec4 reflection;
	reflection.xyz = ( 2.0 * globalNormal * dot3( globalNormal, toViewer ) ) - toViewer;
	reflection.w = floor( ( 1.0 - bumpPage.x ) * 4.0 + _fa_[6 ].x );
	vec3 envColor = vec3( texCUBElod( samp_dynamicenvmap, reflection ));
	specular = envColor * _fa_[7 ].xyz;
	vec4 diffuse;
	YCoCg.z = ( YCoCg.z * 31.875 ) + 1.0;
	YCoCg.z = 1.0 / YCoCg.z;
	YCoCg.xy *= YCoCg.z;
	diffuse.x = dot4( YCoCg, matrixCoCg1YtoRGB1X );
	diffuse.y = dot4( YCoCg, matrixCoCg1YtoRGB1Y );
	diffuse.z = dot4( YCoCg, matrixCoCg1YtoRGB1Z );
	diffuse.w = 1.0;
	vec3 color = light * ( specular * specularPage.xyz + diffuse.xyz * _fa_[8 ].xyz * 2.0 );
	float fog = saturate( exp2( -clamp( length( vofi_TexCoord2.xyz - _fa_[3 ].xyz ) - _fa_[9 ].x, 0.0, _fa_[10 ].x ) * _fa_[11 ].x ) );
	out_FragColor.rgb = color * fog;
	out_FragColor.a = 1;
}