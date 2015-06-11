/*
* OpenGL - 3D Vision Wrapper
* Copyright (c) Helifax 2015
*/

#ifndef __OGL3DVISION_VERSION_H__
#define __OGL3DVISION_VERSION_H__

/// Major version
#define OGL3DVISION_VERSION_MAJOR          3

/// Minor version
#define OGL3DVISION_VERSION_MINOR          25

/// Version patch
#define OGL3DVISION_VERSION_PATCH          0

/// Beta Enabled
#define OGL3DVISION_VERSION_IS_BETA        1

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#if (OGL3DVISION_VERSION_PATCH == 0)

/// SDKC full version number (multiplied by 100 eg. 204 = version 2.04)
#define OGL3DVISION_VERSION				((OGL3DVISION_VERSION_MAJOR * 100) + OGL3DVISION_VERSION_MINOR)

// Is Beta Enabled ?
/// Full version number as a string
#if (OGL3DVISION_VERSION_IS_BETA == 1)
#define OGL3DVISION_VERSION_STR  STRINGIZE(OGL3DVISION_VERSION_MAJOR) \
"." STRINGIZE(OGL3DVISION_VERSION_MINOR) " Beta"
#else
#define OGL3DVISION_VERSION_STR  STRINGIZE(OGL3DVISION_VERSION_MAJOR) \
"." STRINGIZE(OGL3DVISION_VERSION_MINOR)
#endif

/// Patched version
#else

/// Full version number including patch (Major is byte 2. Minor is byte 1 and Patch version is byte 0)
#define OGL3DVISION_VERSION			((OGL3DVISION_VERSION_MAJOR << 16 ) | (OGL3DVISION_VERSION_MINOR << 8) | OGL3DVISION_VERSION_PATCH) 

// Is Beta Enabled ?
/// SDKC full version number as a string
#if (OGL3DVISION_VERSION_IS_BETA == 1)
#define OGL3DVISION_VERSION_STR  STRINGIZE(OGL3DVISION_VERSION_MAJOR) \
"." STRINGIZE(OGL3DVISION_VERSION_MINOR) "." STRINGIZE(OGL3DVISION_VERSION_PATCH) " Beta"
#else
#define OGL3DVISION_VERSION_STR  STRINGIZE(OGL3DVISION_VERSION_MAJOR) \
"." STRINGIZE(OGL3DVISION_VERSION_MINOR) "." STRINGIZE(OGL3DVISION_VERSION_PATCH)
#endif

#endif

#endif
