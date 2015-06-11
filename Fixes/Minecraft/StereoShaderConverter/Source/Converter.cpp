#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <string>
#include "windows.h"

static std::vector<std::string> m_filenames;
static bool infoScreen = false;
static bool convertScreen = false;
static bool m_screenlock = false;

static std::string GetPath();
static void initFilenames();
static void printInstructions();
static void doConversion();
static void showMenu();
static void keyHandler();

///-------------------------------------------------------------------------------------------

void __cdecl add_log(const char * fmt, ...)
{
	va_list va_alist;
	char logbuf[5000] = "";
	FILE * fp;
	errno_t err;

	va_start(va_alist, fmt);
	_vsnprintf_s(logbuf + strlen(logbuf), 5000, sizeof(logbuf)-strlen(logbuf), fmt, va_alist);
	va_end(va_alist);

	err = fopen_s(&fp, "StereoConverter.log", "ab");
	if (err == 0)
	{
		fprintf(fp, "%s", logbuf);
		fprintf(fp, "\n");
		fclose(fp);
	}
}
///-------------------------------------------------------------------------------------------


static void initFilenames()
{
	m_filenames.push_back("\\shaders\\gbuffers_weather.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_water.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_textured_lit.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_textured.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_terrain.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_skytextured.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_skybasic.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_hand.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_entities.vsh");
	m_filenames.push_back("\\shaders\\gbuffers_basic.vsh");

	m_filenames.push_back("\\shaders\\composite.fsh");
	m_filenames.push_back("\\shaders\\composite1.fsh");
}
///-------------------------------------------------------------------------------------------

static void doConversion()
{
	m_screenlock = true;
	convertScreen = true;
	system("CLS");
	remove("StereoConverter.log");

	std::cout << "MineCraft SEUS Shader Pack 3D Vision Converter\n";
	std::cout << "All right reserved. Helifax 2014.\n\n";
	std::cout << "Contributors:\n" << "Helifax - Creator of this tool and OGL3DVision Wrapper\n" << "Mike_ar69 - For fixing these awesome shaders\n" << "Sonic Ether - For creating the shader pack.\n";
	std::cout << "\n\n";
	std::cout << "Before starting be sure you have extracted the \"shaders\" folder from SEUS Pack and is located in the same folder as this tool.\n\n";
	std::cout << "Press any key to start the conversion.\n";
	system("pause");

	for (size_t i = 0; i < m_filenames.size(); i++)
	{
		// Get the next file
		std::string shaderFileName = GetPath() + m_filenames[i];

		// open the file
		std::ifstream shaderFile(shaderFileName);
		std::string lineRead;

		if (shaderFile.is_open())
		{
			std::string newFileContent;
			bool uniformDone = false;
			
			// check to see if converted
			getline(shaderFile, lineRead);
			if (lineRead.find("//Shaders Converted to 3D Vision by Helifax & Mike_ar69") != std::string::npos)
			{
				std::cout << "Shader already converted to 3D Vision Stereo 3D ! Skipping\n";
			}
			else
			{
				newFileContent.append("//Shaders Converted to 3D Vision by Helifax & Mike_ar69\n");
				// set position to begining of file again
				shaderFile.seekg(0, shaderFile.beg);

				while (getline(shaderFile, lineRead))
				{
					// UNIFORM ADDITION
					{
						if ((lineRead.find("#version") != std::string::npos) && (!uniformDone))
						{
							// Do the shader gl_Position insertion
							lineRead.append("\nuniform float g_depth;\nuniform float g_eye;\nuniform float g_eye_separation;\nuniform float g_convergence;\n");

							// add the line to the normal content buffer
							newFileContent.append(lineRead);
							newFileContent.append("\n");
							uniformDone = true;
						}

						// Stereo Injection
						else if (lineRead.find("gl_Position =") != std::string::npos)
						{
							// Do the shader gl_Position insertion
							lineRead.append("\ngl_Position.x += g_eye * g_eye_separation * (gl_Position.w - g_convergence);\n");

							// add the line to the normal content buffer
							newFileContent.append(lineRead);
							newFileContent.append("\n");
						}
						// composite.fsh correction
						else if ((lineRead.find("vec4 worldposition = vec4(0.0f);") != std::string::npos) && (m_filenames[i] == "\\shaders\\composite.fsh"))
						{
							lineRead.append("\n//===========================================================\n");
							lineRead.append("//Mike_ar69 - Stereo Correction\n");
							lineRead.append("vec4 temp = surface.screenSpacePosition;\n");
							lineRead.append("temp.x -= g_eye * g_eye_separation * (-temp.z - g_convergence)*gbufferProjectionInverse[0].x;\n");
							lineRead.append("surface.screenSpacePosition = temp;\n");
							lineRead.append("//===========================================================\n");

							// add the line to the normal content buffer
							newFileContent.append(lineRead);
							newFileContent.append("\n");
						}

						// composite1.fsh correction
						//////////////////////////////////////////////////////////////////////////
						else if ((lineRead.find("vec3 currentPosition = convertCameraSpaceToScreenSpace(cameraSpaceVectorPosition);") != std::string::npos) && (m_filenames[i] == "\\shaders\\composite1.fsh"))
						{
							lineRead.append("\n//===========================================================\n");
							lineRead.append("//Mike_ar69 - Stereo Correction\n");
							lineRead.append("currentPosition.x += g_eye * g_eye_separation * (-currentPosition.z - g_convergence)*0.05; \n");
							lineRead.append("//===========================================================\n");

							// add the line to the normal content buffer
							newFileContent.append(lineRead);
							newFileContent.append("\n");
						}
						else if ((lineRead.find("currentPosition = convertCameraSpaceToScreenSpace(cameraSpaceVectorPosition);") != std::string::npos) && (m_filenames[i] == "\\shaders\\composite1.fsh"))
						{
							lineRead.append("\n//===========================================================\n");
							lineRead.append("//Mike_ar69 - Stereo Correction\n");
							lineRead.append("currentPosition.x += g_eye * g_eye_separation * (-currentPosition.z - g_convergence)*0.05;\n");
							lineRead.append("//===========================================================\n");

							// add the line to the normal content buffer
							newFileContent.append(lineRead);
							newFileContent.append("\n");
						}
						else if ((lineRead.find("void CloudPlane(inout SurfaceStruct surface)") != std::string::npos) && (m_filenames[i] == "\\shaders\\composite1.fsh"))
						{
							std::string currentLine = lineRead;
							getline(shaderFile, lineRead); //{
							currentLine.append(lineRead);
							getline(shaderFile, lineRead); //Initialize view array
							currentLine.append(lineRead);

							currentLine.append("\n//===========================================================\n");
							currentLine.append("//Mike_ar69 - Fix sky height\n");
							currentLine.append("vec4 VSP = 	-GetViewSpacePosition(texcoord.st);\n");
							currentLine.append("VSP.x -= g_eye * g_eye_separation * (-VSP.z - g_convergence)*gbufferProjectionInverse[0].x;\n");
							currentLine.append("vec4 worldVector = gbufferModelViewInverse * VSP; // (-GetViewSpacePosition(texcoord.st));\n");
							currentLine.append("//===========================================================\n");

							// add the line to the normal content buffer
							newFileContent.append(currentLine);
							newFileContent.append("\n");
						}
						else if ((lineRead.find("vec4 worldVector = gbufferModelViewInverse * (-GetViewSpacePosition(texcoord.st));") != std::string::npos) && (m_filenames[i] == "\\shaders\\composite1.fsh"))
						{
							// DO NOThING.
							// This line must be removed
						}
						else if ((lineRead.find("surface.viewSpacePosition = GetViewSpacePosition(texcoord.st);") != std::string::npos) && (m_filenames[i] == "\\shaders\\composite1.fsh"))
						{
							lineRead.append("\n//===========================================================\n");
							lineRead.append("//Mike_ar69 - Fix cloud reflection on water\n");
							lineRead.append("surface.viewSpacePosition.x -= g_eye * g_eye_separation * (-surface.viewSpacePosition.z - g_convergence)*gbufferProjectionInverse[0].x;\n");
							lineRead.append("//===========================================================\n");

							// add the line to the normal content buffer
							newFileContent.append(lineRead);
							newFileContent.append("\n");
						}


						else // another line just save it
						{
							newFileContent.append(lineRead);
							newFileContent.append("\n");
						}
					}
				}
				// we reached the end of the file so we need to store the new content in the file
				// Close the file to read
				shaderFile.close();
				// Remove the old file
				remove(shaderFileName.c_str());
				// Open the same file as write
				std::ofstream shaderFile(shaderFileName);
				// Save the new content
				shaderFile << newFileContent;
				// close the file
				shaderFile.close();

				char string[255];
				sprintf_s(string, "File %s converted to Stereo 3D!\n", m_filenames[i].c_str());
				add_log(string);

				std::cout << "File " << m_filenames[i] << " converted to Stereo 3D!\n";
			}
		}
		else
		{
			char string[255];
			sprintf_s(string, "Filename %s not found!\n", m_filenames[i].c_str());
			add_log(string);
			std::cout << "Filename " << m_filenames[i] << " not found!\n";
		}
	}
	std::cout << "Options:\n";
	std::cout << "1. Go back to first menu.\n";
	std::cout << "3. Quit.\n";
}
///-------------------------------------------------------------------------------------------

static void showMenu()
{
	system("CLS");
	std::cout << "MineCraft SEUS Shader Pack 3D Vision Converter\n";
	std::cout << "All right reserved. Helifax 2014.\n\n";
	std::cout << "Contributors:\n" << "Helifax - Creator of this tool and OGL3DVision Wrapper\n" << "Mike_ar69 - For fixing these awesome shaders\n" << "Sonic Ether - For creating the shader pack.\n";
	std::cout << "\n\n";
	std::cout << "1. Read Instructions.\n";
	std::cout << "2. Convert SEUS Shader Pack.\n";
	std::cout << "3. Quit.\n";
	m_screenlock = false;
}
///-------------------------------------------------------------------------------------------

static void keyHandler()
{
	char key;
	do
	{
		key = getchar();
		switch (key)
		{
		case 49:
			if ((!infoScreen) && (!convertScreen))
			{
				printInstructions();
			}
			else
			{
				convertScreen = false;
				infoScreen = false;
				showMenu();
			}
			break;

		case 50:
			if (!m_screenlock)
			{
				doConversion();
			}
			break;

		default:
			break;
		}

	} while ((key != 51));
}
///-------------------------------------------------------------------------------------------

static void printInstructions()
{
	m_screenlock = true;
	infoScreen = true;
	system("CLS");
	std::cout << "MineCraft SEUS Shader Pack 3D Vision Converter\n";
	std::cout << "All right reserved. Helifax 2014.\n\n";
	std::cout << "Contributors:\n" << "Helifax - Creator of this tool and OGL3DVision Wrapper\n" << "Mike_ar69 - For fixing these awesome shaders\n" << "Sonic Ether - For creating the shader pack.\n";
	std::cout << "\n\n";
	std::cout << "Folow these steps to convert the SEUS Shader Pack to 3D Vision Compatible Stereo 3D Shaders:\n\n";
	std::cout << "1. Download the \"SEUS Shader Pack\" from: \n\"http://www.minecraftforum.net/forums/mapping-and-modding/minecraft-mods/1280299-sonic-ethers-unbelievable-shaders-v10-1\"\n";
	std::cout << "* For Best performance I recommend the Normal Version (not the ULTRA)\n";
	std::cout << "2. Open the .zip archive file and extract(copy) the \"shaders\" folder.\n";
	std::cout << "3. Put the extracted shaders folder NEXT to this application (.exe file)\n";
	std::cout << "4. Run this tool and select the Number 2 Option\n";
	std::cout << "5. Once converted create a ZIP archive containing the \"shaders\" folder.\n";
	std::cout << "* Be sure the archive contains the \"shaders\" folder as well NOT JUST the files from the \"shaders\" FOLDER !!!\n";
	std::cout << "6. Copy the ZIP archive to \"%APPDATA%\\.minecraft\\shaderpacks\" folder. \n";
	std::cout << "7. Run the game and from the menu select the new ShaderPack zip file to use.\n";
	std::cout << "8. Enjoy !!!\n\n";

	std::cout << "Options:\n";
	std::cout << "1. Go back to first menu.\n";
	std::cout << "3. Quit.\n";
}
///-------------------------------------------------------------------------------------------

void main()
{
	initFilenames();
	showMenu();
	keyHandler();
}


static std::string GetPath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
///-------------------------------------------------------------------------------------------