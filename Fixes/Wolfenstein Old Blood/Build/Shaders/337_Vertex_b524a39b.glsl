#version 150
// Lights
uniform float g_vertexEnabled;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;

float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
vec2 saturate( vec2 v ) { return clamp( v, 0.0, 1.0 ); }
vec3 saturate( vec3 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 saturate( vec4 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 tex2Dlod( sampler2D sampler, vec4 texcoord ) { return textureLod( sampler, texcoord.xy, texcoord.w ); }

float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0.0, 1.0 ), b ); }
vec4 swizzleColor ( vec4 color ) { return color; }
uniform vec4 _va_ [5];

in vec4 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;

out vec4 vofi_TexCoord0;
out vec4 vofi_Color;

void main() {
	gl_Position.x = dot4( in_Position, _va_[0 ] );
	gl_Position.y = dot4( in_Position, _va_[1 ] );
	gl_Position.z = dot4( in_Position, _va_[2 ] );
	gl_Position.w = dot4( in_Position, _va_[3 ] );
gl_Position.x += g_eye * g_eye_separation;

 if(g_vertexEnabled < 1.0)
{gl_Position = vec4(0.0);}

	vofi_TexCoord0 = in_TexCoord.xyxy;
	vofi_Color = swizzleColor( in_Color ) * _va_[4 ];
}
