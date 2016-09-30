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
    Skeleton::~Skeleton()
    {
        // delete all root bones
        for (auto& bone : mRootBoneList) {
            delete bone;
        }
        mRootBoneList.clear();
        mCacheBones.clear();
    }
    
    void Skeleton::addBonesOver()
    {
        mCacheBones.clear();
        if (mRootBoneList.size() > 0) {
            for (auto& root : mRootBoneList) {
                mCacheBones.push_back(root);
                // cache all bones and clac count
                cacheChildrenBonesList(root);
            }
        }
    }
    
    Bone* Skeleton::findBone(const char* name)
    {
        Peach3DAssert(mCacheBones.size() > 0, "Find bone must called after \"addBonesOver\"");
        for (auto& bone : mCacheBones) {
            if (bone->getName() == name) {
                return bone;
            }
        }
        return nullptr;
    }
    
    void Skeleton::cacheChildrenBonesList(Bone* parent)
    {
        auto children = parent->getChildren();
        for (auto& child : children) {
            mCacheBones.push_back(child);
            cacheChildrenBonesList(child);
        }
    }
    
    void Skeleton::fillAnimateBuffer(const std::string& name, float time)
    {
        if (mCacheBones.size() > 0) {
            // update bones transform, cache list also update too
            for (auto& root : mRootBoneList) {
                mCacheBoneMats[root->getName()] = root->timeBoneMatrix(name, time);
                cacheChildrenBonesMatrix(root, name, time);
            }
        }
    }
    
    void Skeleton::cacheChildrenBonesMatrix(Bone* parent, const std::string& name, float time)
    {
        auto children = parent->getChildren();
        for (auto& child : children) {
            mCacheBoneMats[child->getName()] = child->timeBoneMatrix(name, time);
            cacheChildrenBonesMatrix(child, name, time);
        }
    }
    
    float Skeleton::getAnimateTime(const std::string& name)
    {
        if (mAnimations.find(name) != mAnimations.end()) {
            return mAnimations[name];
        }
        return 0.f;
    }
    
    std::vector<Matrix4> Skeleton::getBonesAnimMatrix(const std::vector<std::string>& names)
    {
        std::vector<Matrix4> cached;
        for (auto& name : names) {
            cached.push_back(mCacheBoneMats[name]);
        }
        return cached;
    }
}
