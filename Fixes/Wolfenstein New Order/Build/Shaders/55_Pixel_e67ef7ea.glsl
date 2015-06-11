#version 150
// Some shadows.. somehow
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
float shadow2Ddepth ( sampler2DShadow image, vec3 texcoord ) { return tex2D( image, texcoord ).x; }
vec4 tex2DMSFetch ( sampler2DMS image, ivec2 texcoord, int sampleIndex ) { return texelFetch( image, texcoord, sampleIndex ); }
vec2 screenPosToTexcoord ( vec2 pos, vec4 bias_scale ) { return ( pos * bias_scale.zw + bias_scale.xy ); }
uniform vec4 _fa_ [19];
uniform sampler2D samp_viewdepthmap;
uniform sampler2D samp_lightprojectmap;
uniform sampler2D samp_lightfalloffmap;
uniform sampler2DMS samp_viewnormalmapms;
uniform sampler2D samp_viewnormalmap;
uniform sampler2DShadow samp_lightdepthcomparemap;

in vec4 gl_FragCoord;

out vec4 out_FragColor0;

void main() {
	vec2 viewTexCoord = screenPosToTexcoord( gl_FragCoord.xy, _fa_[0 ] );
	vec4 window;
	window.z = tex2Ddepth( samp_viewdepthmap, viewTexCoord );
	window.w = 1.0;
	window.xy = screenPosToTexcoord( gl_FragCoord.xy, _fa_[1 ] );
	vec4 texcoord;
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


	vec4 lpm = h4tex2D( samp_lightprojectmap, texcoord.xy );
	vec4 lfm = h4tex2D( samp_lightfalloffmap, vec2( texcoord.z, 0.5 ) );
	vec4 globalPosition;
	globalPosition.x = dot4( window, _fa_[6 ] );
	globalPosition.y = dot4( window, _fa_[7 ] );
	globalPosition.z = dot4( window, _fa_[8 ] );
	globalPosition.w = dot4( window, _fa_[9 ] );
	globalPosition.xyz /= globalPosition.w;
	globalPosition.w = 1.0;
	vec3 lightDir = _fa_[10 ].xyz - globalPosition.xyz;
	vec3 lightDirNormalized = normalize( lightDir );
	vec3 viewNormal; if ( _fa_[11 ].x > 1.0 ) {
		viewNormal = tex2DMSFetch( samp_viewnormalmapms, ivec2( gl_FragCoord.xy ), 0 ).xyz;
	} else {
		viewNormal = tex2D( samp_viewnormalmap, viewTexCoord.xy ).xyz;
	}
	vec3 globalNormal = normalize( viewNormal * vec3( 2.0 ) - vec3( 1.0 ) );
	float lightFactor = saturate( dot3( globalNormal, lightDirNormalized ) );
	vec4 color = lpm * lfm * _fa_[12 ];
	color.xyz *= vec3( _fa_[13 ].x * lightFactor );
	color.xyz *= vec3( lightFactor );
	vec4 shadowTexCoord;
	shadowTexCoord.x = dot4( window, _fa_[14 ] );
	shadowTexCoord.y = dot4( window, _fa_[15 ] );
	shadowTexCoord.z = dot4( window, _fa_[16 ] );
	shadowTexCoord.w = dot4( window, _fa_[17 ] );
	
	
	
	float shadow;
	{
		vec3 texCoord = shadowTexCoord.xyz / shadowTexCoord.w;
		{
			float s = _fa_[18 ].y * 2.0;
			vec4 shadow4;
			{
				shadow4.x = shadow2Ddepth( samp_lightdepthcomparemap, texCoord.xyz + s * vec3( -0.375, -0.125, 0 ) );
			};
			{
				shadow4.y = shadow2Ddepth( samp_lightdepthcomparemap, texCoord.xyz + s * vec3( -0.125, 0.375, 0 ) );
			};
			{
				shadow4.z = shadow2Ddepth( samp_lightdepthcomparemap, texCoord.xyz + s * vec3( 0.125, -0.375, 0 ) );
			};
			{
				shadow4.w = shadow2Ddepth( samp_lightdepthcomparemap, texCoord.xyz + s * vec3( 0.375, 0.125, 0 ) );
			};
			shadow = dot( shadow4, vec4( 0.25 ) );
		};
	};
	color.xyz *= vec3( saturate( shadow ) );
	out_FragColor0.xyz = saturate( color.xyz * color.w );
	out_FragColor0.w = 1.0;
 if(g_pixelEnabled < 1.0)
{out_FragColor0 = vec4(0.0);}

}