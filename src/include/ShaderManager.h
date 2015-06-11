/*
* OpenGL - 3D Vision Wrapper
* Copyright (c) Helifax 2015
*/

#ifndef __SHADER__MANAGER_H__
#define __SHADER__MANAGER_H__

#include <string>
#include <vector>
#include <stdlib.h> 
#include <gl_custom.h>

// Stucture that will hold the existing shaders + program that they are attached to
typedef struct
{
	GLuint m_programId;
	GLuint m_shaderId;
	DWORD m_CRC32;
	GLuint m_shaderType;
	std::string m_shaderSourceCode;
} EXISTING_SHADER_T;

// Stucture that will hold the custom shaders
typedef struct
{
	bool m_isApplied;
	DWORD m_CRC32;
	GLuint m_shaderType;
	std::string m_shaderSourceCode;
} CUSTOM_SHADER_T;

typedef struct  
{
	GLuint m_programId;
	float x;
	float y;
	float z;
	float w;
}CUSTOM_SHADER_VALUES_T;

class ShaderManager
{
public:
	static ShaderManager* getInstance()
	{
		if (instance == 0)
		{
			instance = new ShaderManager;
		}
		return instance;
	}

	//default dtor nothing to do 
	~ShaderManager() { ; }

	void ReadConfigFile(void);

	void LoadCustomShaders(void);

	std::string getShaderSource(GLuint shaderId);

	bool isShaderType(GLuint _shaderID, GLenum _shaderType);

	std::string injectStereoScopy(std::string _inputSource, GLuint programId);

	std::string injectFragmentModification(std::string _inputSource, GLuint programId);

	void applyShaderSource(GLuint _shaderID, std::string _shaderSource, const GLint *length);

	//are the shaders already applied ?
	bool GetShadersAppliedStatus(GLuint _vertexId, GLuint _fragmentId);

	// Set the shader pair as applied
	bool ApplyExceptionShaders(std::string &newShaderSource, GLenum shaderType, DWORD CRC32);

	// Apply custom separation + convergence
	void ApplyCustomShaderParams(GLuint progId);

	// Get the stored Shader Source from program and type
	std::string GetShaderSourceCode(GLuint programId, GLuint startIndex, GLenum shaderType);

	//ShaderInjection toggle
	inline void SetVertexInjectionState(bool state)
	{
		m_enableStereoVertexInjection = state;
	}

	inline void SetFgramentInjectionState(bool state)
	{
		m_enableStereoFragmentInjection = state;
	}

	inline bool GetVertexInjectionState(void)
	{
		return m_enableStereoVertexInjection;
	}

	inline bool GetStereoFailureInfoState(void)
	{
		return m_enableStereoFailureInfo;
	}

	inline bool GetFragmentInjectionState(void)
	{
		return m_enableStereoFragmentInjection;
	}

	//Vertex Automatic StereoInjection Stuff
	inline std::string GetVertexInjectionPoint1()
	{
		return m_vertexInjectionPoint1;
	}
	inline std::string GetVertexInjectionPoint2()
	{
		return m_vertexInjectionPoint2;
	}
	inline std::string GetVertexInjectionPoint3()
	{
		return m_vertexInjectionPoint3;
	}
	inline std::string GetVertexInjectionPoint4()
	{
		return m_vertexInjectionPoint4;
	}
	inline std::string GetVertexUniformPoint1()
	{
		return m_vertexUniformPoint1;
	}
	inline std::string GetVertexUniformPoint2()
	{
		return m_vertexUniformPoint2;
	}
	inline std::string GetVertexUniformPoint3()
	{
		return m_vertexUniformPoint3;
	}
	inline std::string GetVertexUniformPoint4()
	{
		return m_vertexUniformPoint4;
	}
	inline std::string GetVertexStereoInjectString()
	{
		return m_vertexStereoInjectionString;
	}

	// Shaders Exception Handling
	inline bool areShadersApplied( int _shaderPairIndex)
	{
		return m_exceptShaders[_shaderPairIndex].m_isApplied;
	}

	//Exisint shaders + source at creation
	inline void addExistingShaderInfo(EXISTING_SHADER_T _shaders)
	{
		m_existingShaders.push_back(_shaders);
	}

#ifdef DEBUG_WRAPPER
	inline std::vector<EXISTING_SHADER_T> GetExistingShaders(void)
	{
		return m_existingShaders;
	}

	// Set the debug shader pair as applied
	bool ApplyDebugExceptionShaders(void);

	// The program Id is found in the existing shader list??
	// Returns -1 if not found or the index position
	int ShaderProgramFound(int progId, GLuint shaderType);

	// get the start exception shader
	inline int GetExceptionShaderStart()
	{
		return m_shaderStart;
	}

	// get the end exception shader
	inline int GetExceptionShaderEnd()
	{
		return m_shaderEnd;
	}

	// Are vertex shaders exceptions enabled ???
	inline bool VertexShadersExceptionsEnabled()
	{
		return m_exceptShadersEnabled;
	}

	// Do we try to disable the current vertex shader ?
	inline bool VertexShaderExceptionsDisableShader()
	{
		return m_disableExceptShaders;
	}

	inline bool DisableCurrentShader()
	{
		return m_disableCurrentShader;
	}

	inline std::string GetVertexDisableString()
	{
		return m_vertexDisableString;
	}

	inline std::string GetPixelDisableString()
	{
		return m_pixelDisableString;
	}

#endif
	
private:
	//default ctor 
	ShaderManager();
	static ShaderManager * instance;
	ShaderManager(const ShaderManager&);
	ShaderManager& operator= (const ShaderManager&);
	// our shader list containing the vertex + fragment pair and new shader source
	std::vector<CUSTOM_SHADER_T> m_exceptShaders;

	bool m_enableStereoVertexInjection;
	bool m_enableStereoFailureInfo;
	bool m_enableStereoFragmentInjection;

	std::string m_vertexInjectionPoint1;
	std::string m_vertexInjectionPoint2;
	std::string m_vertexInjectionPoint3;
	std::string m_vertexInjectionPoint4;
	std::string m_vertexUniformPoint1;
	std::string m_vertexUniformPoint2;
	std::string m_vertexUniformPoint3;
	std::string m_vertexUniformPoint4;
	std::string m_vertexStereoInjectionString;

	
#ifdef DEBUG_WRAPPER
	// Shader Exceptions. Debug Only
	bool m_exceptShadersEnabled;
	bool m_disableExceptShaders;
	bool m_disableCurrentShader;
	int m_shaderStart;
	int m_shaderEnd;

	std::string  m_vertexDisableString;
	std::string  m_pixelDisableString;

#endif
std::vector<EXISTING_SHADER_T> m_existingShaders;
std::vector<CUSTOM_SHADER_VALUES_T> m_customShaderValues;
};

// Public Functions
void CheckCompileState(GLuint newShaderId);
void CheckLinkState(GLuint programId);
void DeleteAndDetachShaders(GLuint progId, GLuint shaderId);
void ExportShader(std::string _shaderType, GLuint _program, DWORD crc32, std::string _shaderSource);
void ExportCompiledShader(std::string _shaderSource);


#endif

