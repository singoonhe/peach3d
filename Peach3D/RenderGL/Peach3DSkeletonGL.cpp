//
//  Peach3DSkeletonGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/11/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DSkeletonGL.h"

namespace Peach3D
{
    SkeletonGL::~SkeletonGL()
    {
        if (mTBOId) {
            glDeleteTextures(1, &mTBOId);
            mTBOId = 0;
        }
    }
}