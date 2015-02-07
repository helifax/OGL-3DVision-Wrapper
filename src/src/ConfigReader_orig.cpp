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
	m_enableLog = false;
	m_enableShaderDump = false;
	m_enableVertexStereoInjection = false;
	m_ScreenBufferCallTracer = false;

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
				while (configLine.find("EnableScrBfrCallTracer") != std::string::npos)
				{
					if (configLine.find("true") != std::string::npos)
					{
						m_ScreenBufferCallTracer = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_ScreenBufferCallTracer = false;
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
				//get the next line
				getline(configFile, configLine);
				while ((foundLocation = configLine.find("Enable3DVision")) != std::string::npos)
				{
					if (configLine.find("true") != std::string::npos)
					{
						m_enableInjectionPoint = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_enableInjectionPoint = false;
					}
					getline(configFile, configLine);
				}
				while ((foundLocation = configLine.find("EnableWindowModeSupport")) != std::string::npos)
				{
					if (configLine.find("true") != std::string::npos)
					{
						m_windowModeEnabled = TRUE;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_windowModeEnabled = FALSE;
					}
					getline(configFile, configLine);
				}
				while ((foundLocation = configLine.find("EnableWindowResizeSupport")) != std::string::npos)
				{
					if (configLine.find("true") != std::string::npos)
					{
						m_windowResizeEnabled = TRUE;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_windowResizeEnabled = FALSE;
					}
					getline(configFile, configLine);
				}
				while ((foundLocation = configLine.find("DepthMultiplicationFactor")) != std::string::npos)
				{
					size_t position = configLine.find("= ");
					std::string value = configLine.substr(position + 1);

					// Read the depth hack
					m_depthFactor = std::stof(value, 0);


					getline(configFile, configLine);
				}
				while ((foundLocation = configLine.find("AutomaticHookPoint")) != std::string::npos)
				{
					if (configLine.find("true") != std::string::npos)
					{
						m_automaticInjectionPoint = true;
					}
					else if (configLine.find("false") != std::string::npos)
					{
						m_automaticInjectionPoint = false;
					}
					getline(configFile, configLine);
				}
			}

			// [Manual_Hook_Point_Options]
			if (configLine.find("[Manual_Hook_Point_Options]") != std::string::npos)
			{
				getline(configFile, configLine);
				while ((foundLocation = configLine.find("HookPoint")) != std::string::npos)
				{
					bool found = false;
					if (configLine.find("\"glBindFramebufferEXT\"") != std::string::npos)
					{
						m_injectionPoint = "glBindFramebufferEXT";
						found = true;
					}
					else if (configLine.find("\"glBindFramebufferARB\"") != std::string::npos)
					{
						m_injectionPoint = "glBindFramebufferARB";
						found = true;
					}
					else if (configLine.find("\"glBindFramebuffer\"") != std::string::npos)
					{
						m_injectionPoint = "glBindFramebuffer";
						found = true;
					}
					else if (configLine.find("\"glSwapBuffers\"") != std::string::npos)
					{
						m_injectionPoint = "glSwapBuffers";
						found = true;
					}
					else if (configLine.find("\"NONE\"") != std::string::npos)
					{
						m_injectionPoint = "NONE";
						found = true;
					}
					else
					{
						// If we are printing the information is doesnt matter
						if (!GetEnableInjectionPoint())
						{
							MessageBox(NULL, "Unknown Injection Point. Use:\n glBindFramebuffer\n glBindFramebufferARB\n glBindFramebufferEXT\n glSwapBuffers\n NONE\n", "ASSERT", MB_OK);
							exit(EXIT_FAILURE);
						}
					}
					getline(configFile, configLine);
				}

			}
			// [3D Settings]
			if (configLine.find("[Alternative_3D_Settings]") != std::string::npos)
			{
				//get the next line
				getline(configFile, configLine);
				if ((foundLocation = configLine.find("EnableToggleMode")) != std::string::npos)
				{
					if (configLine.find("false") != std::string::npos)
					{
						m_enableToggleMode = false;
					}
					else if (configLine.find("true") != std::string::npos)
					{
						m_enableToggleMode = true;
					}
				}
				getline(configFile, configLine);
				if ((foundLocation = configLine.find("AltConvergenceKey")) != std::string::npos)
				{
					size_t position = configLine.find("= ");
					std::string value = configLine.substr(position + 1);
					
					// Read the key
					m_altKey = std::stoul(value, nullptr, 16);
				}
				getline(configFile, configLine);
				if ((foundLocation = configLine.find("ConvergenceValue")) != std::string::npos)
				{
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