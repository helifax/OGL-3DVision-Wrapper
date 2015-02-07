/*
* OpenGL - 3D Vision Wrapper V.1.01
* Copyright (c) Helifax 2014
*/

#ifndef __FPS_INJECT_H__
#define __FPS_INJECT_H__

#include <vector>

/// @brief Returns the bytes that will be written in memory
/// @brief Important: They are in reverse order exactly as they appear in memory
inline std::string getBytes(DWORD64 _number)
{
	std::string result;

	// create our array of bytes
	std::vector<unsigned char> arrayOfByte(4);
	for (int i = 0; i < 4; i++)
	{
		arrayOfByte[3 - i] = (unsigned char)(_number >> (i * 8));
	}

	// Copy the array of bytes in the string
	for (int i = 0; i <4; i++)
	{
		result += arrayOfByte[i];
	}

	// Get the inverse bytes to write in memory
	std::string inverse;
	inverse.resize(4);

	for (int i = 0; i < 4; i++)
	{
		inverse[i] = result[3 - i];
	}
	return inverse;
}

/// @brief Init the procedure
void synchronizeRendering(void);

void syncLeft(void);

void syncRight(void);

#endif