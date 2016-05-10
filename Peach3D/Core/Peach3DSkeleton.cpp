//
//  Peach3DSkeleton.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DSkeleton.h"

namespace Peach3D
{
    float Skeleton::getAnimateTime(const std::string& name)
    {
        if (mAnimations.find(name) != mAnimations.end()) {
            return mAnimations[name];
        }
        return 0.f;
    }
}