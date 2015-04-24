#include "include\ShaderManager.h"
#include "include\configReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <direct.h>
#include "opengl32.h"
extern "C" {
#include "opengl_3dv.h"
}


extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glShaderSource_t orig_glShaderSource;
static bool m_allShadersApplied = false;
extern configReader *g_reader;

#ifdef DEBUG_WRAPPER
extern int debugShaderIndex;
#endif


static std::string GetPath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
// -----------------------------------------------------------------------------------


static std::string removeDoubleSlash(std::string _input)
{
	size_t location =_input.find("\\n");
	while (location != std::string::npos)
	{
		_input.replace(location, 2, "\n");
		location = _input.find("\\n");
	}
	return _input;
}
//--------------------------------------------------------------------------------------------

ShaderManager *ShaderManager::instance = 0;

ShaderManager::ShaderManager()
{
	// Read our shader information
#ifdef DEBUG_WRAPPER
	std::string configFileName = GetPath() + "/3DVisionWrapperDBG.ini";
#else
	std::string configFileName = GetPath() + "/3DVisionWrapper.ini";
#endif
	std::ifstream configFile(configFileName);
	std::string configLine;

	// Default inits
	m_enableStereoFailureInfo = false;

	//file is opened
	if (configFile.is_open())
	{
		// read one line at a time
		while (getline(configFile, configLine))
		{
			//----------------------------------------------
			// Find the Vertex Stereo Injection Point
			if (configLine.find("[Vertex_Stereo_InjectionPoint]") != std::string::npos)
			{
				size_t startPoint;
				size_t endPoint;
				//----------------------------------------------
				//get the next line
				while (configLine.find("Enable") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				if (configLine.find("true") != std::string::npos)
				{
					m_enableStereoVertexInjection = true;
				}
				else if (configLine.find("false") != std::string::npos)
				{
					m_enableStereoVertexInjection = false;
				}
#ifdef DEBUG_WRAPPER
				//----------------------------------------------
				//get the next line
				while (configLine.find("EnableFailureInfo") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				if (configLine.find("true") != std::string::npos)
				{
					m_enableStereoFailureInfo = true;
				}
				else if (configLine.find("false") != std::string::npos)
				{
					m_enableStereoFailureInfo = false;
				}
#endif
				//----------------------------------------------
				//get the next line
				while (configLine.find("InjectionPoint1") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"",0);
				endPoint = configLine.find("\"", startPoint+1);
				m_vertexInjectionPoint1 = configLine.substr(startPoint + 1, endPoint - startPoint-1);
				
				//----------------------------------------------
				//get the next line
				while (configLine.find("InjectionPoint2") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"", 0);
				endPoint = configLine.find("\"", startPoint + 1);
				m_vertexInjectionPoint2 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);

				//----------------------------------------------
				//get the next line
				while (configLine.find("InjectionPoint3") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"", 0);
				endPoint = configLine.find("\"", startPoint + 1);
				m_vertexInjectionPoint3 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);

				//----------------------------------------------
				//get the next line
				while (configLine.find("InjectionPoint4") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"", 0);
				endPoint = configLine.find("\"", startPoint + 1);
				m_vertexInjectionPoint4 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);

				//----------------------------------------------
				//get the next line
				while (configLine.find("UniformInjection1") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"", 0);
				endPoint = configLine.find("\"", startPoint + 1);
				m_vertexUniformPoint1 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);

				//----------------------------------------------
				//get the next line
				while (configLine.find("UniformInjection2") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"", 0);
				endPoint = configLine.find("\"", startPoint + 1);
				m_vertexUniformPoint2 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);

				//----------------------------------------------
				//get the next line
				while (configLine.find("UniformInjection3") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"", 0);
				endPoint = configLine.find("\"", startPoint + 1);
				m_vertexUniformPoint3 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);

				//----------------------------------------------
				//get the next line
				while (configLine.find("UniformInjection4") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"", 0);
				endPoint = configLine.find("\"", startPoint + 1);
				m_vertexUniformPoint4 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);

				//----------------------------------------------
				//get the next line
				while (configLine.find("StereoString") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the injection point
				startPoint = configLine.find("\"", 0);
				endPoint = configLine.find("\"", startPoint + 1);
				m_vertexStereoInjectionString = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				m_vertexStereoInjectionString = removeDoubleSlash(m_vertexStereoInjectionString);
			}
#ifdef DEBUG_WRAPPER
			//----------------------------------------------
			// Shader Compiler options
			if (configLine.find("[Shader_Compiler_Options]") != std::string::npos)
			{
				size_t startPoint;

				//----------------------------------------------
				//get the next line
				while (configLine.find("EnableCompilerOptions") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				if (configLine.find("false") != std::string::npos)
				{
					m_exceptShadersEnabled = false;
				}
				else if (configLine.find("true") != std::string::npos)
				{
					m_exceptShadersEnabled = true;
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("ExceptStartVertexShader") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the start shader
				startPoint = configLine.find("= ", 0);
				std::string value = configLine.substr(startPoint + 1);
				m_shaderVStart = std::stoul(value, nullptr, 10);

				//----------------------------------------------
				//get the next line
				while (configLine.find("ExceptEndVertexShader") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the end shader
				startPoint = configLine.find("= ", 0);
				value = configLine.substr(startPoint + 1);
				m_shaderVEnd = std::stoul(value, nullptr, 10);

				//----------------------------------------------
				//get the next line
				while (configLine.find("ShaderCompilerStartIndex") == std::string::npos)
				{
					getline(configFile, configLine);
				}
				// store the end shader
				startPoint = configLine.find("= ", 0);
				value = configLine.substr(startPoint + 1);
				debugShaderIndex = std::stoul(value, nullptr, 10);

				//----------------------------------------------
			}
#endif
			//----------------------------------------------
			// Find the Vertex Stereo Exceptions
			if (configLine.find("[Vertex_Stereo_Exception]") != std::string::npos)
			{
				// this is needed in order to parse all the except shaders
				bool stop = false;

				//read next line
				getline(configFile, configLine);

				while (configLine.find("[") == std::string::npos)
				{
					size_t startPoint;
					size_t middlePoint;
					size_t middlePoint2;
					size_t endPoint;
					SHADER_EXCEPT_T shaderPair;
					//----------------------------------------------
					//get the next line
					while (configLine.find("ShaderPair") == std::string::npos)
					{
						getline(configFile, configLine);
						// if we cannot find additional except shaders we need to break from here
						if (configLine.find("[") != std::string::npos)
						{
							stop = true;
							break;
						}
					}
					//we finished parsing the exception
					if (stop)
						break;

					// store the injection point
					startPoint = configLine.find("(", 0);
					middlePoint = configLine.find(",", startPoint + 1);
					middlePoint2 = configLine.find(",", middlePoint + 1);
					endPoint = configLine.find(")", middlePoint + 1);

					shaderPair.m_programId = atoi(configLine.substr(startPoint + 1, middlePoint - startPoint - 1).c_str());
					shaderPair.m_vertexShader = atoi(configLine.substr(middlePoint + 1, middlePoint2 - middlePoint - 1).c_str());
					shaderPair.m_pixelShader = atoi(configLine.substr(middlePoint2 + 1, endPoint - middlePoint2 - 1).c_str());
					//----------------------------------------------

					//get the next line
					while (configLine.find("VertexShaderSource") == std::string::npos)
					{
						getline(configFile, configLine);
					}
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					shaderPair.m_vertexSource = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
					//----------------------------------------------

					//get the next line
					while (configLine.find("FragmentShaderSource") == std::string::npos)
					{
						getline(configFile, configLine);
					}
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					shaderPair.m_fragmentSource = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
					shaderPair.m_isApplied = false;
					//PUSH the information
					m_exceptShaders.push_back(shaderPair);
					
					//See if we have more exceptions
					getline(configFile, configLine);
				}
			}
		}
		configFile.close();
	}
}
///-------------------------------------------------------------------------------------------

std::string ShaderManager::getShaderSource(GLuint shaderId)
{
	// Need to reset the string everytime we get here!!!
	m_origSource = "";

	//The size of the buffer required to store the returned source code string can be obtained by calling glGetShader with the value GL_SHADER_SOURCE_LENGTH.
	GLsizei size = 0;
	orig_glGetShader(shaderId, GL_SHADER_SOURCE_LENGTH, &size);

	// Get the current source
	GLchar* source = new GLchar[size];
	GLsizei returnSize;

	if (orig_glGetShaderSource == 0x00)
	{
		orig_glGetShaderSource = (funct_glGetShaderSource_t)orig_wglGetProcAddress("glGetShaderSource");
	}

	(*orig_glGetShaderSource)(shaderId, size, &returnSize, source);

	// We got the source add it in our string
	m_origSource = source;
	
	// Delete the GLchar
	delete[] source;

	return m_origSource;
}
///-------------------------------------------------------------------------------------------

	bool ShaderManager::isShaderType(GLuint _shaderID, GLenum _shaderType)
	{
		// only if we don't have it allready
		if (orig_glGetShader == 0x00)
		{
			orig_glGetShader = (funct_glGetShaderiv_t)orig_wglGetProcAddress("glGetShaderiv");
		}
		//Store the current shader ID
		m_shaderID = _shaderID;

		GLint shaderOK = GL_FALSE;
		// Get the Shader Type
		(*orig_glGetShader)(_shaderID, GL_SHADER_TYPE, &shaderOK);

		//if the shader is our type
		if (shaderOK == (GLint)_shaderType)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
///-------------------------------------------------------------------------------------------
	// Deprecated
	std::string ShaderManager::flipPositionInShader(std::string _inputSource)
	{
		m_newSource = _inputSource;
		
		size_t location = m_newSource.find("gl_Position = ");
		size_t count = location;
		if (location != std::string::npos)
		{

			// Need to count until the end of the current gl_Position
			// And apply the flip afterwards
			for (size_t i = location; i < m_newSource.size(); i++)
			{
				if (m_newSource[i] == *(";"))
				{
					// we reached the end of the current instruction so we break
					break;
				}
				else
				{
					// otherwise keep on counting baby!!!!
					count++;
				}
			}
			// Do the shader gl_Position insertion
			m_newSource.insert(count + 1, "\n gl_Position.y = gl_Position.y * -1.0;\n");
		}
		return m_newSource;
	}
///-------------------------------------------------------------------------------------------

	// Change the shader Source
	void ShaderManager::applyShaderSource(GLuint _shaderID, std::string _shaderSource, const GLint *length)
	{
		// only if we don't have it already
		if (orig_glShaderSource == 0x00)
		{
			orig_glShaderSource = (funct_glShaderSource_t)orig_wglGetProcAddress("glShaderSource");
		}
		const GLchar* source = _shaderSource.c_str();
		(*orig_glShaderSource)(_shaderID, 1, &source, length);
	}
///-------------------------------------------------------------------------------------------

	// This is where the magic happens!!!!
	// Inject Stereoscopy in all SHADERS.. Hell yeaa!!!
	std::string ShaderManager::injectStereoScopy(std::string _inputSource, GLuint programId)
	{
		bool stage1 = false;
		bool stage2 = false;
		m_newSource = _inputSource;
		size_t location = 0;
		size_t count;

		// Insert stereo
#if 1
		// We have 3 injection points so go through all of them
		for (size_t i = 0; i < 4; i++)
		{
			switch (i)
			{
			case 0:
				location = m_newSource.find(GetVertexInjectionPoint1());
				break;

			case 1:
				location = m_newSource.find(GetVertexInjectionPoint2());
				break;

			case 2:
				location = m_newSource.find(GetVertexInjectionPoint3());
				break;

			case 3:
				location = m_newSource.find(GetVertexInjectionPoint4());
				break;
			}
			count = location;
			if (location != std::string::npos)
			{

				// Need to count until the end of the current gl_Position
				// And apply the flip afterwards
				for (size_t i = location; i < m_newSource.size(); i++)
					//for (int i = location; i > 0; i--)
				{
					if (m_newSource[i] == *(";"))
					{
						// we reached the end of the current instruction so we break
						break;
					}
					else
					{
						// otherwise keep on counting baby!!!!
						count++;
					}
				}
				m_newSource.insert(count + 1, GetVertexStereoInjectString());
				stage1 = true;
			}	
			// we inserted so we break;
			if (stage1)
			{
				break;
			}
		}
#endif

		//Adding the uniforms
#if 1
		// We have 3 injection points so go through all of them
		for (size_t i = 0; i < 4; i++)
		{
			switch (i)
			{
			case 0:
				location = m_newSource.find(GetVertexUniformPoint1());
				break;

			case 1:
				location = m_newSource.find(GetVertexUniformPoint2());
				break;

			case 2:
				location = m_newSource.find(GetVertexUniformPoint3());
				break;
			
			case 3:
				location = m_newSource.find(GetVertexUniformPoint4());
				break;
			}
			count = location;
			if (location != std::string::npos)
			{
				// Need to count until the end of the current gl_Position
				// And apply the flip afterwards
				for (size_t i = location; i < m_newSource.size(); i++)
				{
					if (m_newSource[i] == *("\n"))
					{
						// we reached the end of the current instruction so we break
						break;
					}
					else
					{
						// otherwise keep on counting baby!!!!
						count++;
					}
				}
				// Do the shader gl_Position insertion
				m_newSource.insert(count + 1, "uniform float g_depth;\nuniform float g_eye;\nuniform float g_eye_separation;\nuniform float g_convergence;\n");

				stage2 = true;
			}
			// we inserted so we break;
			if (stage2)
			{
				break;
			}
		}
#endif	
		if ((!stage1 || !stage2) && (GetStereoFailureInfoState()))
		{
			char result[5000];
			sprintf_s(result, 5000, "Shader Injection FAILED on Shader Program %d.", programId+1);
			MessageBox(NULL, result, "Stereo Shader Injection", MB_OK);
			add_log(result);
		}
		return m_newSource;
	}
///-------------------------------------------------------------------------------------------

	bool ShaderManager::ApplyExceptionShaders(void)
	{
		bool isSet = false;
		bool vertexExist = false;
		bool pixelExist = false;

		// check if all the shaders have been applied
		if (m_allShadersApplied == false)
		{
			for (size_t i = 0; i < m_exceptShaders.size(); i++)
			{
				if (m_exceptShaders[i].m_isApplied == false)
				{
					// not all shaders have been applied
					break;
				}
				else if (i == m_exceptShaders.size() -1)
				{
					m_allShadersApplied = true;
					add_log("All modified shaders have been applied");
				}
			}
		}

		if (!NV3DVisionIsNotInit() && m_allShadersApplied == false)
		{
			for (size_t j = 0; j < m_existingShaders.size(); j++)
			{
				for (size_t i = 0; i < m_exceptShaders.size(); i++)
				{
					
					if (
						((m_existingShaders[j].m_vertexShader == m_exceptShaders[i].m_vertexShader) || (m_exceptShaders[i].m_vertexShader == 0)) &&
						((m_existingShaders[j].m_pixelShader == m_exceptShaders[i].m_pixelShader) || (m_exceptShaders[i].m_pixelShader == 0)) &&
						((m_existingShaders[j].m_programId == m_exceptShaders[i].m_programId) || ((m_exceptShaders[i].m_vertexShader != 0) && (m_exceptShaders[i].m_pixelShader != 0)) )&&
						(m_exceptShaders[i].m_isApplied == false))
					{
						std::string newVertexSource;
						std::string newFragmentSource;

						// Read our shader source for Vertex
						if (m_exceptShaders[i].m_vertexSource != "")
						{
							// we have a source file
							std::ifstream shaderFile("Shaders/" + m_exceptShaders[i].m_vertexSource);
							std::string shaderLine;

							//file is opened
							if (shaderFile.is_open())
							{
								// read one line at a time
								while (getline(shaderFile, shaderLine))
								{
									newVertexSource += shaderLine + "\n";
								}
								shaderFile.close();
								vertexExist = true;
							}
						}

						// Read our shader source for Fragment
						if (m_exceptShaders[i].m_fragmentSource != "")
						{
							// we have a source file
							std::ifstream shaderFile("Shaders/" + m_exceptShaders[i].m_fragmentSource);
							std::string shaderLine;

							//file is opened
							if (shaderFile.is_open())
							{
								// read one line at a time
								while (getline(shaderFile, shaderLine))
								{
									newFragmentSource += shaderLine + "\n";
								}
								shaderFile.close();
								pixelExist = true;
							}
						}

						GLint vertexOK, fragmentOK, LinkOK;
						GLuint newVertexId = 0;
						GLuint newFragmentId = 0;

						/////////////////////////////////////////////////////////////////////////////////////
						/////			IF we have a LIVE shader just swap the source
						/////
						/////////////////////////////////////////////////////////////////////////////////////
						if ((m_exceptShaders[i].m_vertexShader != 0) && (m_exceptShaders[i].m_pixelShader != 0))
						{
							newVertexId = m_exceptShaders[i].m_vertexShader;
							newFragmentId = m_exceptShaders[i].m_pixelShader;
						}
						else
						{
							// We need to create the shaders
							// Create the new shader 
							orig_glCreateShader = (func_glCreateShader)orig_wglGetProcAddress("glCreateShader");
							if (orig_glCreateShader == 0x0)
							{
								add_log("glCreateShader not found !!!!");
							}

							newVertexId = (*orig_glCreateShader)(GL_VERTEX_SHADER);
							newFragmentId = (*orig_glCreateShader)(GL_FRAGMENT_SHADER);
						}


						/// VERTEX SHADER///////////////////////////////////////////////////////////////////////////////////
						// if we read the vertex
						if (vertexExist == true && pixelExist == true)
						{
							if (newVertexSource != "")
							{
								// Apply the shader Source
								applyShaderSource(newVertexId, newVertexSource, NULL);

								//Update the Source in the current shader
								m_existingShaders[j].m_vertexSource = newVertexSource;
							}
							else
							{
								// Apply the shader Source
								applyShaderSource(newVertexId, m_existingShaders[j].m_vertexSource, NULL);
							}

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
								(*orig_glAttachShader)(m_existingShaders[j].m_programId, newVertexId);
							}
							else
							{
								CheckCompileState(newVertexId);
							}

							/// FRAGMENT SHADER///////////////////////////////////////////////////////////////////////////////////
							if (newFragmentSource != "")
							{
								// Apply the shader Source
								applyShaderSource(newFragmentId, newFragmentSource, NULL);
								
								//Update the Source in the current shader
								m_existingShaders[j].m_fragmentSource = newFragmentSource;
							}
							else
							{
								// Apply the shader Source
								applyShaderSource(newFragmentId, m_existingShaders[j].m_fragmentSource, NULL);
							}

							//Compile vertex shader
							orig_glCompileShader = (func_glCompileShader)orig_wglGetProcAddress("glCompileShader");
							(*orig_glCompileShader)(newFragmentId);

							orig_glGetShaderiv = (func_glGetShaderiv_t)orig_wglGetProcAddress("glGetShaderiv");
							(*orig_glGetShaderiv)(newFragmentId, GL_COMPILE_STATUS, &fragmentOK);
							if (fragmentOK == GL_FALSE)
							{
								CheckCompileState(newFragmentId);
							}

							if ((fragmentOK != GL_FALSE) && (vertexOK != GL_FALSE))
							{
								//Attach
								orig_glAttachShader = (func_glAttachShader_t)orig_wglGetProcAddress("glAttachShader");
								(*orig_glAttachShader)(m_existingShaders[j].m_programId, newFragmentId);

								// Link the program
								(*orig_glLinkProgram)(m_existingShaders[j].m_programId);

								//Check for error
								LinkOK = 0;
								orig_glGetProgramiv = (func_glGetProgramiv_t)orig_wglGetProcAddress("glGetProgramiv");
								(*orig_glGetProgramiv)(m_existingShaders[j].m_programId, GL_LINK_STATUS, &LinkOK);
								if (LinkOK == GL_FALSE)
								{
									CheckLinkState(m_existingShaders[j].m_programId);
								}	
							}

							//////////////////////////////////
							////////////ONLY IF WE CREATED THE SHADERS//////////////////////////////////
							//////////////////////////////////

							if ((m_exceptShaders[i].m_vertexShader == 0) && (m_exceptShaders[i].m_pixelShader == 0))
							{
								//Detach Shaders
								orig_glDetachShader = (func_glDetachShader_t)orig_wglGetProcAddress("glDetachShader");
								orig_glDeleteShader = (func_glDeleteShader_t)orig_wglGetProcAddress("glDeleteShader");

								if (orig_glDetachShader == 0x0)
								{
									add_log("glDetachShader not found !!!!");
								}
								else
								{
									(*orig_glDetachShader)(m_existingShaders[j].m_programId, newVertexId);
									(*orig_glDetachShader)(m_existingShaders[j].m_programId, newFragmentId);
								}

								//Delete Shaders
								if (orig_glDeleteShader == 0x0)
								{
									add_log("glDeleteShader not found !!!!");
								}
								else
								{
									(*orig_glDeleteShader)(newVertexId);
									(*orig_glDeleteShader)(newFragmentId);
								}
							}
							
							// Export the shaders again with the new source
							ExportShader("Vertex", m_existingShaders[j].m_programId, m_existingShaders[j].m_vertexShader, m_existingShaders[j].m_vertexSource);
							ExportShader("Fragment", m_existingShaders[j].m_programId, m_existingShaders[j].m_pixelShader, m_existingShaders[j].m_fragmentSource);

							if (vertexOK != 0 && fragmentOK != 0)
							{
								// we swapped the shaders
								m_exceptShaders[i].m_isApplied = true;
								isSet = true;
							}
							break;
						}
					}
				}
			}
		}
		return isSet;
	}
///-------------------------------------------------------------------------------------------

	///-------------------------------------------------------------------------------------------
	/// PUBLIC FUNCTIONS
	///-------------------------------------------------------------------------------------------

	void CheckCompileState(GLuint newShaderId)
	{
#ifdef DEBUG_WRAPPER
		MessageBox(NULL, "Shader Compile FAILED", "", MB_OK);
		GLint maxLength = 0;
		(*orig_glGetShaderiv)(newShaderId, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		GLchar *infoLog;
		infoLog = new GLchar[maxLength];
		orig_glGetShaderInfoLog = (func_glGetShaderInfoLog_t)orig_wglGetProcAddress("glGetShaderInfoLog");
		(*orig_glGetShaderInfoLog)(newShaderId, maxLength, &maxLength, infoLog);

		// Get the current source
		GLchar source[50000];
		GLsizei size;
		orig_glGetShaderSource = (funct_glGetShaderSource_t)orig_wglGetProcAddress("glGetShaderSource");
		(*orig_glGetShaderSource)(newShaderId, 50000, &size, source);

		strcat_s(source, "\n");
		strcat_s(source, "\n");
		strcat_s(source, "----------------------------------------------------------------------------\n");
		strcat_s(source, "Error Output: \n");
		strcat_s(source, infoLog);
		strcat_s(source, "----------------------------------------------------------------------------\n");

		MessageBox(NULL, source, "Shader Compile Error:", MB_OK);
		exit(0);
#else
		// Avoid warning
		(void)newShaderId;
#endif
}
	///-------------------------------------------------------------------------------------------

	void CheckLinkState(GLuint programId)
	{
#ifdef DEBUG_WRAPPER
		GLint maxLength = 0;

		orig_glGetProgramiv = (func_glGetProgramiv_t)orig_wglGetProcAddress("glGetProgramiv");
		(*orig_glGetProgramiv)(programId, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		GLchar *infoLog;
		infoLog = new GLchar[maxLength + 1];
		orig_glGetProgramInfoLog = (func_glGetProgramInfoLog_t)orig_wglGetProcAddress("glGetProgramInfoLog");
		(*orig_glGetProgramInfoLog)(programId, maxLength, &maxLength, infoLog);
		char result[5000];
		sprintf_s(result, 5000, "Linking Error on Program %d", programId);
		strcat_s(result, "----------------------------------------------------------------------------\n\n");
		strcat_s(result, infoLog);
		MessageBox(NULL, result, "Linking Error:", MB_OK);
		exit(0);
#else
		(void)programId;
#endif
	}
	///-------------------------------------------------------------------------------------------

	void DeleteAndDetachShaders(GLuint progId, GLuint shaderId)
	{
		//Detach Shaders
		orig_glDetachShader = (func_glDetachShader_t)orig_wglGetProcAddress("glDetachShader");
		orig_glDeleteShader = (func_glDeleteShader_t)orig_wglGetProcAddress("glDeleteShader");

		if (orig_glDetachShader == 0x0)
		{
			MessageBox(NULL, "glDetachShader not found !!!!", "Detach Error:", MB_OK);
			exit(0);
		}
		else
		{
			(*orig_glDetachShader)(progId, shaderId);
		}
		//Delete Shaders
		if (orig_glDeleteShader == 0x0)
		{
			MessageBox(NULL, "glDeleteShader not found !!!!", "Delete Error:", MB_OK);
			exit(0);
		}
		else
		{
			(*orig_glDeleteShader)(shaderId);
		}
	}
	///-------------------------------------------------------------------------------------------

	void ExportShader(std::string _shaderType, GLuint _program, GLuint _shaderNumber, std::string _shaderSource)
	{
		if (g_reader->GetEnableShaderDump())
		{
			//Make our dir
			_mkdir("DebugShaders");

			//make the file
			std::string shaderFilename = "DebugShaders/" + std::to_string(_program) + "_" + _shaderType + "_" + std::to_string(_shaderNumber) + ".glsl";

			std::ofstream shaderFile;
			shaderFile.open(shaderFilename);
			shaderFile << _shaderSource;
			shaderFile.close();
		}
	}
	///-------------------------------------------------------------------------------------------