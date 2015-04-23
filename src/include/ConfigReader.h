/*
* OpenGL - 3D Vision Wrapper V.1.5
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

	bool GetPrintDebugInfoState()
	{
		return m_ScreenBufferCallTracer;
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

	bool GetWindowModeEnabled()
	{
		return m_windowModeEnabled;
	}
	// ---------------------------------------------------------------------------------------------

	void SetWindowModeEnabled(bool setMode)
	{
		m_windowModeEnabled = setMode;
	}
	// ---------------------------------------------------------------------------------------------

	bool GetWindowResizeEnabled()
	{
		return m_windowResizeEnabled;
	}
	// ---------------------------------------------------------------------------------------------

	bool IsAutomaticHookMode()
	{
		return m_automaticInjectionPoint;
	}
	// ---------------------------------------------------------------------------------------------

	unsigned int GetDefaultConvergence()
	{
		return m_defaultConvergence;
	}
	// ---------------------------------------------------------------------------------------------

private:
	//Log
	bool m_enableSplashScreen;
	bool m_enableLog;
	bool m_enableShaderDump;
	
	// Insertion Point	
	bool m_ScreenBufferCallTracer;
	bool m_3dVisionEnabled;
	bool m_windowModeEnabled;
	bool m_windowResizeEnabled;
	std::string m_injectionPoint;
	bool m_automaticInjectionPoint;
	bool m_enableVertexStereoInjection;

	// OpenGL Legacy Code Flag
	bool m_legacyOpenGL;

	//3D Settings specific
	bool m_enableToggleMode;
	int m_altKey;
	float m_altConvergence;
	float m_depthFactor;
	unsigned int m_defaultConvergence;

};


#endif