#include "include\ShaderManager.h"
#include <direct.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <thread>
#include "opengl32.h"

extern "C" {
#include "opengl_3dv.h"
}

extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glShaderSource_t orig_glShaderSource;

#ifdef DEBUG_WRAPPER
extern int debugShaderIndex;
extern bool printCurrentShader;
extern bool isCurrentShaderVertex;
extern bool isCompileEnable;
#endif

// Local functions
static void ShaderCompilerPrintInfo(GLuint progId, GLuint vertexId, GLuint fragmentId, std::string shaderSource, bool isVertex);

#ifdef DEBUG_WRAPPER
bool ShaderManager::ApplyDebugExceptionShaders(void)
{
	bool isSet = false;

	if (!NV3DVisionIsNotInit() && isCompileEnable)
	{
		int shaderProgramIndex = 0;
		if ((shaderProgramIndex = ShaderProgramFound(debugShaderIndex)) != -1)
		{
				// Read source file
#if 1
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
#endif

				// Vertex Shader
				if (isCurrentShaderVertex)
				{

// Compile The Vertex and attach the source
#if 1
					GLint vertexOK, LinkOK;
					// Create the new shader 
					orig_glCreateShader = (func_glCreateShader)orig_wglGetProcAddress("glCreateShader");
					if (orig_glCreateShader == 0x0)
					{
						add_log("glCreateShader not found !!!!");
					}

					GLuint newVertexId = (*orig_glCreateShader)(GL_VERTEX_SHADER);

					// if we read the vertex
					if (sourceExists == true)
					{
						if (newShaderSource != "")
						{
							// Apply the shader Source
							applyShaderSource(newVertexId, newShaderSource, NULL);

							// Save the current Source of the shader
							SaveNewSource(m_existingShaders[shaderProgramIndex].m_programId, newShaderSource, true);
						}
						else
						{
							// No source found
							sourceExists = false;
						}
					}

					if (sourceExists == true)
					{
						//Compile vertex shader
						orig_glCompileShader = (func_glCompileShader)orig_wglGetProcAddress("glCompileShader");
						if (orig_glCompileShader == 0x0)
						{
							add_log("glCompileShader not found !!!!");
						}
						(*orig_glCompileShader)(newVertexId);

						//Test compile
						orig_glGetShaderiv = (func_glGetShaderiv_t)orig_wglGetProcAddress("glGetShaderiv");
						if (orig_glGetShaderiv == 0x0)
						{
							add_log("glGetShaderiv not found !!!!");
						}
						(*orig_glGetShaderiv)(newVertexId, GL_COMPILE_STATUS, &vertexOK);
						if (vertexOK != GL_FALSE)
						{
							orig_glAttachShader = (func_glAttachShader_t)orig_wglGetProcAddress("glAttachShader");
							if (orig_glAttachShader == 0x0)
							{
								add_log("glAttachShader not found !!!!");
							}
							//Attach
							(*orig_glAttachShader)(m_existingShaders[shaderProgramIndex].m_programId, newVertexId);
						}
						else
						{
							// Compile failed
							CheckCompileState(newVertexId);
							// Detach and Delete the shaders
							DeleteAndDetachShaders(m_existingShaders[shaderProgramIndex].m_programId, newVertexId);
						}

						// LINK
						if (vertexOK != GL_FALSE)
						{
							// ONLY IF THE EXISTING VERTEX shader is different than the EXISTING one. (If they are the same the programs swapped anyway)
							if (newVertexId != m_existingShaders[shaderProgramIndex].m_vertexShader)
							{
								//!!!!!!!!!!!!!!!!!!!!!!!!!! BE SURE TO REMOVE THE EXISTING SOURCES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!							
								//Detach the current Vertex Shaders
								orig_glDetachShader = (func_glDetachShader_t)orig_wglGetProcAddress("glDetachShader");
								(*orig_glDetachShader)(m_existingShaders[shaderProgramIndex].m_programId, m_existingShaders[shaderProgramIndex].m_vertexShader);
								// Delete the current shaders
								orig_glDeleteShader = (func_glDeleteShader_t)orig_wglGetProcAddress("glDeleteShader");
								(*orig_glDeleteShader)(m_existingShaders[shaderProgramIndex].m_vertexShader);

								//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							}

							// Link the program
							(*orig_glLinkProgram)(m_existingShaders[shaderProgramIndex].m_programId);

							//Check for error
							LinkOK = 0;
							orig_glGetProgramiv = (func_glGetProgramiv_t)orig_wglGetProcAddress("glGetProgramiv");
							(*orig_glGetProgramiv)(m_existingShaders[shaderProgramIndex].m_programId, GL_LINK_STATUS, &LinkOK);
							if (LinkOK == GL_FALSE)
							{
								CheckLinkState(m_existingShaders[shaderProgramIndex].m_programId);
								// Detach and Delete the shaders
								DeleteAndDetachShaders(m_existingShaders[shaderProgramIndex].m_programId, newVertexId);
							}
							else
							{
								// Compile done
								MessageBox(NULL, "Shader Compiled Successfully !!!", "Compile Done:", MB_OK);

								// Re-export the current shader program
								ExportShader("MODIFIED_Vertex", 
											m_existingShaders[shaderProgramIndex].m_programId, 
											m_existingShaders[shaderProgramIndex].m_vertexShader, 
											m_existingShaders[shaderProgramIndex].m_vertexSource
											);
							}
						}
						// Detach and Delete the shaders
						DeleteAndDetachShaders(m_existingShaders[shaderProgramIndex].m_programId, newVertexId);

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
#endif					
				}
				// Fragment Shader
				else
				{
// Compile The Fragment and attach the source
#if 1
					GLint fragmentOK, LinkOK;
					// Create the new shader 
					orig_glCreateShader = (func_glCreateShader)orig_wglGetProcAddress("glCreateShader");
					if (orig_glCreateShader == 0x0)
					{
						add_log("glCreateShader not found !!!!");
					}

					GLuint newFragmentId = (*orig_glCreateShader)(GL_FRAGMENT_SHADER);

					// if we read the vertex
					if (sourceExists == true)
					{
						if (newShaderSource != "")
						{
							// Apply the shader Source
							applyShaderSource(newFragmentId, newShaderSource, NULL);

							// Save the current Source of the shader
							SaveNewSource(m_existingShaders[shaderProgramIndex].m_programId, newShaderSource, false);
						}
						else
						{
							// No source found
							sourceExists = false;
						}
					}

					if (sourceExists == true)
					{
						//Compile vertex shader
						orig_glCompileShader = (func_glCompileShader)orig_wglGetProcAddress("glCompileShader");
						if (orig_glCompileShader == 0x0)
						{
							add_log("glCompileShader not found !!!!");
						}
						(*orig_glCompileShader)(newFragmentId);

						//Test compile
						orig_glGetShaderiv = (func_glGetShaderiv_t)orig_wglGetProcAddress("glGetShaderiv");
						if (orig_glGetShaderiv == 0x0)
						{
							add_log("glGetShaderiv not found !!!!");
						}
						(*orig_glGetShaderiv)(newFragmentId, GL_COMPILE_STATUS, &fragmentOK);
						if (fragmentOK != GL_FALSE)
						{
							orig_glAttachShader = (func_glAttachShader_t)orig_wglGetProcAddress("glAttachShader");
							if (orig_glAttachShader == 0x0)
							{
								add_log("glAttachShader not found !!!!");
							}
							//Attach
							(*orig_glAttachShader)(m_existingShaders[shaderProgramIndex].m_programId, newFragmentId);
						}
						else
						{
							CheckCompileState(newFragmentId);
							// Detach and Delete the shaders
							DeleteAndDetachShaders(m_existingShaders[shaderProgramIndex].m_programId, newFragmentId);
						}

						// LINK
						if (fragmentOK != GL_FALSE)
						{
							// ONLY IF THE EXISTING FRAGMENT shader is different than the EXISTING one. (If they are the same the programs swapped anyway)
							if (newFragmentId != m_existingShaders[shaderProgramIndex].m_pixelShader)
							{
								//!!!!!!!!!!!!!!!!!!!!!!!!!! BE SURE TO REMOVE THE EXISTING SOURCES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!							
								//Detach the current Vertex Shaders
								orig_glDetachShader = (func_glDetachShader_t)orig_wglGetProcAddress("glDetachShader");
								(*orig_glDetachShader)(m_existingShaders[shaderProgramIndex].m_programId, m_existingShaders[shaderProgramIndex].m_pixelShader);
								// Delete the current shaders
								orig_glDeleteShader = (func_glDeleteShader_t)orig_wglGetProcAddress("glDeleteShader");
								(*orig_glDeleteShader)(m_existingShaders[shaderProgramIndex].m_pixelShader);

								//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							}

							// Link the program
							(*orig_glLinkProgram)(m_existingShaders[shaderProgramIndex].m_programId);

							//Check for error
							LinkOK = 0;
							orig_glGetProgramiv = (func_glGetProgramiv_t)orig_wglGetProcAddress("glGetProgramiv");
							(*orig_glGetProgramiv)(m_existingShaders[shaderProgramIndex].m_programId, GL_LINK_STATUS, &LinkOK);
							if (LinkOK == GL_FALSE)
							{
								CheckLinkState(m_existingShaders[shaderProgramIndex].m_programId);
								// Detach and Delete the shaders
								DeleteAndDetachShaders(m_existingShaders[shaderProgramIndex].m_programId, newFragmentId);
							}
							else
							{
								// Compile done
								MessageBox(NULL, "Fragment Shader Compiled Successfully !!!", "Compile Done:", MB_OK);

								// Re-export the current shader program
								ExportShader("MODIFIED_Fragment",
									m_existingShaders[shaderProgramIndex].m_programId,
									m_existingShaders[shaderProgramIndex].m_pixelShader,
									m_existingShaders[shaderProgramIndex].m_fragmentSource
									);
							}
						}
						// Detach and Delete the shaders
						DeleteAndDetachShaders(m_existingShaders[shaderProgramIndex].m_programId, newFragmentId);
						
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
#endif					
				}
		}
	}
	int shaderProgIndex = 0;
	// Monitor the Shader Compiler & the Shader Program Id exists
	if (printCurrentShader == true && (shaderProgIndex = ShaderProgramFound(debugShaderIndex)) != -1)
	{
		std::string shaderSource;
		// If the current shader is vertex and the program is found
		if (isCurrentShaderVertex)
		{
			

			shaderSource = m_existingShaders[shaderProgIndex].m_vertexSource;
		}
		else 
		{
			shaderSource = m_existingShaders[shaderProgIndex].m_fragmentSource;
		}

		// Display the information and create the source file
		std::thread ShaderInfo(
								ShaderCompilerPrintInfo, 
								m_existingShaders[shaderProgIndex].m_programId,
								m_existingShaders[shaderProgIndex].m_vertexShader,
								m_existingShaders[shaderProgIndex].m_pixelShader,
								shaderSource,
								isCurrentShaderVertex
								);
		ShaderInfo.detach();
		printCurrentShader = false;
	}
	return isSet;
}
//--------------------------------------------------------------------------------------------

static void ShaderCompilerPrintInfo(GLuint progId, GLuint vertexId,  GLuint fragmentId, std::string shaderSource, bool isVertex)
{
	char str[500];
	if (isVertex)
	{
		sprintf_s(str, "Selected SHADER PROGRAM: %d.\nVERTEX Shader Id: %d. ", progId, vertexId);
		strcat_s(str, 500, "\n\nCurrent Shader Source file: DebugShaders/CurrentShaderSource.glsl.\n\n--> Press [F8] to swap to the Fragment Shader. <--\n\n--> Press [F9] to re-compile. <--");
	}
	else
	{
		sprintf_s(str, "Selected SHADER PROGRAM: %d.\nFRAGMENT Shader Id: %d. ", progId, fragmentId);
		strcat_s(str, 500, "\n\nCurrent Shader Source file: DebugShaders/CurrentShaderSource.glsl.\n\n--> Press [F8] to swap to the Vertex Shader. <--\n\n--> Press [F9] to re-compile. <--");
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

int ShaderManager::ShaderProgramFound(int progId)
{
	int ret = -1;

	for (size_t i = 0; i < m_existingShaders.size(); i++)
	{
		if (m_existingShaders[i].m_programId == (GLuint)progId)
		{
			// Found & break
			ret = i;
			break;
		}
	}
	return ret;
}
//--------------------------------------------------------------------------------------------

void ShaderManager::SaveNewSource(int progId, std::string shaderSource, bool isVertex)
{
	for (size_t i = 0; i < m_existingShaders.size(); i++)
	{
		if (m_existingShaders[i].m_programId == (GLuint)progId)
		{
			// Found & save source
			if (isVertex)
			{
				m_existingShaders[i].m_vertexSource = shaderSource;
			}
			else
			{
				m_existingShaders[i].m_fragmentSource = shaderSource;
			}
			// Done and break;
			break;
		}
	}
}
#endif