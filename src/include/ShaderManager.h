/*
* OpenGL - 3D Vision Wrapper V.1.5
* Copyright (c) Helifax 2015
*/

#ifndef __SHADER__MANAGER_H__
#define __SHADER__MANAGER_H__

#include <string>
#include <vector>
#include <stdlib.h> 
#include <gl_custom.h>

typedef struct ShaderInfo
{
	std::vector <GLuint> _shaderID;
	std::vector <GLenum> _shaderType;
}SHADER_INFO_T;

// Structure to keep track of unique vertex+pixel shader pairs
typedef struct
{
	GLuint m_programId;
	GLuint m_pixelShader;
	GLuint m_vertexShader;
	std::string m_vertexSource;
	std::string m_fragmentSource;
} EXISTING_SHADERS_T;


typedef struct
{
	GLuint m_programId;
	GLuint m_pixelShader;
	GLuint m_vertexShader;
	std::string m_vertexSource;
	std::string m_fragmentSource;
	bool m_isApplied;
} SHADER_EXCEPT_T;

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

	std::string getShaderSource(GLuint shaderId);

	bool isShaderType(GLuint _shaderID, GLenum _shaderType);

	std::string injectStereoScopy(std::string _inputSource, GLuint programId);

	std::string injectFragmentModification(std::string _inputSource);

	std::string flipPositionInShader(std::string _inputSource);

	void applyShaderSource(GLuint _shaderID, std::string _shaderSource, const GLint *length);

	//are the shaders already applied ?
	bool GetShadersAppliedStatus(GLuint _vertexId, GLuint _fragmentId);

	// Set the shader pair as applied
	bool ApplyExceptionShaders(void);

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
	inline void addExistingShaderInfo(EXISTING_SHADERS_T _shaders)
	{
		m_existingShaders.push_back(_shaders);
	}

#ifdef DEBUG_WRAPPER
	inline std::vector<EXISTING_SHADERS_T> GetExistingShaders(void)
	{
		return m_existingShaders;
	}

	// Set the debug shader pair as applied
	bool ApplyDebugExceptionShaders(void);

	// The program Id is found in the existing shader list??
	// Returns -1 if not found or the index position
	int ShaderProgramFound(int progId);

	// Save the source of the current compiled and modified shader
	void SaveNewSource(int progId, std::string ShaderSource, bool isVertex);

	// get the start exception shader
	inline int GetExceptionShaderVertexStart()
	{
		return m_shaderVStart;
	}

	// get the end exception shader
	inline int GetExceptionShaderVertexEnd()
	{
		return m_shaderVEnd;
	}

	// Are vertex shaders exceptions enabled ???
	inline bool VertexShadersExceptionsEnabled()
	{
		return m_exceptShadersEnabled;
	}

#endif
	
private:
	//default ctor 
	ShaderManager();
	static ShaderManager * instance;
	ShaderManager(const ShaderManager&);
	ShaderManager& operator= (const ShaderManager&);
	//Current shaderID
	GLuint m_shaderID;
	// Original Shader Source
	std::string m_origSource;
	// New Shader Source
	std::string m_newSource;
	//our shader lookup table
	SHADER_INFO_T m_shaderInfo;
	// our shader list containing the vertex + fragment pair and new shader source
	std::vector<SHADER_EXCEPT_T> m_exceptShaders;

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
	int m_shaderVStart;
	int m_shaderVEnd;
#endif
std::vector<EXISTING_SHADERS_T> m_existingShaders;
};

// Public Functions
void CheckCompileState(GLuint newVertexId);
void CheckLinkState(GLuint programId);
void DeleteAndDetachShaders(GLuint progId, GLuint shaderId);
void ExportShader(std::string _shaderType, GLuint _program, GLuint _shaderNumber, std::string _shaderSource);
void ExportCompiledShader(std::string _shaderSource);


#endif

