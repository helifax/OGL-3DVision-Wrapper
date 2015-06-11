#include "include\ShaderManager.h"
#include <direct.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <thread>
#include <mutex>
#include "opengl32.h"

extern "C" {
#include "opengl_3dv.h"
}

extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glShaderSource_t orig_glShaderSource;

#ifdef DEBUG_WRAPPER
extern int debugVertexIndex;
extern int debugPixelIndex;
extern bool printCurrentShader;
extern bool isCurrentShaderVertex;
extern bool isCompileEnable;
#endif

// Local functions
static void ShaderCompilerPrintInfo(GLuint progId, GLuint shaderId, std::string shaderSource, GLuint shaderType);
static std::recursive_mutex m_compilerMutex;


#ifdef DEBUG_WRAPPER
bool ShaderManager::ApplyDebugExceptionShaders(void)
{
	bool isSet = false;

	m_compilerMutex.lock();

	// Get all the required functions
	orig_glCreateShader = (func_glCreateShader)orig_wglGetProcAddress("glCreateShader");
	if (orig_glCreateShader == 0x0)
		add_log("glCreateShader not found !!!!");
	orig_glCompileShader = (func_glCompileShader)orig_wglGetProcAddress("glCompileShader");
	if (orig_glCompileShader == 0x0)
		add_log("glCompileShader not found !!!!");
	orig_glGetShaderiv = (func_glGetShaderiv_t)orig_wglGetProcAddress("glGetShaderiv");
	if (orig_glGetShaderiv == 0x0)
		add_log("glGetShaderiv not found !!!!");
	orig_glGetAttachedShaders = (funct_glGetAttachedShaders_t)orig_wglGetProcAddress("glGetAttachedShaders");
	if (orig_glGetAttachedShaders == 0x0)
		add_log("orig_glGetAttachedShaders not found !!!!");
	orig_glAttachShader = (func_glAttachShader_t)orig_wglGetProcAddress("glAttachShader");
	if (orig_glAttachShader == 0x0)
		add_log("glAttachShader not found !!!!");
	orig_glDetachShader = (func_glDetachShader_t)orig_wglGetProcAddress("glDetachShader");
	if (orig_glDetachShader == 0x0)
		add_log("glDetachShader not found !!!!");
	orig_glDeleteShader = (func_glDeleteShader_t)orig_wglGetProcAddress("glDeleteShader");
	if (orig_glDeleteShader == 0x0)
		add_log("glDeleteShader not found !!!!");

	// select the current pixel or vertex
	int currentShaderIndex = 0;
	GLuint currentShaderType = 0;
	GLenum pairShaderType = 0;
	if (isCurrentShaderVertex)
	{
		currentShaderIndex = debugVertexIndex;
		currentShaderType = GL_VERTEX_SHADER;
		pairShaderType = GL_FRAGMENT_SHADER;
	}
	else
	{
		currentShaderIndex = debugPixelIndex;
		currentShaderType = GL_FRAGMENT_SHADER;
		pairShaderType = GL_VERTEX_SHADER;
	}

	if (!NV3DVisionIsNotInit() && isCompileEnable)
	{
		int shaderProgramIndex = 0;
		if ((shaderProgramIndex = ShaderProgramFound(currentShaderIndex, currentShaderType)) != -1)
		{
			// Read source file
			std::string newShaderSource;
			bool sourceExists = false;

			// Open the Source File
			std::ifstream shaderFile("DebugShaders/CurrentShaderSource.glsl");
			std::string shaderLine;

			//file is opened
			if (shaderFile.is_open())
			{
				// read one line at a time
				while (getline(shaderFile, shaderLine))
				{
					newShaderSource += shaderLine + "\n";
				}
				shaderFile.close();
				sourceExists = true;
			}


			GLint statusOk, LinkOK;
			GLuint newShaderId = 0;
			GLuint pairShaderId = 0;

			bool shaderIsCreated = false;
			bool pairIsCreated = false;
			GLsizei shaderCount;
			GLuint shaders[2] = { 0, 0 };
			(*orig_glGetAttachedShaders)(m_existingShaders[shaderProgramIndex].m_programId, 4, &shaderCount, shaders);

			switch (currentShaderType)
			{
			case GL_VERTEX_SHADER:
				newShaderId = shaders[0];
				pairShaderId = shaders[1];
				break;

			case GL_FRAGMENT_SHADER:
				newShaderId = shaders[1];
				pairShaderId = shaders[0];
				break;
			}

			if (newShaderId == 0)
			{
				newShaderId = (*orig_glCreateShader)(currentShaderType);
				shaderIsCreated = true;
			}

			/// Swap the SHADER///////////////////////////////////////////////////////////////////////////////////
			if (sourceExists == true)
			{
				if (newShaderSource != "")
				{
					// Apply the shader Source
					applyShaderSource(newShaderId, newShaderSource, NULL);

					//Update the Source in the current shader
					m_existingShaders[shaderProgramIndex].m_shaderSourceCode = newShaderSource;
				}
				else
				{
					// Apply the shader Source
					applyShaderSource(newShaderId, m_existingShaders[shaderProgramIndex].m_shaderSourceCode, NULL);
				}

				//Compile vertex shader
				(*orig_glCompileShader)(newShaderId);

				//Test compile
				(*orig_glGetShaderiv)(newShaderId, GL_COMPILE_STATUS, &statusOk);

				if (statusOk == GL_FALSE)
				{
					CheckCompileState(newShaderId);
				}

				// Attach the Current Shader to the Program
				(*orig_glAttachShader)(m_existingShaders[shaderProgramIndex].m_programId, newShaderId);

				//////////////////////////////////////////////////////////////////////////
				/// GET THE SOURCE CODE FOR THE SHADER-PAIR
				//////////////////////////////////////////////////////////////////////////

				// If we don't have the shader attached
				// We need to create one
				if (pairShaderId == 0)
				{
					pairShaderId = (*orig_glCreateShader)(pairShaderType);
					pairIsCreated = true;
				}

				// Apply the shader Source
				std::string sourceCode = GetShaderSourceCode(m_existingShaders[shaderProgramIndex].m_programId, shaderProgramIndex, pairShaderType);
				applyShaderSource(pairShaderId, sourceCode, NULL);

				//Compile vertex shader
				(*orig_glCompileShader)(pairShaderId);

				//Test compile
				(*orig_glGetShaderiv)(pairShaderId, GL_COMPILE_STATUS, &statusOk);

				if (statusOk == GL_FALSE)
				{
					CheckCompileState(pairShaderId);
				}

				// Attach the PAIR Shader to the Program
				(*orig_glAttachShader)(m_existingShaders[shaderProgramIndex].m_programId, pairShaderId);


				//////////////////////////////////////////////////////////////////////////
				/// AT this stage we have both Shaders Compiled. PS+VS
				/// We can LINK

				// Link the program
				(*orig_glLinkProgram)(m_existingShaders[shaderProgramIndex].m_programId);

				//Check for error
				LinkOK = 0;
				(*orig_glGetProgramiv)(m_existingShaders[shaderProgramIndex].m_programId, GL_LINK_STATUS, &LinkOK);
				if (LinkOK == GL_FALSE)
				{
					CheckLinkState(m_existingShaders[shaderProgramIndex].m_programId);
				}

				if (pairIsCreated)
				{
					// Delete our PAIR Shader
					(*orig_glDetachShader)(m_existingShaders[shaderProgramIndex].m_programId, pairShaderId);
					(*orig_glDeleteShader)(pairShaderId);
				}

				// If we created the shader
				// Clean up
				if (shaderIsCreated)
				{
					// We need to clean up here
					(*orig_glDetachShader)(m_existingShaders[shaderProgramIndex].m_programId, newShaderId);
					(*orig_glDeleteShader)(newShaderId);
				}
				
				// Compile done
				// Export shader as correct format
				if (isCurrentShaderVertex)
				{
					MessageBox(NULL, "Vertex Shader Compiled Successfully !!!", "Compile Done:", MB_OK);
					// Re-export the current shader program
					ExportShader("MODIFIED_Vertex",
						m_existingShaders[shaderProgramIndex].m_programId,
						m_existingShaders[shaderProgramIndex].m_CRC32,
						m_existingShaders[shaderProgramIndex].m_shaderSourceCode
						);
				}
				else
				{
					MessageBox(NULL, "Pixel Shader Compiled Successfully !!!", "Compile Done:", MB_OK);
					// Re-export the current shader program
					ExportShader("MODIFIED_Pixel",
						m_existingShaders[shaderProgramIndex].m_programId,
						m_existingShaders[shaderProgramIndex].m_CRC32,
						m_existingShaders[shaderProgramIndex].m_shaderSourceCode
						);
				}
				// Reset the compile state
				isCompileEnable = false;

			}
			else
			{
				// Reset the compile state
				isCompileEnable = false;

				// NO SOURCE CODE
				MessageBox(NULL, "Cannot Compile as the Source file is EMPTY !!!", "Source Code Missing:", MB_OK);
			}			
		}
	}
	int shaderProgIndex = 0;
	// Monitor the Shader Compiler & the Shader Program Id exists
	if (printCurrentShader == true && (shaderProgIndex = ShaderProgramFound(currentShaderIndex, currentShaderType)) != -1)
	{
		// Display the information and create the source file
		std::thread ShaderInfo(
								ShaderCompilerPrintInfo, 
								m_existingShaders[shaderProgIndex].m_programId,
								m_existingShaders[shaderProgIndex].m_shaderId,
								m_existingShaders[shaderProgIndex].m_shaderSourceCode,
								currentShaderType
								);
		ShaderInfo.join();
		printCurrentShader = false;
	}

	m_compilerMutex.unlock();

	return isSet;
}
//--------------------------------------------------------------------------------------------

int ShaderManager::ShaderProgramFound(int progId, GLuint shaderType)
{
	int ret = -1;

	for (size_t i = 0; i < m_existingShaders.size(); i++)
	{
		if ((m_existingShaders[i].m_programId == (GLuint)progId) && (m_existingShaders[i].m_shaderType == shaderType))
		{
			// Found & break
			ret = i;
			break;
		}
	}
	return ret;
}
//--------------------------------------------------------------------------------------------

static void ShaderCompilerPrintInfo(GLuint progId, GLuint shaderId, std::string shaderSource, GLuint shaderType)
{
	char str[500];
	if (shaderType == GL_VERTEX_SHADER)
	{
		sprintf_s(str, "Selected SHADER PROGRAM: %d.\nVERTEX Shader Id: %d. ", progId, shaderId);
		strcat_s(str, 500, "\n\nCurrent Shader Source file: DebugShaders/CurrentShaderSource.glsl.\n--> Press [F9] to re-compile. <--");
	}
	else if (shaderType == GL_FRAGMENT_SHADER)
	{
		sprintf_s(str, "Selected SHADER PROGRAM: %d.\nFRAGMENT Shader Id: %d. ", progId, shaderId);
		strcat_s(str, 500, "\n\nCurrent Shader Source file: DebugShaders/CurrentShaderSource.glsl.\n--> Press [F9] to re-compile. <--");
	}

	// Generate the shader source file
	ExportCompiledShader(shaderSource);

	// Print info
	MessageBox(NULL, str, "Shader Info :", MB_OK);
}
//--------------------------------------------------------------------------------------------

void ExportCompiledShader(std::string _shaderSource)
{
		//Make our dir
		_mkdir("DebugShaders");

		//make the file
		std::string shaderFilename = "DebugShaders/CurrentShaderSource.glsl";

		std::ofstream shaderFile;
		shaderFile.open(shaderFilename);
		shaderFile << _shaderSource;
		shaderFile.close();
}
//--------------------------------------------------------------------------------------------
#endif