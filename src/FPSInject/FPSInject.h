/*
* OpenGL - 3D Vision Wrapper
* Copyright (c) Helifax 2015
*/

#ifndef __FPS_INJECT_H__
#define __FPS_INJECT_H__

#include <vector>
/*
/// @brief Init the procedure
void synchronizeRendering(void);

void syncLeft(void);

void syncRight(void);
*/

class FPSInject
{
public:
	// ctor
	FPSInject() { ; }

	//dtor
	~FPSInject(){ ; } 
	
	// Sync left eye
	virtual void syncLeft(void);
	// Sync right eye
	virtual void syncRight(void);
	// Finish rendering
	virtual void synchronizeRendering(void);

private:
	/// @brief Returns the bytes that will be written in memory
	/// @brief Important: They are in reverse order exactly as they appear in memory
	std::string getBytes(DWORD64 _number);

	void getBaseAddress(std::string processName, DWORD64* pBaseAddress, HANDLE* pHandle);
};

#endif