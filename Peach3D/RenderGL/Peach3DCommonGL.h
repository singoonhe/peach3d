//
//  Peach3DCommonGL.h
//  Peach3DLib
//
//  Created by singoon.he on 9/1/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PEACH3DCOMMONGL_h
#define PEACH3D_PEACH3DCOMMONGL_h

#include "Peach3DCompile.h"

#if (PEACH3D_CURRENT_PLATFORM==PEACH3D_PLATFORM_IOS)
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/ES3/glext.h>
#elif (PEACH3D_CURRENT_PLATFORM==PEACH3D_PLATFORM_ANDROID)
#ifdef ANDROID_DYNAMIC_ES3
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "gl3stub/gl3stub.h"
#include "gl3stub/glandroidext2.h"
#else
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "gl3stub/glandroidext3.h"
#endif
#elif (PEACH3D_CURRENT_PLATFORM==PEACH3D_PLATFORM_MAC)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif

// check opengl extersion support
#define PD_GLEXT_SUPPORT(ext) ((RenderGL*)IRender::getSingletonPtr())->isTypeExtersionSupport(ext)
// check opengl "VertexArray" extersion support
#define PD_GLEXT_VERTEXARRAY_SUPPORT() (PD_RENDERLEVEL_GL3() || PD_GLEXT_SUPPORT(GLExtensionType::eVertexArray))

#endif
