#version 150

uniform float g_depth;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;

float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
vec2 saturate( vec2 v ) { return clamp( v, 0.0, 1.0 ); }
vec3 saturate( vec3 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 saturate( vec4 v ) { return clamp( v, 0.0, 1.0 ); }

float dot3 ( vec3 a, vec3 b ) { return dot( a, b ); }
float dot3 ( vec3 a, vec4 b ) { return dot( a, b.xyz ); }
float dot3 ( vec4 a, vec3 b ) { return dot( a.xyz, b ); }
float dot3 ( vec4 a, vec4 b ) { return dot( a.xyz, b.xyz ); }
float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0, 1 ), b ); }
uniform vec4 _va_ [18];
uniform vec4 hardwareSkinning ;
uniform matrices_ubo { vec4 matrices[768]; };

in vec4 in_Position;
in vec2 in_TexCoord;
in vec4 in_Normal;
in vec4 in_Tangent;
in vec4 in_Color;
in vec4 in_Morph;

out vec4 gl_Position;
out vec4 vofi_TexCoord0;
out vec4 vofi_TexCoord1;
out vec4 vofi_TexCoord2;
out vec4 vofi_TexCoord3;
out vec4 vofi_TexCoord4;
out vec4 vofi_TexCoord5;
out vec4 vofi_TexCoord6;

void main() {
	vec4 position = in_Position * _va_[0 ] + _va_[1 ];
	vec4 normal = in_Normal * 2.0 - 1.0;
	vec4 tangent = in_Tangent * 2.0 - 1.0;
	float morphScale = 0.0;
	if ( hardwareSkinning.x != 0.0 ) {
		vec4 vertexPosition = position;
		if ( hardwareSkinning.y != 0.0 ) {
			vec3 morphVector = ( in_Morph.xyz * 2 ) - 1;
			morphScale = in_Morph.w;
			vertexPosition.xyz += ( morphVector * morphScale * 32.0 );
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
		vec4 tempNormal = normal;
		normal.x = dot3( tempNormal, matX );
		normal.y = dot3( tempNormal, matY );
		normal.z = dot3( tempNormal, matZ );
		vec4 tempTangent = tangent;
		tangent.x = dot3( tempTangent, matX );
		tangent.y = dot3( tempTangent, matY );
		tangent.z = dot3( tempTangent, matZ );
	}
	vec3 bitangent = cross( normal.xyz, tangent.xyz ) * tangent.w;
	vofi_TexCoord0.x = dot4( position, _va_[2 ] );
	vofi_TexCoord0.y = dot4( position, _va_[3 ] );
	vofi_TexCoord0.z = dot4( position, _va_[4 ] );
	vofi_TexCoord0.w = dot4( position, _va_[5 ] );
	vec2 texcoord0 = in_TexCoord * _va_[6 ].xy + _va_[6 ].zw;
	vofi_TexCoord1 = texcoord0.xyxy;
	vofi_TexCoord2.x = dot4( position, _va_[7 ] );
	vofi_TexCoord2.y = dot4( position, _va_[8 ] );
	vofi_TexCoord2.z = dot4( position, _va_[9 ] );
	vofi_TexCoord2.w = 1.0;
	vofi_TexCoord3.x = dot3( tangent, _va_[7 ] );
	vofi_TexCoord3.y = dot3( bitangent, _va_[7 ] );
	vofi_TexCoord3.z = dot3( normal, _va_[7 ] );
	vofi_TexCoord3.w = 1.0;
	vofi_TexCoord4.x = dot3( tangent, _va_[8 ] );
	vofi_TexCoord4.y = dot3( bitangent, _va_[8 ] );
	vofi_TexCoord4.z = dot3( normal, _va_[8 ] );
	vofi_TexCoord4.w = 1.0;
	vofi_TexCoord6.x = dot3( tangent, _va_[9 ] );
	vofi_TexCoord6.y = dot3( bitangent, _va_[9 ] );
	vofi_TexCoord6.z = dot3( normal, _va_[9 ] );
	vofi_TexCoord6.w = 1.0;
	vofi_TexCoord5.x = dot4( position, _va_[10 ] );
	vofi_TexCoord5.y = dot4( position, _va_[11 ] );
	vofi_TexCoord5.z = dot4( position, _va_[12 ] );
	vofi_TexCoord5.w = dot4( position, _va_[13 ] );
	gl_Position.x = dot4( position, _va_[14 ] );
	gl_Position.y = dot4( position, _va_[15 ] );
	gl_Position.z = dot4( position, _va_[16 ] );
	gl_Position.w = dot4( position, _va_[17 ] );
	
	gl_Position.x += g_eye * -g_eye_separation/48 * (gl_Position.w - 39.781235);
}