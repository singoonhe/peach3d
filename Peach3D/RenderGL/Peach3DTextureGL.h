//
//  Peach3DTextureGL.h
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_TEXTUREGL_H
#define PEACH3D_TEXTUREGL_H

#include "Peach3DCommonGL.h"
#include "Peach3DITexture.h"

namespace Peach3D
{
    struct TextureGLFormatInfo
    {
        TextureGLFormatInfo() {}
        TextureGLFormatInfo(GLint _format, GLenum _byte) : glFormat(_format), glByte(_byte) {}
        GLint   glFormat;   // gl format, GL_RGB/GL_RGBA...
        GLenum  glByte;     // gl bits per pixel, GL_UNSIGNED_BYTE/GL_UNSIGNED_SHORT_5_6_5...
    };
    
    class TextureGL : public ITexture
    {
    public:
        // constructor and destructor must be public, because shared_ptr need call them 
        TextureGL(const char* name) : ITexture(name), mTextureId(0), mFrameBuffer(0), mIsDepthFrame(false) {}
        ~TextureGL();
        
        /** Set texture data and init texture, format and size must set before. */
        virtual bool setTextureData(void* data, uint size, TextureDataStatus status = TextureDataStatus::eDecoded);
        /** Set texture data and init texture for cube texture, format and size must set before. */
        virtual bool setTextureData(void* dataList[6], uint sizeList[6], TextureDataStatus status = TextureDataStatus::eDecoded);
        /** Create RTT texture(render to texture).
         * @params isDepth if true, draw depth data to texture.
         */
        virtual bool usingAsRenderTexture(int width, int height, bool isDepth = false);
        
        /* Set texture filter. */
        virtual void setFilter(TextureFilter filter);
        /* Set texture wrap. */
        virtual void setWrap(TextureWrap wrap);
        /* Change GL state before rendering to texture. */
        virtual void beforeRendering();
        /* Restore GL state after rendering to texture. */
        virtual void afterRendering();
        GLuint getGLTextureId() { return mTextureId; }
        
    protected:
        //! fill GL the texture format map
        static void fillGLTextureFormatMap();
        
        //! void set texture filter, ignore bind texture
        void setFilterNoBind(TextureFilter filter);
        //! void set texture wrap, ignore bind texture
        void setWrapNoBind(TextureWrap wrap);
        
    private:
        GLuint      mTextureId;
        GLuint      mFrameBuffer;   // frame buffer for render texture
        bool        mIsDepthFrame;  // is current frame buffer bind depth
        
        static GLint mOldFrameBuffer;   // only cache last frame buffer
        static std::map<TextureFormat, TextureGLFormatInfo> mGLTextureFormatMap;
    };
}

#endif // PEACH3D_TEXTUREGL_H
