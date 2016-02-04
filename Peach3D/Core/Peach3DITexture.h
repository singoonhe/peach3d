//
//  Peach3DITexture.h
//  TestPeach3D
//
//  Created by singoon he on 12-5-23.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_ITEXTURE_H
#define PEACH3D_ITEXTURE_H

#include "Peach3DCompile.h"
#include "Peach3DRect.h"

namespace Peach3D
{
    // define texture data status
    enum class PEACH3D_DLL TextureDataStatus
    {
        eCompressed,    // for some compressed texture, like DDS...
        eEncoded,       // read from file, like jpeg/png
        eDecoded,       // decoded data, must set width/height/format before setTextureData call
    };
    
    // define texture type
    enum class PEACH3D_DLL TextureType
    {
        e2D,        // 2D texture
        eCube,      // cube texture
    };

    // define texture format supported
    enum class PEACH3D_DLL TextureFormat
    {
        eUnknow,    // unknow format for texture init
        eI8,        // gray texture, only 8 bits
        eA8I8,      // texture include 8 bits gray and 8 bits alpha
        eRGB8,      // R8G8B8 texture
        eRGBA8,     // R8G8B8A8 texture
    };
    
    // define texture filter
    enum class PEACH3D_DLL TextureFilter
    {
        eNearest,   // texture filter use nearest
        eLinear,    // texture filter use linear
        eAnisotropic, // supported by gl3 ext, gl2 ext, dx11
    };
    
    // define texture wrap
    enum class PEACH3D_DLL TextureWrap
    {
        eClampToEdge,       // texture clamp to edge, default value
        eRepeat,            // texture repeat
        eMirrored,          // texture mirrored repeat
    };
    
    class PEACH3D_DLL ITexture
    {
    public:
        /** Set texture data and init texture, format and size must set before. */
        virtual bool setTextureData(void* data, uint size, TextureDataStatus status = TextureDataStatus::eDecoded);
        /** Set texture data and init texture for cube texture, format and size must set before. */
        virtual bool setTextureData(void* dataList[6], uint sizeList[6], TextureDataStatus status = TextureDataStatus::eDecoded);
        
        //! set texture format
        virtual void setFormat(TextureFormat format) { mTexFormat = format; }
        //! get texture format
        TextureFormat getFormat() { return mTexFormat; }
        //! set texture width
        virtual void setWidth(uint width) { mWidth = width; }
        //! get texture width
        uint getWidth() { return mWidth; }
        //! set texture height
        virtual void setHeight(uint height) { mHeight = height; }
        //! get texture height
        uint getHeight() { return mHeight; }
        //! get texture name
        const std::string& getName()const {return mTexName;}
        
        //! set texture filter
        virtual void setFilter(TextureFilter filter) { mTexFilter = filter; }
        //! set wrap texture
        virtual void setWrap(TextureWrap wrap) { mTexWrap = wrap; }
        
    protected:
        ITexture(const char* name);
        virtual ~ITexture() {}
        
    protected:
        std::string     mTexName;   // texture name (file relative path or named by user)
        TextureFormat   mTexFormat; // texture format
        uint            mWidth;     // texture width
        uint            mHeight;    // texture height
        TextureFilter   mTexFilter; // texture filter
        TextureWrap     mTexWrap;   // texture wrap
        TextureType     mTexType;   // texture type
        
        friend class   IRender;
    };
    
    // define texture frame, include texture and coord
    struct PEACH3D_DLL TextureFrame
    {
        TextureFrame() : tex(nullptr) {}
        TextureFrame(ITexture* _tex) : tex(_tex) { rc.pos.x = rc.pos.y = 0.f; rc.size.x = rc.size.y = 1.f; }
        TextureFrame(ITexture* _tex, const Rect& _rc, const std::string& _name="") : tex(_tex), rc(_rc), name(_name) {}
        std::string name;   // texture name, used for Sprite::create "#xxxx.png"
        ITexture*   tex;    // texture handler
        Rect        rc;     // texture coord
    };
}

#endif // PEACH3D_ITEXTURE_H
