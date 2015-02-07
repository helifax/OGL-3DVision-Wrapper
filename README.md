# OGL-3DVision-Wrapper
OpenGL Wrapper to enable 3D Vision Support under Windows


-------------------
NEW IN VERSION 1.50
-------------------

New Features:
- Automatic Hooking Mode.
- Alternative Buffer Flipping Mode.
- Partial implementation of loading ARB Object Shaders (required by Minecraft). ShaderSource, Compile & Link to be done.

Updates:
- Updated the Shader Source Hooking algorithm as sometimes the shader source code was incomplete.
- Fixed some crashes when using the Debug Info option (for intercepting draw calls).
- Implemented Window Resize (still prototype phase as it crashes in multiple games).

-------------------
FEATURE LIST
-------------------

Ability to log the wrapper functions.

[3D Vision Specific]
- Ability to toggle 3D Vision Support ON/OFF (so you don't need to delete the wrapper files)
- Ability to trace some of the OpenGL draw functions (Hooking points for the wrapper)
- Ability to specify which hooking point to use

[3D Vision Automatic Enabling]
- Ability to enable or disable the Stereoscopic Shader Injection (equivalent of 3D Vision Automatic)
- Ability to specify up to 3 distinct Shader Stereoscopy Injection points and 3 distinct Shader Uniform Injection points
- Ability to specify the Stereoscopic Injection function.

[Shader Compiler]
- Automatic detection of Shader Compilation & Linking errors + full debug information.
- Ability to select a pair of shaders between which the Stereocopy will not be applied (for hunting shaders down)
- Ability to Change & Compile + Link a shader In real-time (so you can immediately see the difference)
- Ability to dump all shaders as soon as they are created.
- Ability to load own shaders during shader creation stage.
Shader Support: Only GLSL is currently supported. No ARB Shaders support. (Please contact me if you want to help in this section)
[Alternative 3D Vision Settings]
- Ability to query the current separation (depth) and convergence values that are being applied
- Ability to bind a different convergence value to a keyboard key/mouse button (hold to work/ toggle mode)

[FPS Frame cap Remover Support]
- Ability to write your own plugin that removes the FPS lock in a game.

[Wrapper Versions]
- A Development version for creating a FIX.
- A Public Release version for using a FIX.

Supported Platforms: 
- Windows 7(and above) x86(32 bits) and x64(64 bits).
HOT-KEYS USED FOR DEVELOPEMENT

Keyboard Key :
Functionality :
[F8]	Shader Compiler: Toggle between Vertex and Pixel Shader for the current selected Shader Program
[F9]	Shader Compiler: Compile & Link the Current Vertex or Pixel Shader for the current selected Shader Program
[F10]	Show Current 3D Vision Settings: Separation/Depth and Convergence values that are being applied
[CTRL] + [+]	Shader Compiler: Select the Next Shader Program
[CTRL] + [-]	Shader Compiler: Select the Previous Shader Program

-------------------
For latest BUILD and DOCUMENTATION go to: http://3dsurroundgaming.com/OGL3DVision.html
-------------------


-------------------
BUILDING THE LIBRARY
-------------------

Prerequisites:
- Microsoft 
- Visual Studio 2013
- NVAPI libraries.
- DirectX Software Development Kit
- For x64 build it requires "Intel C++ Compiler" (as Ms decided NOT to support emmbeded assembly into C/C++ code in x64)

