#include "include\ShaderManager.h"
#include "opengl32.h"

extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glGetShaderiv_t orig_glGetShader;
extern funct_glShaderSource_t orig_glShaderSource;

std::string ShaderManager::getShaderSource(GLsizei count, const GLchar **string)
{
	// Need to reset the string everytime we get here!!!
	m_origSource = "";
	for (int i = 0; i < count; i++)
	{
		m_origSource.append(*(string + i));
	}
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

	void ShaderManager::applyShaderSource(GLuint _shaderID, std::string _shaderSource, const GLint *length)
	{
		// only if we don't have it allready
		if (orig_glShaderSource == 0x00)
		{
			orig_glShaderSource = (funct_glShaderSource_t)orig_wglGetProcAddress("glShaderSource");
		}
		//MessageBox(NULL, m_newSource.c_str(), "DBG", MB_OK);
		const GLchar* source = _shaderSource.c_str();
		(*orig_glShaderSource)(_shaderID, 1, &source, length);
	}
	///-------------------------------------------------------------------------------------------

	// Unique per game
	std::string ShaderManager::injectStereoScopy(std::string _inputSource)
	{
		m_newSource = _inputSource;
		size_t location;
		size_t count;
		GLint g_exceptShader = 0;

		std::string except1, except2, except3;

		// Shader OpenGL ID 4 original // Some UI/ load pictures and main title halo
		// Disabled
#if 0
		std::string except1 = "tmpvar_7.xyz = ((((vPos_2 + positionNormAge.xyz) * g_vScale) + g_vParticleToWorld) * 300.000);";
		std::string except2 = { "gl_Position = (tmpvar_7 * g_mViewProj);" };
		std::string except3 = "";
		if ((location = m_newSource.find(except1) != std::string::npos) && (location = m_newSource.find(except2) != std::string::npos))
		{
			// save the current shader so we don't write to it again
			g_exceptShader = m_shaderID;
		}
#endif

		// Shader OpenGL ID 9 original //UI
#if 0
		except1 = "gl_Position = position;";
		except2 = "uvVarying = uv;";
		except3 = "colorVarying = color;";
		if ((location = m_newSource.find(except1) != std::string::npos) && (location = m_newSource.find(except2) != std::string::npos) && (location = m_newSource.find(except3) != std::string::npos))
		{
			// save the current shader so we don't write to it again
			g_exceptShader = m_shaderID;
		}
#endif

		// Shader OpenGL ID 21 original //toggle between persons
#if 0
		except1 =  "uvVarying2 = (((uv - 0.500000) * g_vUVScale) + 0.500000);";
		except2 =  "uvVarying = uv;";
		except3 =  "gl_Position = position;";
		if ((location = m_newSource.find(except1) != std::string::npos) && (location = m_newSource.find(except2) != std::string::npos) && (location = m_newSource.find(except3) != std::string::npos))
		{
			// save the current shader so we don't write to it again
			g_exceptShader = m_shaderID;
		}
#endif

		// Insert stereo
#if 1
		location = m_newSource.find("_OutPos.w = ");
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
			// Do the shader gl_Position insertion
			//separation 65.00000 - used - interaxial /Screen Width
			//convergence 1.515831 - used - 
			//eye separation 0.043882 -not used = interocular / real screen width

			if ((m_shaderID != (GLuint)g_exceptShader)) // Shaders for UI
			{
				//m_newSource.insert(count + 1, "\n if ( ((gl_Position.w <= 0.4) && (gl_Position.w > 0.1)) && (gl_Position.x < 0.35) && (gl_Position.y < 0.3) && (gl_Position.z < 0.4) ){\n gl_Position.x += g_eye * g_eye_separation * (gl_Position.w - g_convergence/4);\n } else {\n gl_Position.x += g_eye * g_eye_separation * (gl_Position.w - g_convergence);\n}\n");
				//m_newSource.insert(count + 1, "\n if ( (_OutPos.w <= g_convergence/2) && (_OutPos.z < g_convergence/2) ){\n _OutPos.x += g_eye * g_eye_separation * (_OutPos.w - g_convergence/8);\n } else {\n _OutPos.x += g_eye * g_eye_separation * (_OutPos.w - g_convergence);\n}\n");
				m_newSource.insert(count + 1, "\n _OutPos.x += g_eye * g_eye_separation * (_OutPos.w - g_convergence);\n");
			}
		}
#endif

		//Adding the uniforms
#if 1
		location = m_newSource.find("#version ");
		count = location;

		if (location != std::string::npos)
		{
			// Need to count until the end of the current gl_Position
			// And apply the flip afterwards
			for (size_t i = location; i < m_newSource.size(); i++)
				//for (int i = location; i > 0; i--)
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
		}
#endif	
		//MessageBox(NULL, m_newSource.c_str(), "DEBUG", MB_OK);
		return m_newSource;
	}
///-------------------------------------------------------------------------------------------

	// Unique per game
	std::string ShaderManager::injectFragmentModification(std::string _inputSource)
	{
		m_newSource = _inputSource;
		size_t location;
		size_t count;

		// Insert Modification
#if 1
		location = m_newSource.find("float4 final = float4(Occlusion, 0.0, 0.0, 1.0 - Occlusion);");
		count = location;
		if (location != std::string::npos)
		{
			// Light fixing
			{
				std::string temp = "float4 final = float4(Occlusion, 0.0, 0.0, 2.0 - Occlusion); \n";
				m_newSource.replace(count, temp.length(), temp);
			}
		}
#endif

#if 1
		location = m_newSource.find("DoFog(FogConst0,FogConst1,FogConst2,FogConst3,ScreenCoord,Fog1,FogResult,FogAlphaScale);");
		count = location;
		if (location != std::string::npos)
		{
			// Light fixing
			if (m_shaderID == 495)
			{
				std::string temp = "DoFog(FogConst0,FogConst1,FogConst2,FogConst3,ScreenCoord,Fog1,FogResult,FogAlphaScale);\n";
				//m_newSource.replace(count, temp.length(), temp);
				//MessageBox(NULL, m_newSource.c_str(), "DEBUG", MB_OK);
			}
		}
#endif

		//DOF
#if 1
		// Add a uniform
		location = m_newSource.find("#version ");
		count = location;

		if (location != std::string::npos)
		{
			// Need to count until the end of the current gl_Position
			// And apply the flip afterwards
			for (size_t i = location; i < m_newSource.size(); i++)
				//for (int i = location; i > 0; i--)
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
			m_newSource.insert(count + 1, "uniform float g_eye_separation_pixel;\n"); // 0.17 worked
		}

		//DOF
		location = m_newSource.find("float4 TexMV = SampleFPTexLinear2D(Sampler_MV, tc_mvlow.xy,ScreenDimRcp.xy * 4.0);");
		count = location;
		if (location != std::string::npos)
		{
			if (m_shaderID == 879)
			{
				std::string temp = "//float4 TexMV = SampleFPTexLinear2D(Sampler_MV, tc_mvlow.xy, ScreenDimRcp.xy * 4.0);\n";
				m_newSource.replace(count, temp.length(), temp);
				m_newSource.insert(count + temp.length(), "float4 TexMV = SampleFPTexLinear2D(Sampler_MV, float2(tc_mvlow.x + g_eye_separation_pixel, tc_mvlow.y), ScreenDimRcp.xy * 4.0);\n");
				//MessageBox(NULL, m_newSource.c_str(), "DEBUG", MB_OK);
			}
		}
#endif
		return m_newSource;
	}
	///-------------------------------------------------------------------------------------------

	std::string ShaderManager::debug_injectFragmentModification(std::string _inputSource)
	{
		m_newSource = _inputSource;
		size_t count = 0;


		// Insert Modification
#if 1
		size_t location = 0;
		location = m_newSource.find("void main");
		count = location;
		if (location != std::string::npos)
		{
			// Need to count until the end of the current gl_Position
			// And apply the flip afterwards
			for (size_t i = location; i < m_newSource.size(); i++)
			{
				count++;
				if (m_newSource[i] == *("}"))
				{
					break;
				}
			}
			// Do the shader gl_Position insertion
			m_newSource.insert(count - 1, "\ngl_FragData = vec4(1,0,0,0);\n");
			//MessageBox(NULL, m_newSource.c_str(), "DEBUG", MB_OK);
		}
#endif
		return m_newSource;
	}