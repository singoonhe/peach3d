//
//  Peach3DTextureGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DTextureGL.h"
#include "Peach3DIPlatform.h"
#include "Peach3DLayoutManager.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    GLint TextureGL::mOldFrameBuffer = 0;
    std::map<TextureFormat, TextureGLFormatInfo> TextureGL::mGLTextureFormatMap;
    
    void TextureGL::fillGLTextureFormatMap()
    {
        mGLTextureFormatMap[TextureFormat::eI8] = TextureGLFormatInfo(GL_ALPHA, GL_UNSIGNED_BYTE);
        mGLTextureFormatMap[TextureFormat::eRGB8] = TextureGLFormatInfo(GL_RGB, GL_UNSIGNED_BYTE);
        mGLTextureFormatMap[TextureFormat::eRGBA8] = TextureGLFormatInfo(GL_RGBA, GL_UNSIGNED_BYTE);
    }
    
    bool TextureGL::setTextureData(void* data, uint size, TextureDataStatus status)
    {
        bool isNewSuccess = true;
        do {
            // check width, height, format
            isNewSuccess = ITexture::setTextureData(data, size, status);
            if (!isNewSuccess) { break; }
            
            // gen texture
            glGenTextures(1, &mTextureId);
            glBindTexture(GL_TEXTURE_2D, mTextureId);
            if (mGLTextureFormatMap.size() == 0) {
                // init GL texture format map
                fillGLTextureFormatMap();
            }
            GLint glFormat = mGLTextureFormatMap[mTexFormat].glFormat;
            glTexImage2D(GL_TEXTURE_2D, 0, glFormat, mWidth, mHeight, 0, glFormat, mGLTextureFormatMap[mTexFormat].glByte, data);
            
            // if enable mipmap
            if (ResourceManager::getSingleton().isTextureMipMapEnabled()) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            // set default texture filter
            setFilterNoBind(mTexFilter);
            // set texture wrap
            //!!! this must be set, or texture2D() will return black on OpenGL ES
            setWrapNoBind(mTexWrap);
            
            glBindTexture(GL_TEXTURE_2D, 0);
        } while (0);
        return isNewSuccess;
    }
    
    bool TextureGL::setTextureData(void* dataList[6], uint sizeList[6], TextureDataStatus status)
    {
        bool isNewSuccess = true;
        do {
            // check width, height, format, data size
            isNewSuccess = ITexture::setTextureData(dataList, sizeList, status);
            if (!isNewSuccess) { break; }
            
            // gen texture
            glGenTextures(1, &mTextureId);
            glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);
            if (mGLTextureFormatMap.size() == 0) {
                // init GL texture format map
                fillGLTextureFormatMap();
            }
            GLint glFormat = mGLTextureFormatMap[mTexFormat].glFormat;
            for (auto i = 0; i < 6; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glFormat, mWidth, mHeight, 0, glFormat,
                             mGLTextureFormatMap[mTexFormat].glByte, dataList[i]);
            }
            
            // if enable mipmap
            if (ResourceManager::getSingleton().isTextureMipMapEnabled()) {
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            }
            // set default texture filter
            setFilterNoBind(mTexFilter);
            // set texture wrap
            //!!! this must be set, or texture2D() will return black on OpenGL ES
            setWrapNoBind(mTexWrap);
            
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        } while (0);
        return isNewSuccess;
    }
    
    bool TextureGL::usingAsRenderTexture(int width, int height, bool isDepth)
    {
        bool isSuccess = ITexture::usingAsRenderTexture(width, height);
        if (isSuccess) {
            mIsDepthFrame = isDepth;
            mTexType = TextureType::eRTT;
            // generate GL texture
            glGenTextures(1, &mTextureId);
            glBindTexture(GL_TEXTURE_2D, mTextureId);
            if (isDepth) {
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_GLES
                GLint internalFormat = PD_RENDERLEVEL_GL3() ? GL_DEPTH_COMPONENT32F : GL_DEPTH_COMPONENT;
#else
                GLint internalFormat = PD_RENDERLEVEL_GL3() ? GL_DEPTH_COMPONENT32F : GL_DEPTH_COMPONENT32;
#endif
                GLenum dataType = PD_RENDERLEVEL_GL3() ? GL_FLOAT : GL_UNSIGNED_INT;
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_DEPTH_COMPONENT, dataType, 0);
                GL_CHECK_ERROR("TextureGL::usingAsRenderTexture glTexImage2D");
                mTexFormat = TextureFormat::eDepth;
                // set depth compare stype
                if (PD_RENDERLEVEL_GL3()) {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                }
            }
            else {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                GL_CHECK_ERROR("TextureGL::usingAsRenderTexture glTexImage2D");
                mTexFormat = TextureFormat::eRGBA8;
            }
            // set default texture filter
            setFilterNoBind(TextureFilter::eLinear);
            // set texture wrap
            //!!! this must be set, or texture2D() will return black on OpenGL ES
            setWrapNoBind(mTexWrap);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            // cache old frame buffer
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mOldFrameBuffer);
            // generate frame buffer
            glGenFramebuffers(1, &mFrameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
            if (isDepth) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTextureId, 0);
                bool isModifyBuffers = true;
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_GLES
                isModifyBuffers = PD_RENDERLEVEL_GL3();
#endif
                if (isModifyBuffers) {
                    // draw nothing
                    GLenum drawBuffers[1] = {GL_NONE};
                    glDrawBuffers(1, drawBuffers);
                    GL_CHECK_ERROR("TextureGL::usingAsRenderTexture glDrawBuffers");
                    glReadBuffer(GL_NONE);
                    GL_CHECK_ERROR("TextureGL::usingAsRenderTexture glReadBuffer");
                }
            }
            else {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureId, 0);
            }
            GL_CHECK_ERROR("TextureGL::usingAsRenderTexture glFramebufferTexture2D");
            // check is frame buffer complete
            auto checkStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            glBindFramebuffer(GL_FRAMEBUFFER, mOldFrameBuffer);
            if(checkStatus != GL_FRAMEBUFFER_COMPLETE) {
                glDeleteTextures(1, &mTextureId);
                mTextureId = 0;
                glDeleteFramebuffers(1, &mFrameBuffer);
                mFrameBuffer = 0;
                Peach3DErrorLog("Render texture frame buffer check error code:%d", checkStatus);
                return false;
            }
            
            isSuccess = true;
            mWidth = width;
            mHeight = height;
        }
        return isSuccess;
    }
    
    void TextureGL::setFilter(TextureFilter filter)
    {
        if (mTextureId && filter != mTexFilter) {
            ITexture::setFilter(filter);
            
            GLenum curTarget = mTexType == TextureType::eCube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
            glBindTexture(curTarget, mTextureId);
            setFilterNoBind(filter);
            glBindTexture(curTarget, 0);
        }
    }
    
    void TextureGL::setFilterNoBind(TextureFilter filter)
    {
        GLenum curTarget = mTexType == TextureType::eCube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
        bool mipmapEnabled = ResourceManager::getSingleton().isTextureMipMapEnabled();
        if (filter == TextureFilter::eLinear) {
            // set linear texture filter
            glTexParameteri(curTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(curTarget, GL_TEXTURE_MIN_FILTER, mipmapEnabled ? GL_LINEAR_MIPMAP_LINEAR: GL_LINEAR);
        }
        else if (filter == TextureFilter::eAnisotropic) {
            // set anisotropic texture filter
            glTexParameteri(curTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(curTarget, GL_TEXTURE_MIN_FILTER, mipmapEnabled ? GL_LINEAR_MIPMAP_LINEAR: GL_LINEAR);
            float maxAnisotropyLevel = 0.0f;
            glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropyLevel );
            glTexParameterf(curTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropyLevel );
        }
        else {
            // set nearest texture filter default
            glTexParameteri(curTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(curTarget, GL_TEXTURE_MIN_FILTER, mipmapEnabled ? GL_NEAREST_MIPMAP_NEAREST: GL_NEAREST);
        }
    }
    
    void TextureGL::setWrap(TextureWrap wrap)
    {
        if (mTextureId && wrap != mTexWrap) {
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_GLES
            RenderFeatureLevel level = IPlatform::getSingleton().getRenderFeatureLevel();
            if (level == RenderFeatureLevel::eGL2 && (((mWidth & mWidth - 1) != 0) || ((mHeight & mHeight - 1) != 0))) {
                Peach3DLog(LogLevel::eWarn, "OpenGL ES 2.0 only support modify wrap that texture size is power of two");
                return ;
            }
#endif
            GLenum curTarget = mTexType == TextureType::eCube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
            ITexture::setWrap(wrap);
            // set texture wrap
            glBindTexture(curTarget, mTextureId);
            setWrapNoBind(wrap);
            glBindTexture(curTarget, 0);
        }
    }
    
    void TextureGL::setWrapNoBind(TextureWrap wrap)
    {
        GLenum curTarget = mTexType == TextureType::eCube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
        if (wrap == TextureWrap::eClampToEdge) {
            // clamp to edge
            glTexParameteri(curTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(curTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            if (mTexType == TextureType::eCube) {
                glTexParameteri(curTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }
        }
        else if (wrap == TextureWrap::eRepeat) {
            // repeat
            glTexParameteri(curTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(curTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
            if (mTexType == TextureType::eCube) {
                glTexParameteri(curTarget, GL_TEXTURE_WRAP_R, GL_REPEAT);
            }
        }
        else if (wrap == TextureWrap::eMirrored) {
            // mirrored repeat
            glTexParameteri(curTarget, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(curTarget, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            if (mTexType == TextureType::eCube) {
                glTexParameteri(curTarget, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
            }
        }
    }
    
    void TextureGL::beforeRendering()
    {
        ITexture::beforeRendering();
        
        // cache last frame buffer first
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mOldFrameBuffer);
        // bind framebuffer and set viewport
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
        glViewport(0, 0, mWidth, mHeight);
        if (mIsDepthFrame) {
            // only need clear depth frame
            glClear(GL_DEPTH_BUFFER_BIT);
            // open polygon offset
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.f, 4.f);
        }
        else {
            // use normal GL state
            IRender::getSingleton().prepareForMainRender();
        }
    }
    
    void TextureGL::afterRendering()
    {
        ITexture::afterRendering();
        
        if (mIsDepthFrame) {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        // restore frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, mOldFrameBuffer);
        // restore viewport
        auto size = LayoutManager::getSingleton().getScreenSize();
        glViewport(0, 0, (int)lroundf(size.x), (int)lroundf(size.y));
    }
    
    TextureGL::~TextureGL()
    {
        if (mTextureId) {
            glDeleteTextures(1, &mTextureId);
            mTextureId = 0;
        }
        if (mFrameBuffer) {
            glDeleteFramebuffers(1, &mFrameBuffer);
            mFrameBuffer = 0;
        }
    }
}
