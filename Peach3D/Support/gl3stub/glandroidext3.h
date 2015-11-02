#ifndef __glandroidext3_h_
#define __glandroidext3_h_

#include <GLES3/gl3.h>

#ifdef __cplusplus
extern "C" {
#endif

// defined map buffer func for GLES 2.0 running and GLES 3.0 compile
extern GL_APICALL void* GL_APIENTRY glMapBufferOES (GLenum target, GLenum access);
extern GL_APICALL GLboolean GL_APIENTRY glUnmapBufferOES (GLenum target);
#define GL_WRITE_ONLY_OES                                       0x88B9

/* GL_EXT_texture_filter_anisotropic */
#define GL_TEXTURE_MAX_ANISOTROPY_EXT                           0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT                       0x84FF

#ifdef __cplusplus
}
#endif

#endif
