#ifndef __glandroidext2_h_
#define __glandroidext2_h_

#include <GLES2/gl2.h>
#include <GLES2/gl2platform.h>
#include <GLES2/gl2ext.h>

#ifdef __cplusplus
extern "C" {
#endif

// defined map buffer func for GLES 2.0 running and GLES 3.0 compile
extern PFNGLMAPBUFFEROESPROC glMapBufferOES;
extern PFNGLUNMAPBUFFEROESPROC glUnmapBufferOES;

#ifdef __cplusplus
}
#endif

#endif
