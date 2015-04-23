/*
Author: Helifax
Data: 24.94.2015

Interface to NVAPI to handle profile modification/creation in order for 3D Vision to render Properly!
*/

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "NVAPI_343\nvapi.h"
#include "NVAPI_343\NvApiDriverSettings.h"
#include "ConfigReader.h"

extern "C" {
#include "opengl_3dv.h"
}

#define NVAPI_STEREO_CONVERGENCE_ID				0x708DB8C5
#define NVAPI_STEREO_WINDOWED_3D_SUPPORT_ID		0x701EB457

static char m_exeName[255] = "";
static char m_profileName[255] = "";
static char m_friendlyName[255] = "";

extern configReader *g_reader;
extern void __cdecl add_log(const char * fmt, ...);
static bool NvApi_IsProfileFound(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile);
static bool NvApi_Enable3DVisionSetting(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile);
static bool NvApi_LoadDefaultProfile(NvDRSSessionHandle hSession);
static void NvApi_SetUnicodeString(NvAPI_UnicodeString& nvstr, const char * cstr);
static void NvApi_ReadProfile(void);
static bool NvApi_ApplyProfileFromFile(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile);
static void _getProfileName(const char* inExeName, char* outProfileName);
static void _getApplicationName(char *exePath);

static void PrintError(NvAPI_Status status)
{
	NvAPI_ShortString szDesc = { 0 };
	NvAPI_GetErrorMessage(status, szDesc);
	add_log(" NVAPI error: %s\n", szDesc);
}
///--------------------------------------------------------------------------------------

// Enumerate profiles and find the current profile for the .exe
void NvApi_3DVisionProfileSetup()
{
	NvAPI_Status status;
	bool profileFound = false;

	// (0) Initialize NVAPI. This must be done first of all
	status = NvAPI_Initialize();
	if (status != NVAPI_OK)
		PrintError(status);
	
	// (1) Create the session handle to access driver settings
	NvDRSSessionHandle hSession = 0;
	status = NvAPI_DRS_CreateSession(&hSession);
	if (status != NVAPI_OK)
		PrintError(status);
	
	// (2) load all the system settings into the session
	status = NvAPI_DRS_LoadSettings(hSession);
	if (status != NVAPI_OK)
		PrintError(status);
	
	// (3) Enumerate through all the profiles. Do so until the
	// return value is NVAPI_END_ENUMERATION.
	NvDRSProfileHandle hProfile = 0;
	unsigned int index = 0;
	
	// Get the current Application Name
	_getApplicationName(m_exeName);
	
	while ((status = NvAPI_DRS_EnumProfiles(hSession, index, &hProfile)) == NVAPI_OK) 
	{
		// (4) hProfile is now a valid Profile handle.
		// retrieve information from the Profile
		profileFound = NvApi_IsProfileFound(hSession, hProfile);
		if (profileFound)
		{
			// Set the magic flag to enable 3D Vision for this profile
			if (NvApi_Enable3DVisionSetting(hSession, hProfile))
			{
				add_log("Game profile existing/updated with the 3D Vision flag !!!");
			}
			break;
		}
		index++;
	}

	if (!profileFound)
	{
		add_log("Existing profile not found for %s application! Attempting to generate one...", m_exeName);
	}

	if ((status == NVAPI_END_ENUMERATION) && (!profileFound))
	{
		// Create a default profile
		if (!NvApi_LoadDefaultProfile(hSession))
			add_log("Could not generate profile for %s application! 3D Vision will be disabled...", m_exeName);
	}
	else if (status != NVAPI_OK)
	{
		PrintError(status);
		add_log("Could not generate profile for %s application! 3D Vision will be disabled...", m_exeName);
	}
	
	// (6) We clean up. This is analogous to doing a free()
	NvAPI_DRS_DestroySession(hSession);
	hSession = 0;
}
///--------------------------------------------------------------------------------------

// If we found a profile for the current .exe
static bool NvApi_IsProfileFound(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile)
{
	// (0) this function assumes that the hSession and hProfile are
	// valid handles obtained from nvapi.
	NvAPI_Status status;

	// (1) First, we retrieve generic profile information
	// The structure will provide us with name, number of applications
	// and number of settings for this profile.
	NVDRS_PROFILE profileInformation = { 0 };
	profileInformation.version = NVDRS_PROFILE_VER;
	status = NvAPI_DRS_GetProfileInfo(hSession, hProfile, &profileInformation);
	if (status != NVAPI_OK)
	{
		PrintError(status);
		return false;
	}
	// (2) Now we enumerate through all the applications on the profile,
	// if there is any
	if (profileInformation.numOfApps > 0) 
	{
		NVDRS_APPLICATION *appArray = new NVDRS_APPLICATION[profileInformation.numOfApps];
		NvU32 numAppsRead = profileInformation.numOfApps, i;
		bool profileFound = false;

		// (3) It is possible to enumerate all applications one by one,
		// or all at once on a preallocated array. The numAppsRead
		// represents the number of NVDRS_APPLICATION structures
		// allocated in the array. It will be modified on return of the
		// function contain the number of actual applications that have
		// been filled by NVAPI
		appArray[0].version = NVDRS_APPLICATION_VER;
		status = NvAPI_DRS_EnumApplications(hSession, hProfile, 0, &numAppsRead, appArray);
		if (status != NVAPI_OK)
		{
			PrintError(status);
			delete[] appArray;
			return false;
		}
		for (i = 0; i < numAppsRead; i++) 
		{
			wchar_t  appName[255];
			size_t origsize = strlen(m_exeName) + 1;
			const size_t newsize = 100;
			size_t convertedChars = 0;
			wchar_t unicodeExeName[newsize];
			mbstowcs_s(&convertedChars, unicodeExeName, origsize, m_exeName, _TRUNCATE);
			swprintf_s(appName, 255, L"%s", appArray[i].appName);

			// Found the profile
			if (wcscmp(appName, unicodeExeName) == 0)
			{
				profileFound = true;
				break;
			}
		}
		delete[] appArray;
		return profileFound;
	}
	return false;
}
///--------------------------------------------------------------------------------------

// Set the 3D Vision Profile Magic
static bool NvApi_Enable3DVisionSetting(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile)
{
	NvAPI_Status status;

	// (1) First, we retrieve generic profile information
	// The structure will provide us with name, number of applications
	// and number of settings for this profile.
	NVDRS_PROFILE profileInformation = { 0 };
	profileInformation.version = NVDRS_PROFILE_VER;
	status = NvAPI_DRS_GetProfileInfo(hSession, hProfile, &profileInformation);
	if (status != NVAPI_OK)
	{
		PrintError(status);
		return false;
	}

	// (4) Now we enumerate all the settings on the profile
	if (profileInformation.numOfSettings > 0)
	{
		NVDRS_SETTING *setArray = new NVDRS_SETTING[profileInformation.numOfSettings];
		NvU32 numSetRead = profileInformation.numOfSettings, i;

		// (5) The function to retrieve the settings in a profile works
		// like the function to retrieve the applications.
		setArray[0].version = NVDRS_SETTING_VER;
		status = NvAPI_DRS_EnumSettings(hSession, hProfile, 0, &numSetRead, setArray);
		if (status != NVAPI_OK)
		{
			PrintError(status);
			return false;
		}

		bool fixed = false;

		for (i = 0; i < numSetRead; i++)
		{
			if (setArray[i].settingLocation != NVDRS_CURRENT_PROFILE_LOCATION)
			{

				// (6) The settings that are not from the Current Profile
				// are inherited from the Base or Global profiles. Skip them.
				continue;
			}
			
			// Find the setting related to 3D Vision Window Mode support
			if (setArray[i].settingId == 0x701EB457)
			{
				// Setting found. Nothing to do as profile already exists.
				// cleanup
				delete(setArray);
				fixed = true;
				break;
			}
		}
		
		if (!fixed)
		{
			// Read the profile file.
			NvApi_ReadProfile();
			// Add the settings
			if (NvApi_ApplyProfileFromFile(hSession, hProfile))
			{
				add_log("Setting Successfully imported from Profile.nip");
			}

			// We didn't find our value so we need to set it.
			NVDRS_SETTING setNewSettings;
			
			// Update the values
			// Set the magic 3D Vision enable flag!
			setNewSettings.version = NVDRS_SETTING_VER;
			setNewSettings.settingId = NVAPI_STEREO_WINDOWED_3D_SUPPORT_ID;
			setNewSettings.settingType = NVDRS_DWORD_TYPE;
			setNewSettings.u32CurrentValue = 0x00000001;
			status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);

			// Set the default convergence
			setNewSettings.version = NVDRS_SETTING_VER;
			setNewSettings.settingId = NVAPI_STEREO_CONVERGENCE_ID;
			setNewSettings.settingType = NVDRS_DWORD_TYPE;
			setNewSettings.u32CurrentValue = g_reader->GetDefaultConvergence();
			status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);

			if (status == NVAPI_OK)
			{
				// Save settings to the driver
				status = NvAPI_DRS_SaveSettings(hSession);
				fixed = true;
			}
			// Delete the array
			delete(setArray);
		}

		return fixed;
	}
	return false;
}
///--------------------------------------------------------------------------------------

// Create Default 3D Vision Profile
static bool NvApi_LoadDefaultProfile(NvDRSSessionHandle hSession)
{
	NVDRS_PROFILE newProfileInfo = {0};
	NvDRSProfileHandle returnNewProfile;
	NvAPI_Status status;
	NVDRS_APPLICATION newApplication;

	newProfileInfo.version = NVDRS_PROFILE_VER;
	newProfileInfo.isPredefined = 0;

	// Generate the required Names
	_getProfileName(m_exeName, m_profileName);
	_getProfileName(m_exeName, m_friendlyName);
	
	// Set Profile Name
	NvApi_SetUnicodeString(newProfileInfo.profileName, m_profileName);
	
	newApplication.version = NVDRS_APPLICATION_VER;
	newApplication.isPredefined = 0;	

	// Set the names
	NvApi_SetUnicodeString(newApplication.appName, m_exeName);
	NvApi_SetUnicodeString(newApplication.userFriendlyName, m_friendlyName);
	NvApi_SetUnicodeString(newApplication.launcher, "");
	NvApi_SetUnicodeString(newApplication.fileInFolder, "");

	// Create the profile
	status  = NvAPI_DRS_CreateProfile(hSession, &newProfileInfo, &returnNewProfile);

	if (status == NVAPI_OK)
	{
		// Create the Application and added it to the profile
		status = NvAPI_DRS_CreateApplication(hSession, returnNewProfile, &newApplication);
		if (status == NVAPI_OK)
		{
			// Read the profile file.
			NvApi_ReadProfile();
			// Add the settings
			if (NvApi_ApplyProfileFromFile(hSession, returnNewProfile))
			{
				add_log("Setting Successfully imported from Profile.nip");
			}

			// Create our new setting
			NVDRS_SETTING setNewSettings;

			// Update the values
			// Set the magic 3D Vision enable flag!
			setNewSettings.version = NVDRS_SETTING_VER;
			setNewSettings.settingId = 0x701EB457;
			setNewSettings.settingType = NVDRS_DWORD_TYPE;
			setNewSettings.u32CurrentValue = 0x00000001;
			status = NvAPI_DRS_SetSetting(hSession, returnNewProfile, &setNewSettings);

			// Set the default convergence
			setNewSettings.version = NVDRS_SETTING_VER;
			setNewSettings.settingId = NVAPI_STEREO_CONVERGENCE_ID;
			setNewSettings.settingType = NVDRS_DWORD_TYPE;
			setNewSettings.u32CurrentValue = g_reader->GetDefaultConvergence();
			status = NvAPI_DRS_SetSetting(hSession, returnNewProfile, &setNewSettings);

			if (status == NVAPI_OK)
			{
				// Save settings to the driver
				status = NvAPI_DRS_SaveSettings(hSession);
				return true;
			}
			return false;
		}
		return false;
	}
	return false;
}
///--------------------------------------------------------------------------------------

// Convert from char to unicode chars used by NVAPI
static void NvApi_SetUnicodeString(NvAPI_UnicodeString& nvstr, const char * cstr)
{
	// convert to wchar_t
	size_t origsize = strlen(cstr) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	
	wchar_t unicodeName[newsize];
	mbstowcs_s(&convertedChars, unicodeName, origsize, cstr, _TRUNCATE);

	// convert to NvUnicode
	for (int i = 0; i < NVAPI_UNICODE_STRING_MAX; ++i) 
	{
		nvstr[i] = 0;
	}

	int i = 0;
	while (unicodeName[i] != 0)
	{
		nvstr[i] = unicodeName[i];
		i++;
	}
}
///--------------------------------------------------------------------------------------

// Generate the Profile Name
static void _getProfileName(const char* inExeName, char* outProfileName)
{
	const char* toFind = ".exe";
	const char *posFound = strstr(inExeName, toFind);
	int pos =  posFound- inExeName;
	char profileName[255] = "";
	if (pos != 0)
	{
		int len = strlen(inExeName) - (pos / sizeof(char))-1;
		strncat_s(profileName, inExeName, len);
		strncat_s(profileName, "-3D Vision Wrapper", 255);
		strcpy_s(outProfileName, 255, profileName);
	}
}
///--------------------------------------------------------------------------------------

// Get the exe name of the current application
static void _getApplicationName(char *exePath)
{
	TCHAR exepath[MAX_PATH + 1];
	// Get the current exe name
	if (GetModuleFileName(0, exepath, MAX_PATH + 1) != 0)
	{
		const char* toFind = "\\";
		const char* prevAddress = 0;
		const char *posFound = strstr(exepath, toFind);
		//exeName
		while (posFound)
		{
			// Remove the slash
			posFound++;
			prevAddress = posFound;
			posFound = strstr(posFound, toFind);
		}

		// Copy only the exe name
		strcpy_s(exePath, 255, prevAddress);

		// Convert to lower case as used by Nvidia Profiler
		for (; *exePath; ++exePath)
			*exePath = (char)tolower(*exePath);
	}
}
///--------------------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// Importing a profile from a file
//////////////////////////////////////////////////////////////////////////

typedef struct
{
	unsigned int settingId;
	unsigned int settingValue;
} NVAPI_SETTINGS_T;

static std::vector<NVAPI_SETTINGS_T> m_settingFromFile;

static std::string GetPath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
// -----------------------------------------------------------------------------------

// Read nip file
static void NvApi_ReadProfile(void)
{
	std::string profileFileName = GetPath() + "/Profile.nip";
	std::ifstream profileFile(profileFileName);
	std::string profileLine;
	size_t position;

	//file is opened
	if (profileFile.is_open())
	{
		// read one line at a time
		while (getline(profileFile, profileLine))
		{
			if (profileLine.find("<ProfileSetting>") != std::string::npos)
			{
				NVAPI_SETTINGS_T settings = {0};
				getline(profileFile, profileLine);

				if ((position = profileLine.find("<SettingID>")) != std::string::npos)
				{
					std::string newStr = profileLine.substr(position + strlen("<SettingID>"));

					// Read the id
					settings.settingId = std::stoi(newStr, 0);
				}
				getline(profileFile, profileLine);

				if ((position = profileLine.find("<SettingValue>")) != std::string::npos)
				{
					std::string newStr = profileLine.substr(position + +strlen("<SettingValue>"));

					// Read the value
					settings.settingValue = std::stoi(newStr, 0);;
				}

				// push the settings
				if (settings.settingId != 0 && settings.settingValue != 0)
				{
					m_settingFromFile.push_back(settings);
				}
			}
			getline(profileFile, profileLine);
			//----------------------------------------------
		}
		profileFile.close();
	}
}

// Apply values from the nip file
static bool NvApi_ApplyProfileFromFile(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile)
{
	NvAPI_Status status = NVAPI_OK;
	size_t settingSize = 0;
	
	// Did we read settings from file ?
	if (m_settingFromFile.size() != 0)
	{
		for (settingSize = 0; settingSize < m_settingFromFile.size(); settingSize++)
		{
			// If we can add all the settings
			if (status == NVAPI_OK)
			{
				NVDRS_SETTING setNewSettings;
				// Add the values from the profile
				setNewSettings.version = NVDRS_SETTING_VER;
				setNewSettings.settingId = m_settingFromFile[settingSize].settingId;
				setNewSettings.settingType = NVDRS_DWORD_TYPE;
				setNewSettings.u32CurrentValue = m_settingFromFile[settingSize].settingValue;
				status = NvAPI_DRS_SetSetting(hSession, hProfile, &setNewSettings);
			}
			else
				// something went wrong so abort
				break;
		}

		// Did we load all the settings?
		if ((status == NVAPI_OK) && (settingSize == m_settingFromFile.size()))
		{
			return true;
		}
		//else
		return false;
	}
	return false;
}