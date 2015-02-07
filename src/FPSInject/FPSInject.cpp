#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#include "Psapi.h"
#include "FPSInject.h"


static void *toggleAddress;
static DWORD pid;

/// @brief DLL EXPORT specific access exports
extern "C" __declspec(dllexport) FPSInject* __cdecl create_object()
{
	return new FPSInject();
}
/// --------------------------------------------------------------------------------------------------------------------------------->

extern "C" __declspec(dllexport) void __cdecl destroy_object(FPSInject* object)
{
	delete object;
}

/// --------------------------------------------------------------------------------------------------------------------------------->
// ACTUAL FUNCTIONS
/// --------------------------------------------------------------------------------------------------------------------------------->

void FPSInject::synchronizeRendering()
{
	// Get our PID
	std::string process = "BrokenAge.exe";
	DWORD64 baseAddress;
	HANDLE pHandle;

	getBaseAddress(process, &baseAddress, &pHandle);
	
	if (baseAddress != -1)
	{
		DWORD offset = 0x19102;
		std::string shellcode = "\xEB";

		//Add our offset
		baseAddress = baseAddress + offset;

		// Do the writting
		WriteProcessMemory(pHandle, (LPVOID)baseAddress, shellcode.c_str(), shellcode.length(), NULL);

		//Close the process handle
		CloseHandle(pHandle);
	}
}
/// --------------------------------------------------------------------------------------------------------------------------------->

void FPSInject::syncLeft()
{
	//MessageBox(NULL, "SYNC LEFT", "TEST", MB_OK);
}
/// --------------------------------------------------------------------------------------------------------------------------------->

void FPSInject::syncRight()
{
	//MessageBox(NULL, "SYNC RIGHT", "TEST", MB_OK);
}
/// --------------------------------------------------------------------------------------------------------------------------------->







/// PRIVATE FUNCTIONS

std::string FPSInject::getBytes(DWORD64 _number)
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
/// --------------------------------------------------------------------------------------------------------------------------------->

void FPSInject::getBaseAddress(std::string processName, DWORD64* pBaseAddress, HANDLE* pHandle)
{
	// Do some magic from here on
	DWORD pid = 0;
	HANDLE hsnap;
	PROCESSENTRY32 pt;
	hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	do
	{
		if (!strcmp(pt.szExeFile, processName.c_str()))
		{
			pid = pt.th32ProcessID;
			CloseHandle(hsnap);
		}
	} while (Process32Next(hsnap, &pt));


	// Get our process handle
	*pHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	HMODULE *hModules;
	char szBuf[50];
	DWORD cModules = 1024;
	*pBaseAddress = (DWORD64)-1;
	size_t MODULE = 0;
	MODULE = LIST_MODULES_32BIT;

	//Find our baseAddress
	EnumProcessModulesEx(*pHandle, NULL, 0, &cModules, (DWORD)MODULE);
	hModules = new HMODULE[cModules / sizeof(HMODULE)];

	if (EnumProcessModulesEx(*pHandle, hModules, cModules / sizeof(HMODULE), &cModules, (DWORD)MODULE))
	{
		for (unsigned int i = 0; i < cModules / sizeof(HMODULE); i++)
		{
			if (GetModuleBaseName(*pHandle, hModules[i], szBuf, sizeof(szBuf)))
			{
				if (processName.compare(szBuf) == 0)
				{
					*pBaseAddress = (DWORD64)hModules[i];
					break;
				}
			}
		}
	}
	delete[] hModules;
}
/// --------------------------------------------------------------------------------------------------------------------------------->