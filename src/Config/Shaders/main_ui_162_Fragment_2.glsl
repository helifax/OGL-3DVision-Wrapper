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

vec3 pow3 ( vec3 a, float b ) { return vec3( pow( a.x, b ), pow( a.y, b ), pow( a.z, b ) ); }
uniform vec4 _fa_ [5];
uniform sampler2D samp_transmap;

in vec4 vofi_TexCoord0;
in vec4 vofi_Color;

out vec4 out_FragColor0;

void main() {
	float pixelDeltaScalar = 32.0;
	float edgeThreshold = 0.5;
	float distanceVal = tex2D( samp_transmap, vofi_TexCoord0.xy ).a;
	vec2 tc = vofi_TexCoord0.xy;
	float pixelDelta = ( abs( dFdx( tc.x ) ) + abs( dFdy( tc.y ) ) ) * pixelDeltaScalar;
	pixelDelta = clamp( pixelDelta, 0.0, edgeThreshold );
	float fontGlyphEdgeMin = edgeThreshold - pixelDelta;
	float fontGlyphEdgeMax = edgeThreshold + pixelDelta;
	float alpha = smoothstep( fontGlyphEdgeMin, fontGlyphEdgeMax, distanceVal );
	vec3 color = vofi_Color.xyz; if ( _fa_[0 ].x > 0.0 ) {
		float sharpOutlineDistanceFade = smoothstep( 0.15, 0.25, pixelDelta );
		float outlineThickness = _fa_[0 ].x;
		float outlineThicknessRange = 0.5;
		float outlineThicknessBlendRange = 0.05;
		float outlineMinDistance = outlineThicknessRange * ( 1.0 - outlineThickness );
		float outlineMaxDistance = outlineMinDistance + outlineThicknessBlendRange;
		float outlineAlpha = smoothstep( outlineMinDistance, outlineMaxDistance, distanceVal );
		vec3 finalOutlineColor = mix( ( _fa_[1 ].xyz * outlineAlpha ), color, alpha );
		float finalOutlineAlpha = max( alpha, outlineAlpha * _fa_[1 ].a );
		color = mix( finalOutlineColor, color, sharpOutlineDistanceFade );
		alpha = mix( finalOutlineAlpha, alpha, sharpOutlineDistanceFade );
	} if ( _fa_[0 ].y > 0.0 ) {
		vec2 glowTC = vofi_TexCoord0.xy + ( _fa_[2 ].xy / _fa_[0 ].zw );
		vec4 glowTexel = tex2D( samp_transmap, glowTC );
		float glowThickness = _fa_[0 ].y;
		float glowThicknessRange = 0.25;
		float glowThicknessBlendRange = 0.5;
		float glowMinDistance = glowThicknessRange * ( 1.0 - glowThickness );
		float glowMaxDistance = glowMinDistance + glowThicknessBlendRange;
		float glowAlpha = smoothstep( glowMinDistance, glowMaxDistance, glowTexel.a );
		vec3 finalGlow = _fa_[3 ].xyz;
		color = mix( finalGlow, color, alpha );
		alpha = max( alpha, glowAlpha * _fa_[3 ].a );
	}
	out_FragColor0.xyz = pow3( saturate( color ), _fa_[4 ].x );
	out_FragColor0.w = alpha * vofi_Color.w;
}