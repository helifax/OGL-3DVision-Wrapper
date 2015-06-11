#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#include "Psapi.h"
#include "FPSInject.h"


void *toggleAddress;
DWORD pid;


void synchronizeRendering(void)
{
	// Get our PID
	std::string process = "Amnesia.exe";
	DWORD offset = 0x131C60; // Steam value v.1.03
	std::string shellcode ("\x50\xA1\x44\xA4\x18\x00\x83\xF8\x02\x0F\x85\x06\x00\x00\x00\x0F\x84\x14\x00\x00\x00\x83\x05\x44\xA4\x18\x00\x01\x58\xE9\x79\xBC\xE8\xFA\x6A\x0A\xE8\xC7\xD4\xE8\xFA\xC7\x05\x44\xA4\x18\x00\x01\x00\x00\x00\x58\x6A\x0A\xE9\x37\xBC\xE8\xFA", 59);
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
	DWORD64 logicJumpTo1 = baseAddress + 0x131C69;
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

	// jump 1st to Amnesia.exe + 0xEAB4B
	/// -------------------------------------------------------------------
	// Write the addresses in the CC where to jump in case of conditions
	DWORD64 insertPosition = 0x1E;
	DWORD64 jumpBackAddress1 = logicJumpTo1 - (DWORD64(codeCaveAddress) + insertPosition + 0x04);
	// Write it
	DWORD64 address1 = (DWORD)codeCaveAddress + insertPosition;
	WriteProcessMemory(p, (LPVOID)address1, &jumpBackAddress1, 0x04, NULL);
	/// -------------------------------------------------------------------

	// jump 2nd to Amnesia.exe + 0xEC3A0
	/// -------------------------------------------------------------------
	// Write the addresses in the CC where to jump in case of conditions
	insertPosition = 0x25;
	DWORD64 jumpBackAddress2 = logicJumpTo2 - (DWORD64(codeCaveAddress) + insertPosition + 0x04);
	// Write it
	DWORD64 address2 = (DWORD)codeCaveAddress + insertPosition;
	WriteProcessMemory(p, (LPVOID)address2, &jumpBackAddress2, 0x04, NULL);
	/// -------------------------------------------------------------------


	// Write the JUMP BACK
	/// -------------------------------------------------------------------
	// Calculate the jump back
	DWORD64 jumpBackAddress = (injectAddress + 0x05) - (DWORD64(codeCaveAddress) + shellcode.length());
	// Do the writting
	DWORD64 address = (DWORD64)codeCaveAddress + shellcode.length()  - 0x04;
	WriteProcessMemory(p, (LPVOID)address, &jumpBackAddress, 0x04, NULL);
	/// -------------------------------------------------------------------

	// Write the JUMP TO
	/// -------------------------------------------------------------------
	// Calculate the jump to
	DWORD64 jumpAddress = DWORD64(codeCaveAddress) - injectAddress - (/*A jump is always 5bytes */ 0x05);
	
	// Write the initial jump
	std::string jmpCC = "\xE9\x90\x90\x90\x90\x90\x90\x90\x90";
	WriteProcessMemory(p, (LPVOID)injectAddress, jmpCC.c_str(), jmpCC.length(), NULL);

	//Write to address
	address = injectAddress + 0x01;
	// Write the address
	WriteProcessMemory(p, (LPVOID)address, &jumpAddress, 0x04, NULL);
	/// -------------------------------------------------------------------

	//Close the process handle
	CloseHandle(p);
}

void syncLeft(void)
{
	// Get our process handle
	HANDLE p;
	p = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	// Write the initial value in the new memory location
	WriteProcessMemory(p, (LPVOID)toggleAddress, "\x01", sizeof(DWORD), NULL);

	//Close the process handle
	CloseHandle(p);
}

void syncRight(void)
{
	// Get our process handle
	HANDLE p;
	p = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	// Write the initial value in the new memory location
	WriteProcessMemory(p, (LPVOID)toggleAddress, "\x02", sizeof(DWORD), NULL);

	//Close the process handle
	CloseHandle(p);
}