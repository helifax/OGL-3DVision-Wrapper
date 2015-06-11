
=========================================================
Minecraft - v.1.7.xx - 3D Vision Patch. - v.2.20 Beta
=========================================================
Updated to use OGL3DVision Wrapper v.2.20 Beta
A lot of effort went into creating this fix: a profile so people can take advantage of SLI, 3D Vision Surround and other OGL optimizations.

- This patch requires Java x64 to be installed on the PC. (Default location where it is installed is: "C:\Program Files\Java\jre7\bin" or "Minecraft\runtime\jre\version\bin")



===================================================
Install & Important information regarding the fix:
===================================================
- Added Support for Legacy OpenGL.
- If you want to use your Custom Mods:
	a. See this POST: https://forums.geforce.com/default/topic/769009/3d-vision/minecraft-v-1-7-10-in-3d-vision-/1/
	b. Follow the install instructions from there.
	c.Install the "Mike Shaders.zip" shaders in the Shader Folder of your Minecraft folder (See above post for more info).


- Copy all the files next to the game .exe file.
- To change the HUD Depth In 3DVisionWrapper.ini file change:

[Legacy_OpenGL_Calls]
LegacyHUDSeparation = 0.2

To Uninstall:
- Use "Uninstall.bat" to remove the wrapper.



==========================
Fixed:
=========================
- Everything



=========================
Supports:
=========================
- Window mode and Fullscreen.
- Supports convergence, separation and depth adjustments from nvidia panel (or via hotkeys).
- Tested on Single GPU/ SLI 3D Vision setups
- Tested on 3D Vision Surround setups
- Reported to WORK with 3DTVPlay



=========================
Known Issues:
=========================
- None so far



=========================
Creators:
=========================

- Helifax - OpenGL - 3D Vision Wrapper and initial shader stereoscopy injection
- Mike_ar69 - Fixed alot of shaders effects : Shadows, Reflections, Sky etc (in SEUS shaders) - different release.
- Mike_ar69 - Created the Vanilla Minecraft Stereoscopic shaders (found in this release).

If you like this software and would like to contact me or donate, please use: tavyhome@gmail.com



