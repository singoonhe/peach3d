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
        eUnknow,    // default texture type
        e2D,        // 2D texture
        eCube,      // cube texture
        eRTT,       // render to texture
    };

    // define texture format supported
    enum class PEACH3D_DLL TextureFormat
    {
        eUnknow,    // unknow format for texture init
        eI8,        // gray texture, only 8 bits
        eA8I8,      // texture include 8 bits gray and 8 bits alpha
        eRGB8,      // R8G8B8 texture
        eRGBA8,     // R8G8B8A8 texture
        eDepth,     // depth component format
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
        // constructor and destructor must be public, because shared_ptr need call them
        ITexture(const char* name);
        virtual ~ITexture() {}
        
        /** Set texture data and init texture, format and size must set before. */
        virtual bool setTextureData(void* data, uint size, TextureDataStatus status = TextureDataStatus::eDecoded);
        /** Set texture data and init texture for cube texture, format and size must set before. */
        virtual bool setTextureData(void* dataList[6], uint sizeList[6], TextureDataStatus status = TextureDataStatus::eDecoded);
        /** Create RTT texture(render to texture).
         * @params isDepth if true, draw depth data to texture.
         */
        virtual bool usingAsRenderTexture(int width, int height, bool isDepth = false) { return mTexType == TextureType::eUnknow; }
        
        virtual void setFormat(TextureFormat format) { mTexFormat = format; }
        TextureFormat getFormat() { return mTexFormat; }
        virtual void setWidth(uint width) { mWidth = width; }
        uint getWidth() { return mWidth; }
        virtual void setHeight(uint height) { mHeight = height; }
        uint getHeight() { return mHeight; }
        virtual void setFilter(TextureFilter filter) { mTexFilter = filter; }
        TextureFilter getFilter() { return mTexFilter; }
        virtual void setWrap(TextureWrap wrap) { mTexWrap = wrap; }
        TextureWrap getWrap() { return mTexWrap; }
        /* Return texture file name or system name. */
        const std::string& getName()const { return mTexName; }
        TextureType getType() { return mTexType; }
        /* Return texture file name or system name. */
        void setBeforeRenderingFunc(const std::function<void()>& func) { mBeforeFunc = func; }
        void setAfterRenderingFunc(const std::function<void()>& func) { mAfterFunc = func; }
        /* SceneManager rendering called function. */
        virtual void beforeRendering() { if (mBeforeFunc) mBeforeFunc();}
        virtual void afterRendering() { if (mAfterFunc) mAfterFunc();}
        /* Control is RTT need update, used by engine. */
        void setActived(bool active) { mIsActived = active; }
        bool isActived() { return mIsActived; }
        
    protected:
        std::string     mTexName;   // texture name (file relative path or named by user)
        TextureFormat   mTexFormat; // texture format
        uint            mWidth;     // texture width
        uint            mHeight;    // texture height
        TextureFilter   mTexFilter; // texture filter
        TextureWrap     mTexWrap;   // texture wrap
        TextureType     mTexType;   // texture type
        
        bool            mIsActived; // is texture actived, used for RTT
        std::function<void()>   mBeforeFunc;// render before function
        std::function<void()>   mAfterFunc; // render after function
        
        friend class   IRender;
    };
    
    // make shared texture simple
    using TexturePtr = std::shared_ptr<ITexture>;
    
    // define texture frame, include texture and coord
    struct PEACH3D_DLL TextureFrame
    {
        TextureFrame() : tex(nullptr) { rc.pos.x = rc.pos.y = 0.f; rc.size.x = rc.size.y = 1.f; }
        TextureFrame(const TexturePtr& _tex, const std::string& _name="") : tex(_tex), name(_name) { rc.pos.x = rc.pos.y = 0.f; rc.size.x = rc.size.y = 1.f; }
        TextureFrame(const TexturePtr& _tex, const Rect& _rc, const std::string& _name="") : tex(_tex), rc(_rc), name(_name) {}
        TextureFrame &operator=(const TextureFrame& other){ name = other.name; tex = other.tex; rc = other.rc; return *this; }
        std::string name;   // texture name, used for Sprite::create "#xxxx.png"
        TexturePtr   tex;    // texture handler
        Rect        rc;     // texture coord
    };
}

#endif // PEACH3D_ITEXTURE_H
