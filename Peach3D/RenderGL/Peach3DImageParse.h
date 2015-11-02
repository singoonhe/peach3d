//
//  Peach3DImageParse.h
//  Peach3DLib
//
//  Created by singoon.he on 9/26/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_IMAGEPARSE_H
#define PEACH3D_IMAGEPARSE_H

#include "Peach3DCompile.h"
#include "Peach3DITexture.h"

namespace Peach3D
{
    // parse jpeg data to RGB/RGBA/I8
    uchar* jpegImageDataParse(void* orignData, uint orignSize, uint* outSize, TextureFormat* format, uint* width, uint* height);
    // parse png data to RGB/RGBA/I8/I8A8
    uchar* pngImageDataParse(void* orignData, uint orignSize, uint* outSize, TextureFormat* format, uint* width, uint* height);
}

#endif // PEACH3D_IMAGEPARSE_H
