/*
* OpenGL - 3D Vision Wrapper
* Copyright (c) Helifax 2015
*/

#ifndef __CONFIG_READER_H__
#define __CONFIG_READER_H__

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "windows.h"

//Reads the basic information
// NOT the shader exceptions

class configReader
{
public:
	//ctor
	configReader();
	~configReader();
	// ---------------------------------------------------------------------------------------------

	void ReadConfigFile();
	// ---------------------------------------------------------------------------------------------

	 bool GetEnableLog()
	{
		return m_enableLog;
	}
	// ---------------------------------------------------------------------------------------------
	 bool GetEnableSplashScreen()
	{
		return m_enableSplashScreen;
	}
	// ---------------------------------------------------------------------------------------------

	 bool GetEnableShaderDump()
	{
		return m_enableShaderDump;
	}
	// ---------------------------------------------------------------------------------------------

	 std::string GetInjectionPoint()
	{
		return m_injectionPoint;
	}
	// ---------------------------------------------------------------------------------------------

	 void SetInjectionPoint(std::string value)
	 {
		 m_injectionPoint = value;
	 }
	 // ---------------------------------------------------------------------------------------------

	 bool GetVertexStereoInjection()
	{
		return m_enableVertexStereoInjection;
	}
	// ---------------------------------------------------------------------------------------------

	 bool isLegacyOpenGLEnabled()
	 {
		 return m_legacyOpenGL;
	 }
	 // ---------------------------------------------------------------------------------------------

	 float GetLegacyHUDSeparation()
	 {
		 return m_legacyHUDSeparation;
	 }
	 // ---------------------------------------------------------------------------------------------

	 bool GetKeyToggleMode()
	{
		return m_enableToggleMode;
	}
	// ---------------------------------------------------------------------------------------------

	 int GetKeyNumber()
	{
		return m_altKey;
	}
	// ---------------------------------------------------------------------------------------------

	 float GetAltConvergence()
	{
		return m_altConvergence;
	}
	// ---------------------------------------------------------------------------------------------

	float GetDepthFactor()
	{
		return m_depthFactor;
	}
	// ---------------------------------------------------------------------------------------------

	inline bool Get3DVisionEnabledStatus()
	{
		return m_3dVisionEnabled;
	}
	// ---------------------------------------------------------------------------------------------

	inline void Set3DVisionEnabledStatus(bool newStatus)
	{
		m_3dVisionEnabled = newStatus;
	}
	// ---------------------------------------------------------------------------------------------

	inline bool IsFullScreenForced()
	{
		return m_forceFullScreen;
	}
	// ---------------------------------------------------------------------------------------------

	inline bool IsWindowModeSupportEnabled()
	{
		return m_windowModeEnabled;
	}
	// ---------------------------------------------------------------------------------------------

	inline bool IsFullScreenDetectionEnabled()
	{
		return m_fullscreenDetection;
	}
	// ---------------------------------------------------------------------------------------------

	unsigned int GetDefaultConvergence()
	{
		return m_defaultConvergence;
	}
	// ---------------------------------------------------------------------------------------------

	bool IsNVProfileForced()
	{
		return m_forceNvProfileLoad;
	}
	// ---------------------------------------------------------------------------------------------

	float Get3DVisionRating()
	{
		return m_3DVisionRating;
	}
	// ---------------------------------------------------------------------------------------------

private:
	//Log
	bool m_enableSplashScreen;
	bool m_enableLog;
	bool m_enableShaderDump;
	
	// Insertion Point	
	bool m_3dVisionEnabled;
	bool m_forceFullScreen;
	bool m_windowModeEnabled;
	bool m_fullscreenDetection;
	std::string m_injectionPoint;
	bool m_enableVertexStereoInjection;

	// OpenGL Legacy Code Flag
	bool m_legacyOpenGL;
	float m_legacyHUDSeparation;

	//3D Settings specific
	bool m_enableToggleMode;
	int m_altKey;
	float m_altConvergence;
	float m_depthFactor;
	unsigned int m_defaultConvergence;
	bool m_forceNvProfileLoad;
	float m_3DVisionRating;

};


#endif