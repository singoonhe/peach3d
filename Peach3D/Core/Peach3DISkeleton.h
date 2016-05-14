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
        virtual ~ISkeleton();
        const std::string& getName() { return mName; }
        
        void setRootBone(Bone* root) { mRootBone = root; }
        Bone* getRootBone() { return mRootBone; }
        /** Make adding bone over, calc bones count and generate bones buffer. */
        virtual void addBonesOver();
        /** Return all bone count, must called after addBonesOver. */
        int getBoneCount() { return (int)mCacheBones.size(); }
        /** Update bone current matrix and fill buffer. */
        virtual void fillAnimateBuffer(const std::string& name, float time);
        /** Return all bone matrix, must called after fillAnimateBuffer. */
        const std::vector<Matrix4>& getBonesMatrix() { return mCacheBoneMats; }
        
        /** Add new animation name and time. */
        void addAnimateTime(const char* name, float time) { Peach3DAssert(name && time > 0.f, "Animatie name or time not valid"); mAnimations[name] = time; }
        /** Return named animation time. */
        float getAnimateTime(const std::string& name);
        
    private:
        void cacheChildrenBonesList(Bone* parent);
        void cacheChildrenBonesMatrix(Bone* parent, const std::string& name, float time);
        
    protected:
        std::string mName;
        Bone*       mRootBone;
        std::vector<Bone*> mCacheBones;
        std::vector<Matrix4> mCacheBoneMats;
        
        std::map<std::string, float> mAnimations;
    };
    
    // make shared mesh simple
    using SkeletonPtr = std::shared_ptr<ISkeleton>;
}

#endif /* PEACH3D_ISKELETON_H */
