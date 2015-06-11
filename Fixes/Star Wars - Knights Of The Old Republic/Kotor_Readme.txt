
==============================================================================
Star Wars: Knights Of The Old Republic - 3D Vision Patch. - v.2.20 Beta
==============================================================================
A lot of effort went into creating this fix: a profile so people can take advantage of SLI, 3D Vision Surround and other OGL optimizations.



===================================================
Install & Important information regarding the fix:
===================================================
- Added Full SLI & Surround Support. (By default the game has a big problem in SLI mode and renders incorrectly). The wrapper fixes this.
- Added support for resolution changing.
- Playing Videos is now supported* (See Known Issues)


- Set all the Video Settings in the game before installing the wrapper:
(Modify the existing values if they don't exist add them !)
	a. Disable "Soft Shadows" as it makes certain textures black.


- Copy all the files next to the game .exe file.
- Use Middle Mouse Button to swap to a good converge point for the menus/cutscenes. It can be modified via INI file.


To Uninstall:
- Run "Uninstall.bat" file



==========================
Fixed:
=========================
- Everything



=========================
Supports:
=========================
- Window mode Mode and Fullscreen.
- Supports convergence, separation and depth adjustments from nvidia panel (or via hotkeys).
- Tested on Single GPU/ SLI 3D Vision setups
- Tested on 3D Vision Surround setups
- Works with Flawless Widescreen Fix.


=========================
Known Issues:
=========================
- Soft Shadows option corrupts the textures.
- Movies:
  - Because the nvidia driver and thus the wrapper works differently in SLI(Surround/Single) vs Single GPU behaviour is different:
	- SLI: When a movie start the screen will be black. Simply ALT+TAB to Desktop and back to game to see the movies.
	- Single GPU: Movies will be shown on screen. Also the driver will display the infamous nvidia RED Message of "Unsupported 3D Vision Mode". Simply discard it.
	- Single GPU: After a movie finishes. It is possible the game to be rendered only in one eye. Simply ALT+TAB or restart the game to fix this as the driver is not working correctly anymore...
  - It is possible when the game changes resolutions/modes that the nvidia driver will crash, which is beyond my control.



=========================
Creator:
=========================
- Helifax 2015.

If you like this software and would like to contact me or donate, please use: tavyhome@gmail.com



