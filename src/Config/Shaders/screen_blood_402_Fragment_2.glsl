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
vec3 pow3 ( vec3 a, float b ) { return vec3( pow( a.x, b ), pow( a.y, b ), pow( a.z, b ) ); }
uniform vec4 _fa_ [4];
uniform sampler2D samp_transmap;
uniform sampler2D samp_transmap1;
uniform sampler2D samp_spare1map;
uniform sampler2D samp_spare2map;

in vec4 vofi_TexCoord0;

out vec4 out_FragColor0;

void main() {
	vec2 texcoord1 = vofi_TexCoord0.xy;
	vec2 texcoord2 = vofi_TexCoord0.zw;
	vec3 texel_pulse1 = tex2D( samp_transmap, texcoord1 ).xyz;
	vec3 texel_pulse2 = tex2D( samp_transmap1, texcoord1 ).xyz;
	vec3 texel_pulse = mix( texel_pulse1, texel_pulse2, saturate( _fa_[0 ].y ) );
	vec3 texel_heavy = tex2D( samp_spare1map, texcoord1 ).xyz;
	vec3 texel_splash = tex2D( samp_spare2map, texcoord2 ).xyz;
	vec3 texel_breath = texel_splash;
	vec3 pulse = pow3( texel_pulse, _fa_[0 ].x );
	pulse = mix( vec3( 1.0 ), pulse, _fa_[0 ].w );
	texel_breath = vec3( dot3( vec3( 0.299, 0.587, 0.114 ), texel_breath ) );
	vec3 breath = vec3( pow( texel_breath.y, _fa_[1 ].x ) );
	breath = mix( vec3( 1.0 ), breath, _fa_[1 ].w );
	vec3 heavy = mix( vec3( 1.0 ), texel_heavy, _fa_[2 ].w );
	vec3 splash = mix( vec3( 1.0 ), texel_splash, _fa_[3 ].w );
	vec3 color = breath * pulse * heavy * splash;
	out_FragColor0.xyz = color;
	out_FragColor0.w = 1.0;
}