//
//  Peach3DITexture.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-5-23.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DITexture.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    ITexture::ITexture(const char* name) : mTexName(name), mTexFormat(TextureFormat::eUnknow),
        mTexWrap(TextureWrap::eClampToEdge), mWidth(0), mHeight(0)
    {
        // set texture filter and mipmap
        mTexFilter = ResourceManager::getSingleton().getDefaultTextureFilter();
    }
    
    bool ITexture::setTextureData(void* data, uint size, TextureDataStatus status)
    {
        if ((mWidth == 0 || mHeight == 0) && status==TextureDataStatus::eDecoded)
        {
            Peach3DLog(LogLevel::eError, "Size must be set before setTextureData for decoded data");
            return false;
        }
        // auto set texture type to 2D
        mTexType = TextureType::e2D;
        return true;
    }
    
    bool ITexture::setTextureData(void* dataList[6], uint sizeList[6], TextureDataStatus status)
    {
        if (ITexture::setTextureData(dataList[0], sizeList[0], status)) {
            // check is same size ?
            ulong baseSize = sizeList[0];
            for (size_t i = 1; i < 6; ++i) {
                if (sizeList[i] != baseSize) {
                    Peach3DLog(LogLevel::eError, "Cube six texture data must be same size");
                    return false;
                }
            }
            // auto set texture type to cube
            mTexType = TextureType::eCube;
            return true;
        }
        return false;
    }
}