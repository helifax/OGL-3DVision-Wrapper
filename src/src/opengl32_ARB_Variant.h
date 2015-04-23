
#ifndef H_OPENGL32_ARB_EXTENSION__
#define H_OPENGL32_ARB_EXTENSION__

extern "C" {
#include "opengl_3dv.h"
}
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include "opengl32.h"
#include "include\ShaderManager.h"
#include "windows.h"
#include "d3d9.h"
#include "NVAPI_343\nvapi.h"
#include <thread>
#include "FPSInject/FPSInject.h"
#include "MessageBox\MessageBox.h"
#include "ConfigReader.h"
void sys_glUseProgramObjectARB(GLhandleARB program);
#endif