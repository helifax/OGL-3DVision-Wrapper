#version 150

uniform float g_depth;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;

float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
vec2 saturate( vec2 v ) { return clamp( v, 0.0, 1.0 ); }
vec3 saturate( vec3 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 saturate( vec4 v ) { return clamp( v, 0.0, 1.0 ); }

float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0, 1 ), b ); }
uniform vec4 _va_ [12];
uniform vec4 hardwareSkinning ;
uniform matrices_ubo { vec4 matrices[768]; };

in vec4 in_Position;
in vec2 in_TexCoord;
in vec4 in_Normal;
in vec4 in_Color;
in vec4 in_Morph;

out vec4 gl_Position;
out vec4 vofi_Color;
out vec4 vofi_TexCoord0;

void main() {
	vec4 position = in_Position * _va_[0 ] + _va_[1 ];
	if ( hardwareSkinning.x != 0.0 ) {
		vec4 vertexPosition = position;
		if ( hardwareSkinning.y != 0.0 ) {
			vec3 morphVector = ( in_Morph.xyz * 2 ) - 1;
			vertexPosition.xyz += ( morphVector * in_Morph.w * 32.0 );
		}
		float weights = in_Normal.w * 255;
		float w1 = floor( weights / 16 );
		float w2 = floor( weights / 4 ) - ( w1 * 4 );
		float w3 = weights - ( w2 * 4 ) - ( w1 * 16 );
		w1 *= 1.0 / 30.0;
		w2 *= 1.0 / 9.0;
		w3 *= 1.0 / 12.0;
		float w0 = 1 - ( w1 + w2 + w3 );
		vec4 matX, matY, matZ;
		float joint = in_Color.r * 255;
		matX = matrices[int(joint*3+0)] * w0;
		matY = matrices[int(joint*3+1)] * w0;
		matZ = matrices[int(joint*3+2)] * w0;
		joint = in_Color.g * 255;
		matX += matrices[int(joint*3+0)] * w1;
		matY += matrices[int(joint*3+1)] * w1;
		matZ += matrices[int(joint*3+2)] * w1;
		joint = in_Color.b * 255;
		matX += matrices[int(joint*3+0)] * w2;
		matY += matrices[int(joint*3+1)] * w2;
		matZ += matrices[int(joint*3+2)] * w2;
		joint = in_Color.a * 255;
		matX += matrices[int(joint*3+0)] * w3;
		matY += matrices[int(joint*3+1)] * w3;
		matZ += matrices[int(joint*3+2)] * w3;
		position.x = dot4( vertexPosition, matX );
		position.y = dot4( vertexPosition, matY );
		position.z = dot4( vertexPosition, matZ );
		position.w = 1.0;
	}
	gl_Position.x = dot4( position, _va_[2 ] );
	gl_Position.y = dot4( position, _va_[3 ] );
	gl_Position.z = dot4( position, _va_[4 ] );
	gl_Position.w = dot4( position, _va_[5 ] );
	
	gl_Position.x += g_eye * -g_eye_separation/48 * (gl_Position.w - 39.781235);
	
	vofi_TexCoord0.x = dot4( in_TexCoord, _va_[6 ] );
	vofi_TexCoord0.y = dot4( in_TexCoord, _va_[7 ] );
	vofi_TexCoord0.z = -( clamp( gl_Position.w - _va_[8 ].x, 0.0, _va_[9 ].x ) * _va_[10 ].x );
	vofi_TexCoord0.w = 0.0;
	vofi_Color = _va_[11 ];
}