//
//  Peach3DISkeleton.h
//  Peach3DLib
//
//  Created by singoon.he on 5/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_ISKELETON_H
#define PEACH3D_ISKELETON_H

#include "Peach3DBone.h"

namespace Peach3D
{
    class PEACH3D_DLL ISkeleton
    {
    public:
        ISkeleton(const char* name) : mName(name), mRootBone(nullptr) {}
        const std::string& getName() { return mName; }
        
        void setRootBone(Bone* root) { mRootBone = root; }
        Bone* getRootBone() { return mRootBone; }
        /** Make adding bone over, calc bones count and generate bones buffer. */
        virtual void addBonesOver();
        
        /** Return named animation time. */
        float getAnimateTime(const std::string& name);
        
    private:
        void cacheBonesList(Bone* parent);
        
    protected:
        std::string mName;
        Bone*       mRootBone;
        std::vector<Bone*> mCacheBones;
        
        std::map<std::string, float> mAnimations;
    };
    
    // make shared mesh simple
    using SkeletonPtr = std::shared_ptr<ISkeleton>;
}

#endif /* PEACH3D_ISKELETON_H */
