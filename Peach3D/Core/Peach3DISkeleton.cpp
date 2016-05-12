//
//  Peach3DISkeleton.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DISkeleton.h"

namespace Peach3D
{
    void ISkeleton::addBonesOver()
    {
        mCacheBones.clear();
        if (mRootBone) {
            mCacheBones.push_back(mRootBone);
            // cache all bones and clac count
            cacheBonesList(mRootBone);
        }
    }
    
    void ISkeleton::cacheBonesList(Bone* parent)
    {
        auto children = parent->getChildren();
        for (auto child : children) {
            mCacheBones.push_back(child);
            cacheBonesList(child);
        }
    }
    
    float ISkeleton::getAnimateTime(const std::string& name)
    {
        if (mAnimations.find(name) != mAnimations.end()) {
            return mAnimations[name];
        }
        return 0.f;
    }
}