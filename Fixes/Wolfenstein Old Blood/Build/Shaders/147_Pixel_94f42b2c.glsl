#version 150

// Not sure

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

float dot3 ( vec3 a, vec3 b ) { return dot( a, b ); }
float dot3 ( vec3 a, vec4 b ) { return dot( a, b.xyz ); }
float dot3 ( vec4 a, vec3 b ) { return dot( a.xyz, b ); }
float dot3 ( vec4 a, vec4 b ) { return dot( a.xyz, b.xyz ); }
float tex2Ddepth ( sampler2D image, vec2 texcoord ) { return tex2D( image, texcoord ).x; }
vec2 screenPosToTexcoord ( vec2 pos, vec4 bias_scale ) { return ( pos * bias_scale.zw + bias_scale.xy ); }
uniform vec4 _fa_ [7];
uniform sampler2D samp_viewdepthmap;
uniform sampler2D samp_transmap;

in vec4 gl_FragCoord;
in vec4 vofi_TexCoord4;
in vec4 vofi_TexCoord1;
in vec4 vofi_TexCoord2;
in vec4 vofi_TexCoord3;
in vec4 vofi_Color;
in vec4 vofi_TexCoord0;
in vec4 vofi_TexCoord5;

out vec4 out_FragColor0;

void main() {
	vec2 viewTexCoord = screenPosToTexcoord( gl_FragCoord.xy, _fa_[0 ] );
	float viewZ = tex2Ddepth( samp_viewdepthmap, viewTexCoord );
	vec2 window = screenPosToTexcoord( gl_FragCoord.xy, _fa_[1 ] );
	vec4 wclip = vec4( window, viewZ, 1.0 );
	vec4 worldPos;
	worldPos.x = dot( wclip, _fa_[2 ] );
	worldPos.y = dot( wclip, _fa_[3 ] );
	worldPos.z = dot( wclip, _fa_[4 ] );
	worldPos.w = dot( wclip, _fa_[5 ] );
	worldPos /= worldPos.w;
	
	
	// Attempt 2	
mat4 inverseMVP = mat4(_fa_[2], _fa_[3], _fa_[4], _fa_[5]);
mat4 MVP = inverse(inverseMVP);
worldPos = vec4(wclip * inverseMVP);
worldPos /= worldPos.w;
	
vec4 WP_temp = worldPos * MVP;
WP_temp.x -= g_eye * g_eye_separation * (WP_temp.z - g_convergence)*0.5;
worldPos = vec4(WP_temp * inverseMVP);
worldPos /= worldPos.w;
	
	vec3 center = vofi_TexCoord4.xyz;
	worldPos.xyz -= center;
	vec3 tangent = normalize( vofi_TexCoord1.xyz );
	vec3 bitangent = normalize( vofi_TexCoord2.xyz );
	vec3 normal = normalize( vofi_TexCoord3.xyz );
	vec3 localPos = worldPos.xyz;
	localPos.x = dot3( worldPos, tangent );
	localPos.y = dot3( worldPos, bitangent );
	localPos.z = dot3( worldPos, normal );
	vec3 sizes = vofi_Color.xyz;
	localPos.xyz *= sizes;
	vec3 tc = saturate( ( localPos * 0.5 ) + 0.5 );
	tc.x = 1.0 - tc.x;
	vec4 inset;
	inset.xy = 8.0 / vec2( vofi_TexCoord0.y * _fa_[6 ].x, vofi_TexCoord0.w * _fa_[6 ].y );
	inset.zw = vec2( 0.0 );
	clip( min( tc.xyzx - inset, vec4( 1.0 ) - ( tc.xyzx + inset ) ) );
	float edgefade = 1.0;
	float dist = dot( localPos, localPos );
	dist = pow( dist, 4.0 );
	edgefade = 1.0 - dist;
	vec2 texcoord;
	texcoord.x = tc.x * vofi_TexCoord0.y + vofi_TexCoord0.x;
	texcoord.y = tc.y * vofi_TexCoord0.w + vofi_TexCoord0.z;
	vec3 fogColor = vofi_TexCoord5.xyz;
	float fogAmount = vofi_TexCoord5.w;
	vec4 final = tex2D( samp_transmap, texcoord );
	final.w *= vofi_Color.w;
	final.w *= edgefade;
	out_FragColor0.xyz *= 1.0 - fogAmount;
	out_FragColor0.xyz = final.xyz * final.w;
	out_FragColor0.w = final.w;
 if(g_pixelEnabled < 1.0)
{out_FragColor0 = vec4(0.0);}

}