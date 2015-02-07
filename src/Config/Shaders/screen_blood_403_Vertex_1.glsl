#version 150

float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
vec2 saturate( vec2 v ) { return clamp( v, 0.0, 1.0 ); }
vec3 saturate( vec3 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 saturate( vec4 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 tex2Dlod( sampler2D sampler, vec4 texcoord ) { return textureLod( sampler, texcoord.xy, texcoord.w ); }

float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0.0, 1.0 ), b ); }
vec4 swizzleColor ( vec4 color ) { return color; }
uniform vec4 _va_ [7];

in vec4 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;
in vec4 in_Tangent;

out vec4 vofi_TexCoord0;
out vec4 vofi_Color;
out vec4 vofi_TexCoord1;

void main() {
	vec4 position = in_Position * _va_[0 ] + _va_[1 ];
	gl_Position.x = dot4( position, _va_[2 ] );
	gl_Position.y = dot4( position, _va_[3 ] );
	gl_Position.z = dot4( position, _va_[4 ] );
	gl_Position.w = dot4( position, _va_[5 ] );
	vofi_TexCoord0 = vec4( in_TexCoord.x * _va_[6 ].x + _va_[6 ].z, in_TexCoord.y * _va_[6 ].y + _va_[6 ].w, 0, 0 );
	vofi_Color = swizzleColor( in_Color );
	vofi_TexCoord1 = saturate( in_Tangent * vec4( 2.0*255/254 ) - vec4( 255/255 ) );
}