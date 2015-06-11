#version 150

// BLOOD Decals


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
vec4 swizzleColor ( vec4 color ) { return color; }
uniform vec4 _va_ [11];

in vec4 in_Normal;
in vec4 in_Tangent;
in vec4 in_Position;
in vec2 in_TexCoord;
in vec4 in_Color;

out vec4 vofi_TexCoord4;
out vec4 vofi_TexCoord1;
out vec4 vofi_TexCoord2;
out vec4 vofi_TexCoord3;
out vec4 vofi_Color;
out vec4 vofi_TexCoord0;
out vec4 vofi_TexCoord5;

void main() {
	vec4 snormal = in_Normal * 2.0 - 1.0;
	vec4 stangent = in_Tangent * 2.0 - 1.0;
	vec3 normal = normalize( snormal.xyz );
	vec3 tangent = normalize( stangent.xyz - normal * dot( normal, stangent.xyz ) );
	vec3 bitangent = cross( normal, tangent );
	gl_Position.x = dot4( in_Position, _va_[0 ] );
	gl_Position.y = dot4( in_Position, _va_[1 ] );
	gl_Position.z = dot4( in_Position, _va_[2 ] );
	gl_Position.w = dot4( in_Position, _va_[3 ] );
	gl_Position.x += g_eye * g_eye_separation * (gl_Position.w - g_convergence);

 if(g_vertexEnabled < 1.0)
{gl_Position = vec4(0.0);}

	vec2 scaleBiasX;
	scaleBiasX.x = floor( in_TexCoord.x / _va_[4 ].x );
	scaleBiasX.y = in_TexCoord.x - ( scaleBiasX.x * _va_[4 ].x );
	vec2 scaleBiasY;
	scaleBiasY.x = floor( in_TexCoord.y / _va_[4 ].y );
	scaleBiasY.y = in_TexCoord.y - ( scaleBiasY.x * _va_[4 ].y );
	vofi_TexCoord0.xy = scaleBiasX / _va_[4 ].x;
	vofi_TexCoord0.zw = scaleBiasY / _va_[4 ].y;
	vofi_TexCoord1.xyz = tangent;
	vofi_TexCoord1.w = 0.0;
	vofi_TexCoord2.xyz = bitangent;
	vofi_TexCoord2.w = 0.0;
	vofi_TexCoord3.xyz = normal;
	vofi_TexCoord3.w = 0.0;
	float s = in_Normal.w * 255.1;
	vec3 signs;
	signs.z = floor( s / 4 );
	signs.y = floor( s / 2 ) - ( signs.z * 2 );
	signs.x = s - ( signs.y * 2 ) - ( signs.z * 4 );
	signs = signs * 2.0 - 1.0;
	vec4 col = swizzleColor( in_Color );
	vec3 sizes = col.xyz * 255.0;
	sizes *= signs;
	vec3 origin = in_Position.xyz - sizes;
	vofi_TexCoord4.x = origin.x;
	vofi_TexCoord4.y = origin.y;
	vofi_TexCoord4.z = origin.z;
	vofi_TexCoord4.w = 0.0;
	{
		float FLT_EPSILON = 0.01;
		vec3 cameraToPixel = in_Position.xyz - _va_[5 ].xyz;
		float distanceToPixel = length( cameraToPixel );
		float be = _va_[6 ].x * exp2( - _va_[7 ].w * ( _va_[5 ].z - _va_[7 ].x ) );
		float distanceInFog = max( distanceToPixel - _va_[6 ].y, 0.0 );
		float effectiveZ = max( cameraToPixel.z, FLT_EPSILON );
		if ( cameraToPixel.z < -FLT_EPSILON ) {
			effectiveZ = cameraToPixel.z;
		}
		float lineIntegral = be * distanceInFog;
		lineIntegral *= ( 1.0 - exp2( - _va_[7 ].w * effectiveZ ) ) / ( _va_[7 ].w * effectiveZ );
		float cosL = dot( _va_[8 ].xyz, normalize( cameraToPixel ) );
		float inscatterAlpha = 1.0 - saturate( ( cosL - _va_[7 ].y ) * _va_[7 ].z );
		vofi_TexCoord5.xyz = mix( _va_[9 ].xyz, _va_[10 ].xyz, inscatterAlpha );
		vofi_TexCoord5.w = 1.0 - max( saturate( exp2( -lineIntegral ) ), _va_[6 ].z );
	};
	vec3 pos = in_Position.xyz - origin;
	vec3 localPos;
	localPos.x = dot3( pos, tangent );
	localPos.y = dot3( pos, bitangent );
	localPos.z = dot3( pos, normal );
	col.xyz = 1.0 / abs( localPos );
	vofi_Color = col;
}
