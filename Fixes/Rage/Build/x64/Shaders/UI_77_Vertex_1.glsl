#version 150

uniform float g_depth;
uniform float g_eye;
uniform float g_eye_separation;
uniform float g_convergence;

float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
vec2 saturate( vec2 v ) { return clamp( v, 0.0, 1.0 ); }
vec3 saturate( vec3 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 saturate( vec4 v ) { return clamp( v, 0.0, 1.0 ); }



in vec4 in_Position;

out vec4 gl_Position;

void main() {
	gl_Position = in_Position;
	gl_Position.x += g_eye * -g_eye_separation/48 * (gl_Position.w - 39.781235);
}