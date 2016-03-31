//
//  CompileConfig.h
//  TestPeach3D
//
//  Created by singoon he on 12-4-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_COMPILECONFIG_H
#define PEACH3D_COMPILECONFIG_H

#include <iostream>
#include <map>
#include <math.h>
#include <time.h>
#include <float.h>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <memory>
#include <string>
#include <assert.h>
// Peach3D current version
#define PEACH3D_SDK_VERSION "0.1rc2"

///////////////////////////////////////////////////////
//////////////////////   Platform   ///////////////////
///////////////////////////////////////////////////////
// define all support platform
#define PEACH3D_PLATFORM_UNKNOW     0
#define PEACH3D_PLATFORM_IOS        1
#define PEACH3D_PLATFORM_ANDROID    2
#define PEACH3D_PLATFORM_MAC        3
#define PEACH3D_PLATFORM_WINUWP     4
// define support render
#define PEACH3D_RENDER_UNKNOW       0
#define PEACH3D_RENDER_GL           1
#define PEACH3D_RENDER_GLES         2
#define PEACH3D_RENDER_DX           3

// define current platform 
#define PEACH3D_CURRENT_PLATFORM PEACH3D_PLATFORM_UNKNOW
// define current Render
#define PEACH3D_CURRENT_RENDER   PEACH3D_RENDER_UNKNOW

// MAXOSX
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) || defined(MAC_OS_X_VERSION_MIN_REQUIRED)
    #undef PEACH3D_CURRENT_PLATFORM
    #define PEACH3D_CURRENT_PLATFORM PEACH3D_PLATFORM_MAC
    // define render
    #undef PEACH3D_CURRENT_RENDER
    #define PEACH3D_CURRENT_RENDER   PEACH3D_RENDER_GL
#endif

// IOS
#if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
    #undef PEACH3D_CURRENT_PLATFORM
    #define PEACH3D_CURRENT_PLATFORM PEACH3D_PLATFORM_IOS
    // define render
    #undef PEACH3D_CURRENT_RENDER
    #define PEACH3D_CURRENT_RENDER   PEACH3D_RENDER_GLES
#endif

// android
#if defined(ANDROID)
    #undef PEACH3D_CURRENT_PLATFORM
    #define PEACH3D_CURRENT_PLATFORM PEACH3D_PLATFORM_ANDROID
    // define render
    #undef PEACH3D_CURRENT_RENDER
    #define PEACH3D_CURRENT_RENDER   PEACH3D_RENDER_GLES
#endif

#if defined(_WIN32) && WINAPI_FAMILY==WINAPI_FAMILY_APP
	// windows uwp
    #undef PEACH3D_CURRENT_PLATFORM
    #define PEACH3D_CURRENT_PLATFORM PEACH3D_PLATFORM_WINUWP
    // define render
    #undef PEACH3D_CURRENT_RENDER
    #define PEACH3D_CURRENT_RENDER   PEACH3D_RENDER_DX
#endif

// debug
#if defined(DEBUG) || defined(_DEBUG)
    #define PEACH3D_DEBUG 1
#else
    #define PEACH3D_DEBUG 0
#endif

// not show some insignificant warning on WinUwp
#if (PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX)
    #pragma warning ( disable : 4996 )  // disable c function not safe waring
    #pragma warning ( disable : 4251 )  // disable stl waring
    // DLL library
    #if defined(_WINDLL)
        #define PEACH3D_DLL  __declspec(dllexport)
    #else
        #define PEACH3D_DLL  __declspec(dllimport)
    #endif
#else
    #define PEACH3D_DLL
#endif

// custom assert for feature
#define Peach3DAssert(cmd, desc) assert((cmd) && desc)
// set buffer offset
#define PEACH3D_BUFFER_OFFSET(i) ((char *)NULL + (i))
// max texture count object supported
#define PEACH3D_OBJECT_TEXTURE_COUNT_MAX   4
// max texture count widget supported
#define PEACH3D_WIDGET_TEXTURE_COUNT_MAX   2

///////////////////////some constant////////////////////////////////
#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
#endif // FLT_EPSILON
#ifndef FLT_MAX
#define FLT_MAX         1E+37
#endif // FLT_EPSILON
#define PD_PI           3.1415926536f
#define PD_PIOVER180    0.017453f       // PD_PI/180
#define PD_PIUNDER180   57.295779f      // 180/PD_PI
#define PD_UINT_MAX     ((uint)-1)
#define STRINGIFY(A)    #A

#define RADIANS_TO_DEGREE(radians)  ((radians)*PD_PIUNDER180)
#define DEGREE_TO_RADIANS(degree) ((degree)*PD_PIOVER180)
#define FLOAT_EQUAL(A,B)     (((A)+FLT_EPSILON >= (B)) && ((A)-FLT_EPSILON <= (B)))
#define FLOAT_EQUAL_0(A)     ((A)<=FLT_EPSILON && (A)>=(-FLT_EPSILON))
#define CLAMP(VALUE,MIN,MAX) { VALUE=(VALUE<(MIN))?(MIN):VALUE; VALUE=(VALUE>(MAX))?(MAX):VALUE; }

/////////////////if break macros//////////////////
#define IF_BREAK(condition, log, ...) \
    if (condition) { \
        if (log) { \
            Peach3DErrorLog(log, ##__VA_ARGS__); \
        } \
        break; \
    }

/////////////////typedef base data type//////////////////
typedef unsigned int  uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned long long ulong64;

#endif // PEACH3D_COMPILECONFIG_H
