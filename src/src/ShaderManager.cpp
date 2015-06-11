#include "include\ShaderManager.h"
#include "include\configReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <direct.h>
#include "opengl32.h"
#include "crc32.h"
#include <mutex>

extern "C" {
#include "opengl_3dv.h"
}


extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glShaderSource_t orig_glShaderSource;
static bool m_allShadersApplied = false;
extern configReader *g_reader;
static std::recursive_mutex m_shaderMutex;

#ifdef DEBUG_WRAPPER
extern int debugVertexIndex;
extern int debugPixelIndex;
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
	LoadCustomShaders();
	ReadConfigFile();
}
///-------------------------------------------------------------------------------------------

void ShaderManager::ReadConfigFile(void)
{
	// Remove the current values so we use the new ones
	m_customShaderValues.resize(0);

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
					if (configLine.find("true") != std::string::npos)
					{
						m_enableStereoVertexInjection = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_enableStereoVertexInjection = false;
					}
				}
#ifdef DEBUG_WRAPPER
				//----------------------------------------------
				//get the next line
				while (configLine.find("EnableFailureInfo") == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_enableStereoFailureInfo = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_enableStereoFailureInfo = false;
					}
				}
#endif
				//----------------------------------------------
				//get the next line
				while (configLine.find("InjectionPoint1") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexInjectionPoint1 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("InjectionPoint2") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexInjectionPoint2 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("InjectionPoint3") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexInjectionPoint3 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				}
				

				//----------------------------------------------
				//get the next line
				while (configLine.find("InjectionPoint4") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexInjectionPoint4 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("UniformInjection1") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexUniformPoint1 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("UniformInjection2") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexUniformPoint2 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("UniformInjection3") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexUniformPoint3 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("UniformInjection4") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexUniformPoint4 = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("StereoString") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexStereoInjectionString = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
					m_vertexStereoInjectionString = removeDoubleSlash(m_vertexStereoInjectionString);
				}
#ifdef DEBUG_WRAPPER
				while (configLine.find("VertexDisableString") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_vertexDisableString = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
					m_vertexDisableString = removeDoubleSlash(m_vertexDisableString);
				}

				while (configLine.find("PixelDisableString") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the injection point
					startPoint = configLine.find("\"", 0);
					endPoint = configLine.find("\"", startPoint + 1);
					m_pixelDisableString = configLine.substr(startPoint + 1, endPoint - startPoint - 1);
					m_pixelDisableString = removeDoubleSlash(m_pixelDisableString);
				}
#endif
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
					if (configLine.find("false") != std::string::npos)
					{
						m_exceptShadersEnabled = false;
					}
					else if (configLine.find("true") != std::string::npos)
					{
						m_exceptShadersEnabled = true;
					}
				}

				while (configLine.find("DisableExceptShaderInterval") == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("false") != std::string::npos)
					{
						m_disableExceptShaders = false;
					}
					else if (configLine.find("true") != std::string::npos)
					{
						m_disableExceptShaders = true;
					}
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("ExceptStartShader") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the start shader
					startPoint = configLine.find("= ", 0);
					std::string value = configLine.substr(startPoint + 1);
					m_shaderStart = std::stoul(value, nullptr, 10);
				}

				//----------------------------------------------
				//get the next line
				while (configLine.find("ExceptEndShader") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the end shader
					startPoint = configLine.find("= ", 0);
					std::string value = configLine.substr(startPoint + 1);
					m_shaderEnd = std::stoul(value, nullptr, 10);
				}
				
				while (configLine.find("DisableCurrentShaders") == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("false") != std::string::npos)
					{
						m_disableCurrentShader = false;
					}
					else if (configLine.find("true") != std::string::npos)
					{
						m_disableCurrentShader = true;
					}
				}
				

				//----------------------------------------------
				//get the next line
				while (configLine.find("ShaderCompilerStartIndex") == std::string::npos)
				{
					getline(configFile, configLine);
					// store the end shader
					startPoint = configLine.find("= ", 0);
					std::string value = configLine.substr(startPoint + 1);
					debugVertexIndex = std::stoul(value, nullptr, 10);
					debugPixelIndex = std::stoul(value, nullptr, 10);
				}

				//----------------------------------------------
			}
#endif
			if (configLine.find("[Stereo_Custom_Params]") != std::string::npos)
			{
				// this is needed in order to parse all the except shaders
				while (configLine.find("[End]") == std::string::npos)
				{
					getline(configFile, configLine);
					//----------------------------------------------
					//get the next line
					if (configLine.find("ShaderProgram") != std::string::npos)
					{
						CUSTOM_SHADER_VALUES_T shaderValue;

						// store the injection point
						size_t startPoint = configLine.find("(", 0);
						size_t middlePoint1 = configLine.find(",", startPoint + 1);
						size_t middlePoint2 = configLine.find(",", middlePoint1 + 1);
						size_t middlePoint3 = configLine.find(",", middlePoint2 + 1);
						size_t middlePoint4 = configLine.find(",", middlePoint3 + 1);
						size_t endPoint = configLine.find(")", middlePoint4 + 1);

						shaderValue.m_programId = atoi(configLine.substr(startPoint + 1, middlePoint1 - startPoint - 1).c_str());
						shaderValue.x = (float)atof(configLine.substr(middlePoint1 + 1, middlePoint2 - middlePoint1 - 1).c_str());
						shaderValue.y = (float)atof(configLine.substr(middlePoint2 + 1, middlePoint3 - middlePoint2 - 1).c_str());
						shaderValue.z = (float)atof(configLine.substr(middlePoint3 + 1, middlePoint4 - middlePoint3 - 1).c_str());
						shaderValue.w = (float)atof(configLine.substr(middlePoint4 + 1, endPoint - middlePoint4 - 1).c_str());


						m_customShaderValues.push_back(shaderValue);
						//----------------------------------------------
					}
				}
			}
		}
		configFile.close();
	}

#ifdef DEBUG_WRAPPER
	// Add the shader compiler increments
	if (m_existingShaders.size() != 0)
	{
		for (size_t i = 0; i < m_existingShaders.size(); i++)
		{
			if (debugVertexIndex == (int)m_existingShaders[i].m_programId)
			{
				debugVertexIndex = m_existingShaders[i].m_programId;
				debugPixelIndex = m_existingShaders[i].m_programId;
				break;
			}
			else if (i == (m_existingShaders.size()-1))
			{
				debugVertexIndex = m_existingShaders[0].m_programId;
				debugPixelIndex = m_existingShaders[0].m_programId;
			}
		}
	}

#endif 
}
///-------------------------------------------------------------------------------------------

std::string ShaderManager::getShaderSource(GLuint shaderId)
{
	// Need to reset the string everytime we get here!!!
	std::string returnSournce = "";

	//The size of the buffer required to store the returned source code string can be obtained by calling glGetShader with the value GL_SHADER_SOURCE_LENGTH.
	GLsizei size = 0;
	(*orig_glGetShader)(shaderId, GL_SHADER_SOURCE_LENGTH, &size);

	// Get the current source
	GLchar* source = new GLchar[size];
	GLsizei returnSize;

	if (orig_glGetShaderSource == 0x00)
	{
		orig_glGetShaderSource = (funct_glGetShaderSource_t)orig_wglGetProcAddress("glGetShaderSource");
	}

	(*orig_glGetShaderSource)(shaderId, size, &returnSize, source);

	// We got the source add it in our string
	returnSournce = source;
	
	// Delete the GLchar
	delete[] source;

	return returnSournce;
}
///-------------------------------------------------------------------------------------------

	bool ShaderManager::isShaderType(GLuint _shaderID, GLenum _shaderType)
	{
		// only if we don't have it allready
		if (orig_glGetShader == 0x00)
		{
			orig_glGetShader = (funct_glGetShaderiv_t)orig_wglGetProcAddress("glGetShaderiv");
		}

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
		std::string newSource = _inputSource;
		size_t location = 0;
		size_t count;

		// We only need to inject if we didn't do it previously
		// Some programs like to reuse the shaders
		if (_inputSource.find("uniform float g_eye;\nuniform float g_eye_separation;\nuniform float g_convergence;\n") == std::string::npos)
		{
			// Insert stereo
			// We have 4 injection points so go through all of them
			for (size_t i = 0; i < 4; i++)
			{
				switch (i)
				{
				case 0:
					location = 0;
					location = newSource.find(GetVertexInjectionPoint1());
					break;

				case 1:
					location = 0;
					location = newSource.find(GetVertexInjectionPoint2());
					break;

				case 2:
					location = 0;
					location = newSource.find(GetVertexInjectionPoint3());
					break;

				case 3:
					location = 0;
					location = newSource.find(GetVertexInjectionPoint4());
					break;
				}

				while (location != std::string::npos)
				{
					count = location;
					if ((count = newSource.find(";", count + 1)) != std::string::npos)
					{
						// We insert the string at this position
#ifdef DEBUG_WRAPPER
						newSource.insert(count + 1, GetVertexStereoInjectString() + GetVertexDisableString());
#else
						newSource.insert(count + 1, GetVertexStereoInjectString());
#endif
					}
#ifdef DEBUG_WRAPPER
					location = newSource.find(GetVertexInjectionPoint1(), count + GetVertexStereoInjectString().length() + GetVertexDisableString().length());
#else
					location = newSource.find(GetVertexInjectionPoint1(), count + GetVertexStereoInjectString().length());
#endif
				}

			}
			//Adding the uniforms
			// We have 3 injection points so go through all of them
			for (size_t i = 0; i < 4; i++)
			{
				switch (i)
				{
				case 0:
					location = newSource.find(GetVertexUniformPoint1());
					break;

				case 1:
					location = newSource.find(GetVertexUniformPoint2());
					break;

				case 2:
					location = newSource.find(GetVertexUniformPoint3());
					break;

				case 3:
					location = newSource.find(GetVertexUniformPoint4());
					break;
				}
				count = location;
				if (location != std::string::npos)
				{
					// Need to count until the end of the current gl_Position
					// And apply the flip afterwards
					for (size_t i = location; i < newSource.size(); i++)
					{
						if (newSource[i] == *("\n"))
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
					newSource.insert(count + 1, "uniform float g_vertexEnabled;\nuniform float g_eye;\nuniform float g_eye_separation;\nuniform float g_convergence;\nuniform vec4 g_custom_params;\n");

					stage2 = true;
				}
				// we inserted so we break;
				if (stage2)
				{
					break;
				}
			}
			if ((!stage1 || !stage2) && (GetStereoFailureInfoState()))
			{
				char result[5000];
				sprintf_s(result, 5000, "Shader Injection FAILED on Shader Program %d.", programId);
				MessageBox(NULL, result, "Stereo Shader Injection", MB_OK);
				add_log(result);
			}
			return newSource;
		}
		else
		{
		
			// We return the same source
			return newSource;
		}
	}
///-------------------------------------------------------------------------------------------
	
	// Inject our code in the pixel shader
	std::string ShaderManager::injectFragmentModification(std::string _inputSource, GLuint programId)
	{
		bool stage1 = false;
		bool stage2 = false;
		std::string newSource = _inputSource;
		size_t location = 0;
		size_t count;
		// We only need to inject if we didn't do it previously
		// Some programs like to reuse the shaders
		if (_inputSource.find("uniform float g_pixelEnabled;") == std::string::npos)
		{
			// Insert stereo
			// We have 3 injection points so go through all of them
			for (size_t i = 0; i < 4; i++)
			{
				switch (i)
				{
				case 0:
					location = newSource.find(GetVertexInjectionPoint1());
					break;

				case 1:
					location = newSource.find(GetVertexInjectionPoint2());
					break;

				case 2:
					location = newSource.find(GetVertexInjectionPoint3());
					break;

				case 3:
					location = newSource.find(GetVertexInjectionPoint4());
					break;
				}
				count = location;
				if (location != std::string::npos)
				{

					// Need to count until the end of the current gl_Position
					// And apply the flip afterwards
					for (size_t i = location; i < newSource.size(); i++)
						//for (int i = location; i > 0; i--)
					{
						if (newSource[i] == *(";"))
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
					// WE also add the ability to disable the shader
#ifdef DEBUG_WRAPPER
					newSource.insert(count + 1, GetPixelDisableString());
#endif
					stage1 = true;
				}
				// we inserted so we break;
				if (stage1)
				{
					break;
				}
			}

			//Adding the uniforms
			// We have 3 injection points so go through all of them
			for (size_t i = 0; i < 4; i++)
			{
				switch (i)
				{
				case 0:
					location = newSource.find(GetVertexUniformPoint1());
					break;

				case 1:
					location = newSource.find(GetVertexUniformPoint2());
					break;

				case 2:
					location = newSource.find(GetVertexUniformPoint3());
					break;

				case 3:
					location = newSource.find(GetVertexUniformPoint4());
					break;
				}
				count = location;
				if (location != std::string::npos)
				{
					// Need to count until the end of the current gl_Position
					// And apply the flip afterwards
					for (size_t i = location; i < newSource.size(); i++)
					{
						if (newSource[i] == *("\n"))
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
					newSource.insert(count + 1, "uniform float g_pixelEnabled;\n");

					stage2 = true;
				}
				// we inserted so we break;
				if (stage2)
				{
					break;
				}
			}
			if ((!stage1 || !stage2) && (GetStereoFailureInfoState()))
			{
				char result[5000];
				sprintf_s(result, 5000, "Pixel Shader Injection FAILED on Shader Program %d.", programId);
				MessageBox(NULL, result, "Pixel Shader Injection", MB_OK);
				add_log(result);
			}
			return newSource;
		}
		else
		{
			// We return the same source
			return newSource;
		}
	}
///-------------------------------------------------------------------------------------------

	bool ShaderManager::ApplyExceptionShaders(std::string &newShaderSource, GLenum shaderType, DWORD CRC32)
{
	m_shaderMutex.lock();

	bool isSet = false;

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
			else if (i == m_exceptShaders.size() - 1)
			{
				m_allShadersApplied = true;
				add_log("All modified shaders have been applied");
			}
		}
	}

	if (m_allShadersApplied == false)
	{
		for (size_t i = 0; i < m_exceptShaders.size(); i++)
		{
			if (
				(CRC32 == m_exceptShaders[i].m_CRC32) &&
				(shaderType == m_exceptShaders[i].m_shaderType) &&
				(m_exceptShaders[i].m_isApplied == false)
				)

			{
				std::string shaderSource;

				// Read our shader source for Vertex
				if (m_exceptShaders[i].m_shaderSourceCode != "")
				{
					// we have a source file
					std::ifstream shaderFile("Shaders/" + m_exceptShaders[i].m_shaderSourceCode);
					std::string shaderLine;

					//file is opened
					if (shaderFile.is_open())
					{
						// read one line at a time
						while (getline(shaderFile, shaderLine))
						{
							shaderSource += shaderLine + "\n";
						}
						shaderFile.close();
						isSet = true;
						newShaderSource = shaderSource;
					}
				}
			}
		}				
	}
	m_shaderMutex.unlock();
	return isSet;
}
///-------------------------------------------------------------------------------------------

	void ShaderManager::LoadCustomShaders(void)
	{
		m_exceptShaders.resize(0);
		HANDLE hFind;
		WIN32_FIND_DATA data;
		std::string currentFolder = GetPath() + "\\Shaders\\*.glsl";

		// List all the shaders
		hFind = FindFirstFile(currentFolder.c_str(), &data);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				//Decode the filename
				std::stringstream dwordStream;
				DWORD crc32 = 0;
				std::string filename = data.cFileName;
				CUSTOM_SHADER_T newShader;
				newShader.m_shaderSourceCode = filename;

				// Get the Shader Type
				size_t position = filename.find("_");
				std::string temp = filename.substr(position + 1, 1);
				
				if (temp[0] == 'P')
				{
					// Pixel Shader
					newShader.m_shaderType = GL_FRAGMENT_SHADER;
					
				}
				else if (temp[0] == 'V')
				{
					// Vertex Shader
					newShader.m_shaderType = GL_VERTEX_SHADER;
				}

				// Get the crc
				position = filename.find("_", position + 1);
				dwordStream << filename.substr(position + 1);
				dwordStream >> std::hex >> crc32;
				newShader.m_CRC32 = crc32;

				// Push the current information in the stack
				newShader.m_isApplied = false;
				m_exceptShaders.push_back(newShader);

			} while (FindNextFile(hFind, &data));

			FindClose(hFind);
		}
	}
	///-------------------------------------------------------------------------------------------

	std::string ShaderManager::GetShaderSourceCode(GLuint programId, GLuint startIndex, GLenum shaderType)
	{
		std::string shaderSourceCode = "";
		size_t i = startIndex;

		for (; i < (m_existingShaders.size() - 1); i++)
		{
			// Do this only if we don't have an index of zero
			if (i >= 1)
			{
				// Search the previous one
				if ((m_existingShaders[i - 1].m_programId == programId) && (m_existingShaders[i - 1].m_shaderType == shaderType))
				{
					// we found it
					shaderSourceCode = m_existingShaders[i - 1].m_shaderSourceCode;
					break;
				}
			}
			// Search the next one
			if ((m_existingShaders[i + 1].m_programId == programId) && (m_existingShaders[i + 1].m_shaderType == shaderType))
			{
				// we found it
				shaderSourceCode = m_existingShaders[i + 1].m_shaderSourceCode;
				break;
			}
		}
		return shaderSourceCode;
	}
	///-------------------------------------------------------------------------------------------

	void ShaderManager::ApplyCustomShaderParams(GLuint progId)
	{
		for (size_t i = 0; i < m_customShaderValues.size(); i++)
		{
			if (progId == m_customShaderValues[i].m_programId)
			{
				if (!orig_glUniform4f)
					orig_glUniform4f = (func_glUniform4f_t)(orig_wglGetProcAddress)("glUniform4f");

				//Set the Separation and convergence
				const GLchar *uniform_custom_params = (GLchar*)"g_custom_params";
				GLint location_custom_params = (*orig_glGetUniformLocation)(progId, uniform_custom_params);
				(*orig_glUniform4f)(location_custom_params, m_customShaderValues[i].x, m_customShaderValues[i].y, m_customShaderValues[i].z, m_customShaderValues[i].w);
				//done and break;
				break;
			}
		}
	}

	///-------------------------------------------------------------------------------------------
	/// PUBLIC FUNCTIONS
	///-------------------------------------------------------------------------------------------

	void CheckCompileState(GLuint newShaderId)
	{
		char info[255];
		sprintf_s(info, 255, "Shader Compile FAILED: ShaderID:%d", newShaderId);

		MessageBox(NULL, info, "", MB_OK);
		GLint maxLength = 0;
		(*orig_glGetShaderiv)(newShaderId, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		GLchar *infoLog;
		infoLog = new GLchar[maxLength];
		orig_glGetShaderInfoLog = (func_glGetShaderInfoLog_t)orig_wglGetProcAddress("glGetShaderInfoLog");
		(*orig_glGetShaderInfoLog)(newShaderId, maxLength, &maxLength, infoLog);

		// Get the current source
		// Get the Source Length
		GLsizei sizeSource = 0;
		(*orig_glGetShader)(newShaderId, GL_SHADER_SOURCE_LENGTH, &sizeSource);
		GLchar* source = new GLchar[sizeSource+5000];
		GLsizei size;
		orig_glGetShaderSource = (funct_glGetShaderSource_t)orig_wglGetProcAddress("glGetShaderSource");
		(*orig_glGetShaderSource)(newShaderId, sizeSource, &size, source);

		strcat_s(source, (sizeSource+5000), "\n");
		strcat_s(source, (sizeSource + 5000), "\n");
		strcat_s(source, (sizeSource + 5000), "----------------------------------------------------------------------------\n");
		strcat_s(source, (sizeSource + 5000), "Error Output: \n");
		strcat_s(source, (sizeSource + 5000), infoLog);
		strcat_s(source, (sizeSource + 5000), "----------------------------------------------------------------------------\n");

		MessageBox(NULL, source, "Shader Compile Error:", MB_OK);
		delete[] source;
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
		delete[] infoLog;
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

	void ExportShader(std::string _shaderType, GLuint _program, DWORD crc32, std::string _shaderSource)
	{
		if (g_reader->GetEnableShaderDump())
		{
			//Make our dir
			_mkdir("DebugShaders");

			std::ostringstream ss;
			ss << std::hex << crc32;

			//make the file
			std::string shaderFilename = "DebugShaders/" + std::to_string(_program) + "_" + _shaderType + "_" + ss.str() + ".glsl";

			std::ofstream shaderFile;
			shaderFile.open(shaderFilename);
			shaderFile << _shaderSource;
			shaderFile.close();
		}
	}
	///-------------------------------------------------------------------------------------------
