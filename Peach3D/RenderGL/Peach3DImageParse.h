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
#include "Peach3DTypes.h"

namespace Peach3D
{
    // parse jpeg data to RGB/RGBA/I8
    void* jpegImageDataParse(const ResourceLoaderInput& input);
    // parse png data to RGB/RGBA/I8/I8A8
    void* pngImageDataParse(const ResourceLoaderInput& input);
}

#endif // PEACH3D_IMAGEPARSE_H
