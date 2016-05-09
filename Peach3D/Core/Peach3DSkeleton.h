//
//  Peach3DSkeleton.h
//  Peach3DLib
//
//  Created by singoon.he on 5/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SKELETON_H
#define PEACH3D_SKELETON_H

#include "Peach3DBone.h"

namespace Peach3D
{
    class PEACH3D_DLL Skeleton
    {
    public:
        Skeleton(const char* name) : mName(name), mRootBone(nullptr) {}
        const std::string& getName() { return mName; }
        
        void setRootBone(Bone* root) { mRootBone = root; }
        Bone* getRootBone() { return mRootBone; }
        
        /** If skeleton have named animation. */
        bool isAnimateValid(const std::string& name) { return mAnimations.find(name) != mAnimations.end(); }
        
    private:
        std::string mName;
        Bone*       mRootBone;
        
        std::map<std::string, float> mAnimations;
    };
    
    // make shared mesh simple
    using SkeletonPtr = std::shared_ptr<Skeleton>;
}

#endif /* PEACH3D_SKELETON_H */
