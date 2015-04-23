
#include "opengl32_ARB_Variant.h"

// Currently ShaderSource ARB Variant returns junk characters at the end of the string.....

// Add the stereo values
// OpenGL 2.0
void sys_glUseProgramObjectARB(GLhandleARB program)
{
	add_log("Using ARB Shader");

	// Get the shaderManager
	ShaderManager * shaderManager = ShaderManager::getInstance();

	//Apply the shaders
	shaderManager->ApplyExceptionShaders();

#ifdef DEBUG_WRAPPER
	shaderManager->ApplyDebugExceptionShaders();
#endif

	// Use the original program
	(*orig_glUseProgramObjectARB)(program);

	// Get the locations of the uniforms
	GLfloat value;
	const GLchar *uniform_eye = (GLchar*)"g_eye";
	const GLchar *uniform_eye_spearation = (GLchar*)"g_eye_separation";
	const GLchar *uniform_convergence = (GLchar*)"g_convergence";


	// Get our functions from the driver. Be sure to only take them once
	if (orig_glGetUniformLocation == NULL)
	{
		orig_glGetUniformLocation = (func_glGetUniformLocation_t)(orig_wglGetProcAddress)("glGetUniformLocation");
	}
	//glUniform1f
	if (orig_glUniform1f == NULL)
	{

		orig_glUniform1f = (func_glUniform1f_t)(orig_wglGetProcAddress)("glUniform1f");
	}

	if ((GLint)program >= 0)
	{
		GLint location_eye = (*orig_glGetUniformLocation)(program, uniform_eye);
		GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
		GLint location_convergence = (*orig_glGetUniformLocation)(program, uniform_convergence);

		//set the uniform inside the shaders
		if (NV3DVisionGetCurrentFrame() == 1)
		{
			//left eye
			value = -1.0f;
			(*orig_glUniform1f)(location_eye, value);
		}
		else
		{
			//right eye
			value = +1.0f;
			(*orig_glUniform1f)(location_eye, value);
		}
		//Set the Separation and convergence
		(*orig_glUniform1f)(location_eye_separation, Get3DEyeSeparation());
		(*orig_glUniform1f)(location_convergence, Get3DConvergence());
	}

	/////////////////////////////////////
	// USED TO DISABLE ALOT OF SHADERS
	/////////////////////////////////////
#ifdef	DEBUG_WRAPPER
	if (shaderManager->VertexShadersExceptionsEnabled())
	{
		if (((GLint)program >= (GLint)shaderManager->GetExceptionShaderVertexStart() && (GLint)program <= (GLint)shaderManager->GetExceptionShaderVertexEnd()))
		{
			GLint location_eye_separation = (*orig_glGetUniformLocation)(program, uniform_eye_spearation);
			GLint location_convergence = (*orig_glGetUniformLocation)(program, uniform_convergence);

			//Set the Separation and convergence
			(*orig_glUniform1f)(location_eye_separation, 0);
			(*orig_glUniform1f)(location_convergence, 0);
		}
	}
#endif	
}
///-------------------------------------------------------------------------------------------
