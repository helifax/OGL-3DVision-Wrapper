#include <sstream>
#include "ConfigReader.h"

static std::string GetPath() 
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);	
}
// -----------------------------------------------------------------------------------

configReader::configReader()
{
	ReadConfigFile();
}
// -----------------------------------------------------------------------------------

void configReader::ReadConfigFile()
{
	m_enableLog = false;
	m_enableShaderDump = false;
	m_enableVertexStereoInjection = false;

#ifdef DEBUG_WRAPPER
	std::string configFileName = GetPath() + "/3DVisionWrapperDBG.ini";
#else
	std::string configFileName = GetPath() + "/3DVisionWrapper.ini";
#endif
	std::ifstream configFile(configFileName);
	std::string configLine;

	//file is opened
	if (configFile.is_open())
	{
		// read one line at a time
		while (getline(configFile, configLine))
		{
			//----------------------------------------------
			// Find the log information
			if (configLine.find("[Log]") != std::string::npos)
			{
				//get the next line
				while (configLine.find("EnableSplashScreen") == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_enableSplashScreen = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_enableSplashScreen = false;
					}
				}

				//get the next line
				while (configLine.find("EnableLog") == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_enableLog = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_enableLog = false;
					}
				}
#ifdef DEBUG_WRAPPER
				while (configLine.find("EnableShaderDump") == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_enableShaderDump = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_enableShaderDump = false;
					}
				}
#endif
			}
			//----------------------------------------------

			// [InsertionPoint]
			size_t foundLocation;
			if (configLine.find("[3D_Vision_Global_Settings]") != std::string::npos)
			{
				while ((foundLocation = configLine.find("Enable3DVision")) == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_3dVisionEnabled = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_3dVisionEnabled = false;
					}
				}

				while ((foundLocation = configLine.find("ForceFullScreen")) == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_forceFullScreen = TRUE;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_forceFullScreen = FALSE;
					}
				}
				while ((foundLocation = configLine.find("ForceWindowModeSupport")) == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_windowModeEnabled = TRUE;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_windowModeEnabled = FALSE;
					}
				}
				while ((foundLocation = configLine.find("FullScreenDetection")) == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_fullscreenDetection = TRUE;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_fullscreenDetection = FALSE;
					}
				}
				while ((foundLocation = configLine.find("DepthMultiplicationFactor")) == std::string::npos)
				{
					getline(configFile, configLine);
					size_t position = configLine.find("= ");
					std::string value = configLine.substr(position + 1);

					// Read the depth hack
					m_depthFactor = std::stof(value, 0);
				}
				while ((foundLocation = configLine.find("DefaultConvergence")) == std::string::npos)
				{
					getline(configFile, configLine);
					size_t position = configLine.find("= ");
					std::string value = configLine.substr(position + 1);

					// Read the default convergence
					std::stringstream ss;
					ss << std::hex << value;
					ss >> m_defaultConvergence;
				}
				while ((foundLocation = configLine.find("ForceNVProfileLoad")) == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("true") != std::string::npos)
					{
						m_forceNvProfileLoad = TRUE;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_forceNvProfileLoad = FALSE;
					}
				}
				while ((foundLocation = configLine.find("3DVisionRating")) == std::string::npos)
				{
					getline(configFile, configLine);
					size_t position = configLine.find("= ");
					std::string value = configLine.substr(position + 1);

					// Read the value
					m_3DVisionRating = std::stof(value, 0);
				}
			}

			if (configLine.find("[Render_Control_Options]") != std::string::npos)
			{
				bool found = false;
				while ((foundLocation = configLine.find("RenderMode")) == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("\"SCREEN_BUFFER\"") != std::string::npos)
					{
						m_injectionPoint = "SCREEN_BUFFER";
						found = true;
					}
					else if (configLine.find("\"NONE\"") != std::string::npos)
					{
						m_injectionPoint = "NONE";
						found = true;
					}
					else if (configLine.find("RenderMode") != std::string::npos)
					{
						// If we are printing the information is doesnt matter
						if (Get3DVisionEnabledStatus() == true)
						{
							MessageBox(NULL, "Unknown Injection Point. Use:\n SCREEN_BUFFER\n\nNONE", "ASSERT", MB_OK);
							exit(EXIT_FAILURE);
						}
					}
				}
			}

			// [Legacy_OpenGL_Calls]
			if (configLine.find("[Legacy_OpenGL_Calls]") != std::string::npos)
			{
				while ((foundLocation = configLine.find("LegacyMode")) == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("false") != std::string::npos)
					{
						m_legacyOpenGL = false;
					}
					else if (configLine.find("true") != std::string::npos)
					{
						m_legacyOpenGL = true;
					}
				}

				while ((foundLocation = configLine.find("LegacyHUDSeparation")) == std::string::npos)
				{
					getline(configFile, configLine);
					size_t position = configLine.find("= ");
					std::string value = configLine.substr(position + 1);

					// Read the value
					m_legacyHUDSeparation = std::stof(value, 0);
				}
			}

			//[Alternative_3D_Settings]
			if (configLine.find("[Alternative_3D_Settings]") != std::string::npos)
			{
				while ((foundLocation = configLine.find("EnableToggleMode")) == std::string::npos)
				{
					getline(configFile, configLine);
					if (configLine.find("false") != std::string::npos)
					{
						m_enableToggleMode = false;
					}
					else if (configLine.find("true") != std::string::npos)
					{
						m_enableToggleMode = true;
					}
				}
				
				while ((foundLocation = configLine.find("AltConvergenceKey")) == std::string::npos)
				{
					getline(configFile, configLine);
					size_t position = configLine.find("= ");
					std::string value = configLine.substr(position + 1);

					// Read the key
					m_altKey = std::stoul(value, nullptr, 16);
				}
				
				while ((foundLocation = configLine.find("ConvergenceValue")) == std::string::npos)
				{
					getline(configFile, configLine);
					size_t position = configLine.find("= ");
					std::string value = configLine.substr(position + 1);

					// Read the convergence
					m_altConvergence = std::stof(value, 0);
				}
			}
		}
		configFile.close();
	}
}
// -----------------------------------------------------------------------------------