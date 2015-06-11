
===========================================================
Wolfenstein: The Old BLood - 3D Vision Patch. - v.3.19 Beta
===========================================================
Updated to use OGL3DVision Wrapper v.3.19 Beta
Updated SLI flags so it works in SLI Mode
A lot of effort went into creating this fix: a profile so people can take advantage of SLI, 3D Vision Surround and other OGL optimizations.



==================================================
Install & Important information regarding the fix:
==================================================

READ BEFORE USE:
ID5 Tech Engine is locked at 60fps. The driver/wrapper displays consecutive frames altered for stereoscopic projection. As a result the framerate will drop to 30fps (60/2eyes).
This might induce motion sickness to some. Is not universal and doesn't affect everyone to the same degree.
If you cannot tolerate this, DON'T USE THIS FIX or Uninstall it!!!!


- Make sure the game runs in FULLSCREEN Mode.
- Copy all the files (from the 3DVision folder) next to the game .exe file.
- RUN THE GAME AS ADMINISTRATOR!!! Or else the profile is not created properly and 3D Vision doesn't kick in.
- Disable SPACE SCREEN REFLECTIONS - Not rendering correctly in 3D
- It is possible the game/wrapper to crash if you use CUDA acceleration option in-game. Disable it to avoid this.
- Game doesn't support SLI (you get the same performance in SLI or Non-SLI).
- DON'T PUT EVERYTHING TO ULTRA or you will get very low FPS. On my 2x780Ti I use High preset (in 3D Surround). I recommend Medium/High settings as a base.
- Fix is compatible with Flawless Widescreen (for 3D Surround).

RUNS BEST ON SLI CONFIGURATIONS, due to the fact that I am able to set DX9 context to run in fullscreen! (On single GPU I need to create the DX9 context as Windowed. This is a BUG in nvidia OpenGL-Directx Interoperability layer!!!)
Runs well on Single GPU configurations but I REALLY RECOMMEND SLI !!!!

- Use "Q" key to toggle to a convergence mode when AIMING. Press "Q" to toggle back! (Can be changed from the Ini file. Look for aleternative convergence).
- Profile comes with a defined convergence setting for best stereoscopic effect. You can alter it however, using the normal 3D Vision shortcut Keys.

To Uninstall:
- Run "Uninstall.bat" file



==========================
Fixed:
=========================
- Shadows.
- Lights. (Some lights might render in one eye at certain angles)
- Blood and Decals (like gun-shot holes).
- Subsurface scattering.
- Reflections.
- Specularity.
- HUD + Videos.
- In "3DVisisionWrapper.ini" you can set another convergence setting for "AIM", bind it to a keyboard/mouse button 
and set it as a toggle or "hold".
- As, a result I rated the game as rendering "3D Vision Ready"(as 95% of the times is rendering flawless).



=========================
Supports:
=========================
- Both Window mode and fullscreen.
- Supports convergence, separation and depth adjustments from nvidia panel (or via hotkeys).
- Tested on Single GPU/ SLI 3D Vision setups.
- Tested on 3D Vision Surround setups.
- Didn't test 3DTVPlay, but technically it should work.



=========================
Known Issues:
=========================
- The EYES are NOT completely in sync (explained above).
- Nothing else that I am aware of. If you find something (and not covered in this file) please let me know !


=========================
Creator:
=========================
Helifax. 2015.
If you like this software and would like to contact me or donate, please use: tavyhome@gmail.com