#version 150 
uniform float g_pixelEnabled;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;


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
float dot4 ( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4 ( vec2 a, vec4 b ) { return dot( vec4( a, 0.0, 1.0 ), b ); }
vec4 h4tex2D ( sampler2D image, vec2 texcoord ) { return tex2D( image, texcoord ); }
float tex2Ddepth ( sampler2D image, vec2 texcoord ) { return tex2D( image, texcoord ).x; }
vec4 tex2DMSFetch ( sampler2DMS image, ivec2 texcoord, int sampleIndex ) { return texelFetch( image, texcoord, sampleIndex ); }
vec2 screenPosToTexcoord ( vec2 pos, vec4 bias_scale ) { return ( pos * bias_scale.zw + bias_scale.xy ); }
uniform vec4 _fa_ [21];
uniform sampler2DMS samp_viewdepthstencilmap;
uniform sampler2D samp_lightprojectmap;
uniform sampler2D samp_lightfalloffmap;
uniform sampler2DMS samp_viewnormalmapms;
uniform sampler2D samp_viewdepthmap;
uniform sampler2D samp_viewnormalmap;

in vec4 gl_FragCoord;

out vec4 out_FragColor0;

void main() {
	vec4 window;
	window.xy = screenPosToTexcoord( gl_FragCoord.xy, _fa_[0 ] );
	window.w = 1.0;

	vec4 texcoord;
	vec4 lpm = vec4( 0.0 );
	vec4 lfm = vec4( 0.0 );
	vec4 color = vec4( 0.0 ); if ( _fa_[1 ].x > 1.0 )
	{
		int numSamples = int( _fa_[1 ].x );
		ivec2 viewTexCoordMS = ivec2( gl_FragCoord.xy );
		for ( int sampleId = 0; sampleId < numSamples; sampleId++ ) {
			window.z = tex2DMSFetch( samp_viewdepthstencilmap, viewTexCoordMS, sampleId ).x;
			
			vec4 lpmSample = vec4( 0.0 );
			vec4 lfmSample = vec4( 0.0 );
			{
				vec4 tc;
				{
					tc.x = dot4( window, _fa_[2 ] );
					tc.y = dot4( window, _fa_[3 ] );
					tc.z = dot4( window, _fa_[4 ] );
					tc.w = dot4( window, _fa_[5 ] );
					tc.xyz /= tc.w;

mat4 inverseMVP = mat4(_fa_[2], _fa_[3], _fa_[4], _fa_[5]);
mat4 MVP = inverse(inverseMVP);
tc = vec4(window * inverseMVP);
tc /= tc.w;
	
tc.w = normalize(tc.w);
vec4 WP_temp = tc * MVP;
WP_temp.x -= g_eye * g_eye_separation * (WP_temp.w - g_convergence) * 0.5;
tc = vec4(WP_temp * inverseMVP);
tc /= tc.w;						
					
				};
				lpmSample = h4tex2D( samp_lightprojectmap, tc.xy );
				lfmSample = h4tex2D( samp_lightfalloffmap, vec2( tc.z, 0.5 ) );
			};
			vec4 globalPosition;
			{
				globalPosition.x = dot4( window, _fa_[6 ] );
				globalPosition.y = dot4( window, _fa_[7 ] );
				globalPosition.z = dot4( window, _fa_[8 ] );
				globalPosition.w = dot4( window, _fa_[9 ] );
				globalPosition.xyz /= globalPosition.w;
				globalPosition.w = 1.0;
				
mat4 inverseMVP = mat4(_fa_[6], _fa_[7], _fa_[8], _fa_[9]);
mat4 MVP = inverse(inverseMVP);
globalPosition = vec4(window * inverseMVP);
globalPosition /= globalPosition.w;
	
globalPosition.w = normalize(globalPosition.w);
vec4 WP_temp = globalPosition * MVP;
WP_temp.x -= g_eye * g_eye_separation * (WP_temp.w - g_convergence) * 0.5;
globalPosition = vec4(WP_temp * inverseMVP);
globalPosition /= globalPosition.w;		
				
				{
					vec4 temp;
					temp.x = dot4( globalPosition, _fa_[10 ] );
					temp.y = dot4( globalPosition, _fa_[11 ] );
					temp.z = dot4( globalPosition, _fa_[12 ] );
					temp.w = dot4( globalPosition, _fa_[13 ] );
					float scalehack = mix( 10.0, 1.0, saturate( 0.1 * 0.1 * dot3( temp.xyz, temp.xyz ) ) );
					globalPosition.x = dot4( temp, _fa_[14 ] * vec4( scalehack, 1.0, 1.0, 1.0 ) );
					globalPosition.y = dot4( temp, _fa_[15 ] * vec4( 1.0, scalehack, 1.0, 1.0 ));
					globalPosition.z = dot4( temp, _fa_[16 ] * vec4( 1.0, 1.0, scalehack, 1.0 ));
				}
			};
			vec3 lightDir = _fa_[17 ].xyz - globalPosition.xyz;
			vec3 lightDirNormalized = normalize( lightDir );
			vec3 viewNormal = tex2DMSFetch( samp_viewnormalmapms, viewTexCoordMS, sampleId ).xyz;
			vec3 globalNormal = normalize( viewNormal * vec3( 2.0 ) - vec3( 1.0 ) );
			float lightFactor = saturate( dot3( globalNormal, lightDirNormalized ) );
			vec4 sampleLightColor = lpmSample * lfmSample * _fa_[18 ];
			sampleLightColor.xyz *= vec3( _fa_[19 ].x * lightFactor );
			sampleLightColor.xyz *= vec3( lightFactor );
			color += sampleLightColor;
			lpm += lpmSample;
			lfm += lfmSample;
		}
		window.z = tex2DMSFetch( samp_viewdepthstencilmap, viewTexCoordMS, 0 ).x;
		{
			texcoord.x = dot4( window, _fa_[2 ] );
			texcoord.y = dot4( window, _fa_[3 ] );
			texcoord.z = dot4( window, _fa_[4 ] );
			texcoord.w = dot4( window, _fa_[5 ] );
			texcoord.xyz /= texcoord.w;

mat4 inverseMVP = mat4(_fa_[2], _fa_[3], _fa_[4], _fa_[5]);
mat4 MVP = inverse(inverseMVP);
texcoord = vec4(window * inverseMVP);
texcoord /= texcoord.w;
	
texcoord.w = normalize(texcoord.w);
vec4 WP_temp = texcoord * MVP;
WP_temp.x -= g_eye * g_eye_separation * (WP_temp.w - g_convergence) * 0.5;
texcoord = vec4(WP_temp * inverseMVP);
texcoord /= texcoord.w;						
			
		};
		color *= vec4( 1.0 / _fa_[1 ].x );
		lpm *= vec4( 1.0 / _fa_[1 ].x );
		lfm *= vec4( 1.0 / _fa_[1 ].x );
	}
	else
	{
		vec2 viewTexCoord = screenPosToTexcoord( gl_FragCoord.xy, _fa_[20 ] );
		window.z = tex2Ddepth( samp_viewdepthmap, viewTexCoord );
		{
			texcoord.x = dot4( window, _fa_[2 ] );
			texcoord.y = dot4( window, _fa_[3 ] );
			texcoord.z = dot4( window, _fa_[4 ] );
			texcoord.w = dot4( window, _fa_[5 ] );
			texcoord.xyz /= texcoord.w;
			
mat4 inverseMVP = mat4(_fa_[2], _fa_[3], _fa_[4], _fa_[5]);
mat4 MVP = inverse(inverseMVP);
texcoord = vec4(window * inverseMVP);
texcoord /= texcoord.w;
	
texcoord.w = normalize(texcoord.w);
vec4 WP_temp = texcoord * MVP;
WP_temp.x -= g_eye * g_eye_separation * (WP_temp.w - g_convergence) * 0.5;
texcoord = vec4(WP_temp * inverseMVP);
texcoord /= texcoord.w;	
			
		};
		{
			vec4 tc;
			{
				tc.x = dot4( window, _fa_[2 ] );
				tc.y = dot4( window, _fa_[3 ] );
				tc.z = dot4( window, _fa_[4 ] );
				tc.w = dot4( window, _fa_[5 ] );
				tc.xyz /= tc.w;
				
mat4 inverseMVP = mat4(_fa_[2], _fa_[3], _fa_[4], _fa_[5]);
mat4 MVP = inverse(inverseMVP);
tc = vec4(window * inverseMVP);
tc /= tc.w;
	
tc.w = normalize(tc.w);
vec4 WP_temp = tc * MVP;
WP_temp.x -= g_eye * g_eye_separation * (WP_temp.w - g_convergence) * 0.5;
tc = vec4(WP_temp * inverseMVP);
tc /= tc.w;					
				
			};
			lpm = h4tex2D( samp_lightprojectmap, tc.xy );
			lfm = h4tex2D( samp_lightfalloffmap, vec2( tc.z, 0.5 ) );
		};
		vec4 globalPosition;
		{
			globalPosition.x = dot4( window, _fa_[6 ] );
			globalPosition.y = dot4( window, _fa_[7 ] );
			globalPosition.z = dot4( window, _fa_[8 ] );
			globalPosition.w = dot4( window, _fa_[9 ] );
			globalPosition.xyz /= globalPosition.w;
			globalPosition.w = 1.0;
			
mat4 inverseMVP = mat4(_fa_[6], _fa_[7], _fa_[8], _fa_[9]);
mat4 MVP = inverse(inverseMVP);
globalPosition = vec4(window * inverseMVP);
globalPosition /= globalPosition.w;
	
globalPosition.w = normalize(globalPosition.w);
vec4 WP_temp = globalPosition * MVP;
WP_temp.x -= g_eye * g_eye_separation * (WP_temp.w - g_convergence) * 0.5;
globalPosition = vec4(WP_temp * inverseMVP);
globalPosition /= globalPosition.w;		
			
			{
				vec4 temp;
				temp.x = dot4( globalPosition, _fa_[10 ] );
				temp.y = dot4( globalPosition, _fa_[11 ] );
				temp.z = dot4( globalPosition, _fa_[12 ] );
				temp.w = dot4( globalPosition, _fa_[13 ] );
				float scalehack = mix( 10.0, 1.0, saturate( 0.1 * 0.1 * dot3( temp.xyz, temp.xyz ) ) );
				globalPosition.x = dot4( temp, _fa_[14 ] * vec4( scalehack, 1.0, 1.0, 1.0 ) );
				globalPosition.y = dot4( temp, _fa_[15 ] * vec4( 1.0, scalehack, 1.0, 1.0 ));
				globalPosition.z = dot4( temp, _fa_[16 ] * vec4( 1.0, 1.0, scalehack, 1.0 ));
			}
		};
		vec3 lightDir = _fa_[17 ].xyz - globalPosition.xyz;
		vec3 lightDirNormalized = normalize( lightDir );
		vec3 viewNormal = tex2D( samp_viewnormalmap, viewTexCoord.xy ).xyz;
		vec3 globalNormal = normalize( viewNormal * vec3( 2.0 ) - vec3( 1.0 ) );
		float lightFactor = saturate( dot3( globalNormal, lightDirNormalized ) );
		color = lpm * lfm * _fa_[18 ];
		color.xyz *= vec3( _fa_[19 ].x * lightFactor );
		color.xyz *= vec3( lightFactor );
	}
	out_FragColor0 = saturate( color * color.w );
 if(g_pixelEnabled < 1.0)
{out_FragColor0 = vec4(0.0);}

}