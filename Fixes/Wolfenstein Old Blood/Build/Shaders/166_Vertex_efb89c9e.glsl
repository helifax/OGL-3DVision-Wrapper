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

float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0.0, 1.0 ), b ); }
uniform vec4 _va_ [22];
uniform vec4 hardwareSkinning ;
uniform matrices_ubo { vec4 matrices[768]; };

in vec2 in_TexCoord;
in vec4 in_Position;
in vec4 in_Position1;
in vec4 in_Morph;
in vec4 in_Normal;
in vec4 in_Tangent;
in vec4 in_Color;
in vec4 in_Weights81;
in vec3 in_Weights82;
in vec4 in_Weights83;

out vec4 vofi_TexCoord7;
out vec4 vofi_TexCoord6;

void main() {
	vec4 texcoord0;
	{
		texcoord0 = ( in_TexCoord.xy * _va_[0 ].xy + _va_[0 ].zw ).xyxy;
	};
	vofi_TexCoord7 = texcoord0 * vec4( _va_[1 ].xy, 1.0, 1.0 );
	{
		vec4 localPosition;
		{
			localPosition = in_Position * _va_[2 ] + _va_[3 ];
			if ( hardwareSkinning.z != 0.0 ) {
				localPosition += _va_[4 ].x * in_Position1;
			}
			if ( hardwareSkinning.x != 0.0 ) {
				vec4 vertexPosition = localPosition;
				if ( hardwareSkinning.y != 0.0 ) {
					vec3 morphVector = ( in_Morph.xyz * 2 ) - 1;
					vertexPosition.xyz += ( morphVector * hardwareSkinning.y );
				}
				if ( hardwareSkinning.w == 0.0 ) {
					float weights = in_Normal.w * 255;
					float hiweight = in_Tangent.w * 255.0;
					float w1 = hiweight - floor( hiweight / 128.0 ) * 128.0;
					float w2 = floor( weights / 16.0 );
					float w3 = weights - ( w2 * 16.0 );
					w1 *= 1.0 / 254.0;
					w2 *= 1.0 / 45.0;
					w3 *= 1.0 / 60.0;
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
					localPosition.x = dot4( vertexPosition, matX );
					localPosition.y = dot4( vertexPosition, matY );
					localPosition.z = dot4( vertexPosition, matZ );
					localPosition.w = 1.0;
				} else {
					vec4 matX, matY, matZ;
					vec4 weights1 = in_Weights81;
					vec3 weights2 = in_Weights82;
					float w0 = 1.0 - ( weights1.x + weights1.y + weights1.z + weights1.w + weights2.x + weights2.y + weights2.z );
					int joint = int(in_Color.r * 255) * 3;
					matX = matrices[joint+0] * w0;
					matY = matrices[joint+1] * w0;
					matZ = matrices[joint+2] * w0;
					joint = int(in_Color.g * 255) * 3;
					matX += matrices[joint+0] * weights1.x;
					matY += matrices[joint+1] * weights1.x;
					matZ += matrices[joint+2] * weights1.x;
					joint = int(in_Color.b * 255) * 3;
					matX += matrices[joint+0] * weights1.y;
					matY += matrices[joint+1] * weights1.y;
					matZ += matrices[joint+2] * weights1.y;
					joint = int(in_Color.a * 255) * 3;
					matX += matrices[joint+0] * weights1.z;
					matY += matrices[joint+1] * weights1.z;
					matZ += matrices[joint+2] * weights1.z;
					joint = int(in_Weights83.x * 255.0) * 3;
					matX += matrices[joint+0] * weights1.w;
					matY += matrices[joint+1] * weights1.w;
					matZ += matrices[joint+2] * weights1.w;
					joint = int(in_Weights83.y * 255.0) * 3;
					matX += matrices[joint+0] * weights2.x;
					matY += matrices[joint+1] * weights2.x;
					matZ += matrices[joint+2] * weights2.x;
					joint = int(in_Weights83.z * 255.0) * 3;
					matX += matrices[joint+0] * weights2.y;
					matY += matrices[joint+1] * weights2.y;
					matZ += matrices[joint+2] * weights2.y;
					joint = int(in_Weights83.w * 255.0) * 3;
					matX += matrices[joint+0] * weights2.z;
					matY += matrices[joint+1] * weights2.z;
					matZ += matrices[joint+2] * weights2.z;
					localPosition.x = dot4( vertexPosition, matX );
					localPosition.y = dot4( vertexPosition, matY );
					localPosition.z = dot4( vertexPosition, matZ );
					localPosition.w = 1.0;
				}
			}
			gl_Position.x = dot4( localPosition, _va_[5 ] );
			gl_Position.y = dot4( localPosition, _va_[6 ] );
			gl_Position.z = dot4( localPosition, _va_[7 ] );
			gl_Position.w = dot4( localPosition, _va_[8 ] );
			//gl_Position.x += g_eye * g_eye_separation * (gl_Position.w - g_convergence);

 if(g_vertexEnabled < 1.0)
{gl_Position = vec4(0.0);}

		};
	};
	{
		vec3 vertexPos = in_Position.xyz;
		vofi_TexCoord6 = vec4( 0 );
		float wound3A = ( length( ( _va_[9 ].xyz - vertexPos.xyz ) * _va_[9 ].w ) - 1.0 );
		float wound3B = ( length( ( _va_[10 ].xyz - vertexPos.xyz ) * _va_[10 ].w ) - 1.0 );
		float wound3C = ( length( ( _va_[11 ].xyz - vertexPos.xyz ) * _va_[11 ].w ) - 1.0 );
		float wound3D = ( length( ( _va_[12 ].xyz - vertexPos.xyz ) * _va_[12 ].w ) - 1.0 );
		float wound3E = ( length( ( _va_[13 ].xyz - vertexPos.xyz ) * _va_[13 ].w ) - 1.0 );
		float wound3F = ( length( ( _va_[14 ].xyz - vertexPos.xyz ) * _va_[14 ].w ) - 1.0 );
		float wound3G = ( length( ( _va_[15 ].xyz - vertexPos.xyz ) * _va_[15 ].w ) - 1.0 );
		float wound3H = ( length( ( _va_[16 ].xyz - vertexPos.xyz ) * _va_[16 ].w ) - 1.0 );
		float wound3I = ( length( ( _va_[17 ].xyz - vertexPos.xyz ) * _va_[17 ].w ) - 1.0 );
		float wound3J = ( length( ( _va_[18 ].xyz - vertexPos.xyz ) * _va_[18 ].w ) - 1.0 );
		vec3 ellipsoid;
		vec3 localPos = vertexPos.xyz - vec3( _va_[19 ].w, _va_[20 ].w, _va_[21 ].w );
		ellipsoid.x = dot( _va_[19 ].xyz, localPos.xyz );
		ellipsoid.y = dot( _va_[20 ].xyz, localPos.xyz );
		ellipsoid.z = dot( _va_[21 ].xyz, localPos.xyz );
		float distToEllipsoid = length( ellipsoid ) - 1.0;
		vofi_TexCoord6.w = min( wound3A, wound3B );
		vofi_TexCoord6.w = min( vofi_TexCoord6.w, wound3C );
		vofi_TexCoord6.w = min( vofi_TexCoord6.w, wound3D );
		vofi_TexCoord6.w = min( vofi_TexCoord6.w, wound3E );
		vofi_TexCoord6.w = min( vofi_TexCoord6.w, wound3F );
		vofi_TexCoord6.w = min( vofi_TexCoord6.w, wound3G );
		vofi_TexCoord6.w = min( vofi_TexCoord6.w, wound3H );
		vofi_TexCoord6.w = min( vofi_TexCoord6.w, distToEllipsoid );
		vofi_TexCoord6.z = min( min( wound3I, wound3J ), vofi_TexCoord6.w );
	};
}