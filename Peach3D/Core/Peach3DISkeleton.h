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
        ISkeleton(const char* name) : mName(name) {}
        virtual ~ISkeleton();
        const std::string& getName() { return mName; }
        
        /** Add root bone to skeleton. */
        void addRootBone(Bone* root) { mRootBoneList.push_back(root); }
        const std::vector<Bone*>& getRootBoneList() { return mRootBoneList; }
        /** Make adding bone over, calc bones count and generate bones buffer. */
        virtual void addBonesOver();
        /** Find bone from cache list, must be called after "addBonesOver". */
        Bone* findBone(const char* name);
        /** Return all bone count, must called after addBonesOver. */
        int getAllBoneCount() { return (int)mCacheBones.size(); }
        /** Update bone current matrix and fill buffer. */
        virtual void fillAnimateBuffer(const std::string& name, float time);
        /** Return all bone matrix, must called after fillAnimateBuffer. */
        std::vector<Matrix4> getBonesAnimMatrix(const std::vector<std::string>& names);
        
        /** Add new animation name and time. */
        void addAnimateTime(const char* name, float time) { Peach3DAssert(name && time > 0.f, "Animatie name or time not valid"); mAnimations[name] = time; }
        /** Return named animation time. */
        float getAnimateTime(const std::string& name);
        
    private:
        /** Rranverse all children and cache list. */
        void cacheChildrenBonesList(Bone* parent);
        /** Rranverse all children, cache current children transform. */
        void cacheChildrenBonesMatrix(Bone* parent, const std::string& name, float time);
        
    protected:
        std::string mName;
        std::vector<Bone*> mRootBoneList;   // skeleton may not have only root bone
        std::vector<Bone*> mCacheBones;     // cache all bones to list, accelerate calc transform and find
        std::map<std::string, Matrix4> mCacheBoneMats;// cache all bones transform each render frame
        
        std::map<std::string, float> mAnimations;
    };
    
    // make shared mesh simple
    using SkeletonPtr = std::shared_ptr<ISkeleton>;
}

#endif /* PEACH3D_ISKELETON_H */
