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

// 50						- push eax
// A1 A8 EE 8A 00			- mov eax, [Amnesia.exe + 78EEA8]
// 83 F8 02					- cmp eax, 02
// 0F 85 06 00 00 00		- jne 03350015
// 0F 84 14 00 00 00		- je 03350029
// 83 05 A8 EE 8A 00 01		- add dword ptr[Amnesia.exe + 78EEA8], 01
// 58						- pop eax
// 6A 0A					- push 0A
// E8 0C 5B F0 FC			- call Amnesia.exe + 135B30 // call instruction
// E9 40 1C F0 FC			- jmp Amnesia.exe + 131C69 // Normal Return
// C7 05 A8 EE 8A 00 01 00 00 00 - mov[Amnesia.exe + 78EEA8], 00000001
// 58						- pop eax
// E9 60 1C F0 FC			- jmp Amnesia.exe + 131C99 // Je instrcution



void FPSInject::synchronizeRendering()
{
	// Write the initial code-cave or Bytes in the application memory.
	// Get our PID
	std::string process = "Amnesia.exe";
	DWORD offset = 0x131C60; // Steam value v.1.03
	std::string shellcode("\x50\xA1\xA8\xEE\x8A\x00\x83\xF8\x02\x0F\x85\x06\x00\x00\x00\x0F\x84\x14\x00\x00\x00\x83\x05\xA8\xEE\x8A\x00\x01\x58\x6A\x0A\xE8\x0C\x5B\xF0\xFC\xE9\x40\x1C\xF0\xFC\xC7\x05\xA8\xEE\x8A\x00\x01\x00\x00\x00\x58\xE9\x60\x1C\xF0\xFC", 57);
	DWORD64 baseAddress = -1;

	// Do some magic from here on
	HANDLE hsnap;
	PROCESSENTRY32 pt;
	hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	do
	{
		if (!strcmp(pt.szExeFile, process.c_str()))
		{
			pid = pt.th32ProcessID;
			CloseHandle(hsnap);
		}
	} while (Process32Next(hsnap, &pt));


	// Get our process handle
	HANDLE p;
	p = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	HMODULE *hModules;
	char szBuf[50];
	DWORD cModules = 1024;
	size_t MODULE = 0;
	MODULE = LIST_MODULES_32BIT;

	//Find our baseAddress
	EnumProcessModulesEx(p, NULL, 0, &cModules, (DWORD)MODULE);
	hModules = new HMODULE[cModules / sizeof(HMODULE)];

	if (EnumProcessModulesEx(p, hModules, cModules / sizeof(HMODULE), &cModules, (DWORD)MODULE))
	{
		for (unsigned int i = 0; i < cModules / sizeof(HMODULE); i++)
		{
			if (GetModuleBaseName(p, hModules[i], szBuf, sizeof(szBuf)))
			{
				if (process.compare(szBuf) == 0)
				{
					baseAddress = (DWORD64)hModules[i];
					break;
				}
			}
		}
	}
	delete[] hModules;

	//Add our offset
	DWORD64 injectAddress = baseAddress + offset;

	// Our Jump address (part of the CC but must be calculated)
	DWORD64 logicJumpTo1 = baseAddress + 0x135B30;
	
	// The CALL Address
	DWORD64 logicJumpTo2 = baseAddress + 0x131C99;

	// Allocate memory for the code cave
	void *codeCaveAddress = VirtualAllocEx(p, NULL, shellcode.length(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	// Write our CodeCave in our allocated memory space
	WriteProcessMemory(p, codeCaveAddress, shellcode.c_str(), shellcode.length(), NULL);

	// Allocate a new memory location for toggle flag // 0x04 bytes in this case
	toggleAddress = VirtualAllocEx(p, NULL, sizeof(DWORD), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	// Write the initial value in the new memory location // Initial value is 0x01
	WriteProcessMemory(p, (LPVOID)toggleAddress, "\x01", sizeof(DWORD), NULL);


	// Fill in the Code Cave locations
	/// -------------------------------------------------------------------
	std::string hexToggleAddress = getBytes((DWORD64)toggleAddress);
	// Calculate the first write location
	DWORD writeToggleAddress = (DWORD)codeCaveAddress + 0x02;
	// Write the new memory address inside the code
	WriteProcessMemory(p, (LPVOID)writeToggleAddress, hexToggleAddress.c_str(), hexToggleAddress.length(), NULL);

	// 2nd location
	writeToggleAddress = (DWORD)codeCaveAddress + 0x17;
	// Write the new memory address inside the code
	WriteProcessMemory(p, (LPVOID)writeToggleAddress, hexToggleAddress.c_str(), hexToggleAddress.length(), NULL);

	// 3rd location
	writeToggleAddress = (DWORD)codeCaveAddress + 0x2B;
	// Write the new memory address inside the code
	WriteProcessMemory(p, (LPVOID)writeToggleAddress, hexToggleAddress.c_str(), hexToggleAddress.length(), NULL);
	/// -------------------------------------------------------------------

	// jump 1st
	/// -------------------------------------------------------------------
	// Write the addresses in the CC where to jump in case of conditions
	DWORD64 insertPosition = 0x20;
	DWORD64 jumpBackAddress1 = logicJumpTo1 - (DWORD64(codeCaveAddress) + insertPosition + 0x04);
	// Write it
	DWORD64 address1 = (DWORD)codeCaveAddress + insertPosition;
	WriteProcessMemory(p, (LPVOID)address1, &jumpBackAddress1, 0x04, NULL);
	/// -------------------------------------------------------------------

	// jump 2nd
	/// -------------------------------------------------------------------
	// Write the addresses in the CC where to jump in case of conditions
	insertPosition = 0x35;
	DWORD64 jumpBackAddress2 = logicJumpTo2 - (DWORD64(codeCaveAddress) + insertPosition + 0x04);
	// Write it
	DWORD64 address2 = (DWORD)codeCaveAddress + insertPosition;
	WriteProcessMemory(p, (LPVOID)address2, &jumpBackAddress2, 0x04, NULL);
	/// -------------------------------------------------------------------


	// Write the JUMP BACK
	/// -------------------------------------------------------------------
	// Calculate the jump back
	DWORD64 address3 = (DWORD)codeCaveAddress + 0x25;
	DWORD64 jumpBackAddress = (injectAddress + 0x05) - (address3);
	// Do the writting
	WriteProcessMemory(p, (LPVOID)address3, &jumpBackAddress, 0x04, NULL);
	/// -------------------------------------------------------------------

	// Write the JUMP TO
	/// -------------------------------------------------------------------
	// Calculate the jump to
	DWORD64 jumpAddress = DWORD64(codeCaveAddress) - injectAddress - (/*A jump is always 5bytes */ 0x05);

	// Write the initial jump
	std::string jmpCC = "\xE9\x90\x90\x90\x90\x90\x90\x90\x90";
	WriteProcessMemory(p, (LPVOID)injectAddress, jmpCC.c_str(), jmpCC.length(), NULL);

	//Write to address
	DWORD64 address = injectAddress + 0x01;
	// Write the address
	WriteProcessMemory(p, (LPVOID)address, &jumpAddress, 0x04, NULL);
	/// -------------------------------------------------------------------

	//Close the process handle
	CloseHandle(p);
}
/// --------------------------------------------------------------------------------------------------------------------------------->

void FPSInject::syncLeft()
{
	// Write in Memory at a specific Location a series of Bytes that are valid for the left eye. 
	// Ex: tell the engine to keep on waiting
	// Get our process handle
	HANDLE p;
	p = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	// Write the initial value in the new memory location
	WriteProcessMemory(p, (LPVOID)toggleAddress, "\x01", sizeof(DWORD), NULL);

	//Close the process handle
	CloseHandle(p);
}
/// --------------------------------------------------------------------------------------------------------------------------------->

void FPSInject::syncRight()
{
	// Write in Memory at a specific Location a series of Bytes that are valid for the right eye. 
	// Ex: tell the engine to render the next frame
	// Get our process handle
	HANDLE p;
	p = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	// Write the initial value in the new memory location
	WriteProcessMemory(p, (LPVOID)toggleAddress, "\x02", sizeof(DWORD), NULL);

	//Close the process handle
	CloseHandle(p);
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