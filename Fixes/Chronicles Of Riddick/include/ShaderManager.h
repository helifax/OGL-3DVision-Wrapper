// Handle the Shaders
// To do Better description

#include <string>
#include <vector>
#include <gl_custom.h>


#ifndef __SHADER__MANAGER_H__
#define __SHADER__MANAGER_H__


typedef struct ShaderInfo
{
	std::vector <GLuint> _shaderID;
	std::vector <GLenum> _shaderType;
}SHADER_INFO_T;

// Structure to keep track of unique vertex+pixel shader pairs
typedef struct
{
	GLuint m_pixelShader;
	GLuint m_vertexShader;
} SHADER_EXCEPT_T;

class ShaderManager
{
public:
	//default ctor nothing to do 
	ShaderManager(){ ; }
	//default dtor nothing to do 
	~ShaderManager(){ ; }

	std::string getShaderSource(GLsizei count, const GLchar **string);

	bool isShaderType(GLuint _shaderID, GLenum _shaderType);

	std::string injectStereoScopy(std::string _inputSource);

	std::string injectFragmentModification(std::string _inputSource);
	std::string debug_injectFragmentModification(std::string _inputSource);

	std::string flipPositionInShader(std::string _inputSource);

	void applyShaderSource(GLuint _shaderID, std::string _shaderSource, const GLint *length);
	
private:
	//Current shaderID
	GLuint m_shaderID;
	// Original Shader Source
	std::string m_origSource;
	// New Shader Source
	std::string m_newSource;
	//our shader lookup table
	SHADER_INFO_T m_shaderInfo;
};


extern std::vector<SHADER_EXCEPT_T> g_exceptShaders;
#ifdef NDEBUG
typedef struct
{
	GLuint m_programId;
	GLuint m_pixelShader;
	GLuint m_vertexShader;
} SHADER_DEBUG_T;

extern std::vector<SHADER_DEBUG_T> g_debug_ShaderPair;

// Add a pair to the except
inline void addDebugtShaderPair(int ProgramId, int VertexID, int PixelID)
{
	SHADER_DEBUG_T temp;
	temp.m_programId = ProgramId;
	temp.m_pixelShader = PixelID;
	temp.m_vertexShader = VertexID;
	g_debug_ShaderPair.push_back(temp);
}

extern int debugShaderIndex;
extern bool injected;
extern bool restoreDefault;
#endif

// Add a pair to the except
inline void addExceptShader(int VertexID, int PixelID)
{
	SHADER_EXCEPT_T temp;
	temp.m_pixelShader = PixelID;
	temp.m_vertexShader = VertexID;
	g_exceptShaders.push_back(temp);
}



#endif
