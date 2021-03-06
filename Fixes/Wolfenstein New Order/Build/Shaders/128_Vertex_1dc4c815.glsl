#version 150
// Shadows related
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
				float weights = in_Normal.w * 255;
				float hiweight = in_Tangent.w * 255.0;
				float w1 = hiweight - floor( hiweight / 128.0 ) * 128.0;
				float w2 = floor( weights / 16.0 );
				float w3 = weights - ( w2 * 16.0 );
				w1 *= 1.0 / 254.0;
				w2 *= 1.0 / 45.0;
				w3 *= 1.0 / 60.0;
				float w0 = 1 - ( w1 + w2 + w3 );
				if ( hardwareSkinning.w == 0.0 ) {
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
					vec4 dualQuats[ 8 ];
					float joint = in_Color.r * 255;
					dualQuats[0] = matrices[ int(joint*2+0) ];
					dualQuats[1] = matrices[ int(joint*2+1) ];
					joint = in_Color.g * 255;
					dualQuats[2] = matrices[ int(joint*2+0) ];
					dualQuats[3] = matrices[ int(joint*2+1) ];
					joint = in_Color.b * 255;
					dualQuats[4] = matrices[ int(joint*2+0) ];
					dualQuats[5] = matrices[ int(joint*2+1) ];
					joint = in_Color.a * 255;
					dualQuats[6] = matrices[ int(joint*2+0) ];
					dualQuats[7] = matrices[ int(joint*2+1) ];
					if (dot4(dualQuats[0], dualQuats[2]) < 0.0)
					w1 = -w1;
					if (dot4(dualQuats[0], dualQuats[4]) < 0.0)
					w2 = -w2;
					if (dot4(dualQuats[0], dualQuats[6]) < 0.0)
					w3 = -w3;
					vec4 Trans0 = dualQuats[0] * w0;
					vec4 Trans1 = dualQuats[1] * w0;
					Trans0 += dualQuats[2] * w1;
					Trans1 += dualQuats[3] * w1;
					Trans0 += dualQuats[4] * w2;
					Trans1 += dualQuats[5] * w2;
					Trans0 += dualQuats[6] * w3;
					Trans1 += dualQuats[7] * w3;
					float invlen = 1.0/length(Trans0);
					Trans0 *= invlen;
					Trans1 *= invlen;
					vec4 LocPos = vertexPosition;
					localPosition.xyz = LocPos.xyz + 2.0 * cross(Trans0.xyz, cross(Trans0.xyz, LocPos.xyz) + Trans0.w * LocPos.xyz);
					localPosition.xyz += 2.0 * (Trans0.w * Trans1.xyz - Trans1.w * Trans0.xyz + cross(Trans0.xyz, Trans1.xyz));
					localPosition.w = 1.0;
				}
			}
			gl_Position.x = dot4( localPosition, _va_[5 ] );
			gl_Position.y = dot4( localPosition, _va_[6 ] );
			gl_Position.z = dot4( localPosition, _va_[7 ] );
			gl_Position.w = dot4( localPosition, _va_[8 ] );
// gl_Position.x += g_eye * g_eye_separation * (gl_Position.w - g_convergence);

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