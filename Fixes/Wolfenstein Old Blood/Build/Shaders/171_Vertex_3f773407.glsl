#version 150
uniform float g_vertexEnabled;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;

float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
vec2 saturate( vec2 v ) { return clamp( v, 0.0, 1.0 ); }
vec3 saturate( vec3 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 saturate( vec4 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 tex2Dlod( sampler2D sampler, vec4 texcoord ) { return textureLod( sampler, texcoord.xy, texcoord.w ); }

float dot3 ( vec3 a, vec3 b ) { return dot( a, b ); }
float dot3 ( vec3 a, vec4 b ) { return dot( a, b.xyz ); }
float dot3 ( vec4 a, vec3 b ) { return dot( a.xyz, b ); }
float dot3 ( vec4 a, vec4 b ) { return dot( a.xyz, b.xyz ); }
float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0.0, 1.0 ), b ); }
uniform vec4 _va_ [6];

in vec4 in_Position;
in vec4 in_Normal;


void main() {
	vec4 pos = in_Position;
	vec3 normal = in_Normal.xyz * 2.0 - 1.0;
	if ( dot3( normal, _va_[0 ] ) > 0.0 ) {
		float dist = dot4( pos, _va_[1 ] );
		float closing = dot3( _va_[0 ].xyz, _va_[1 ].xyz );
		pos.xyz -= _va_[0 ].xyz * ( dist / closing );
	}
	gl_Position.x = dot4( pos, _va_[2 ] );
	gl_Position.y = dot4( pos, _va_[3 ] );
	gl_Position.z = dot4( pos, _va_[4 ] );
	gl_Position.w = dot4( pos, _va_[5 ] );
 	gl_Position.x += g_eye * g_eye_separation * (gl_Position.w - g_convergence);

 if(g_vertexEnabled < 1.0)
{gl_Position = vec4(0.0);}

}
