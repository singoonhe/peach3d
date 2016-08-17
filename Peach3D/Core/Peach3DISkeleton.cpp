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
    ISkeleton::~ISkeleton()
    {
        // delete all root bones
        for (auto bone : mRootBoneList) {
            delete bone;
        }
        mRootBoneList.clear();
        mCacheBones.clear();
    }
    
    void ISkeleton::addBonesOver()
    {
        mCacheBones.clear();
        mUsedBonesCount = 0;
        if (mRootBoneList.size() > 0) {
            for (auto root : mRootBoneList) {
                mCacheBones.push_back(root);
                // cache all bones and clac count
                cacheChildrenBonesList(root);
            }
            // sort bones by index
            std::sort(mCacheBones.begin(), mCacheBones.end(), [](Bone* a, Bone* b)->int{
                return a->getIndex() < b->getIndex();
            });
            // calc valid bone count, invalid bone not need memory which index is -1
            for (auto bone : mCacheBones) {
                if (bone->getIndex() >= 0) {
                    mUsedBonesCount++;
                }
            }
        }
    }
    
    Bone* ISkeleton::findBone(const char* name)
    {
        Peach3DAssert(mCacheBones.size() > 0, "Find bone must called after \"addBonesOver\"");
        for (auto bone : mCacheBones) {
            if (bone->getName() == name) {
                return bone;
            }
        }
        return nullptr;
    }
    
    void ISkeleton::cacheChildrenBonesList(Bone* parent)
    {
        auto children = parent->getChildren();
        for (auto child : children) {
            mCacheBones.push_back(child);
            cacheChildrenBonesList(child);
        }
    }
    
    void ISkeleton::fillAnimateBuffer(const std::string& name, float time)
    {
        if (mCacheBones.size() > 0) {
            // update bones transform, cache list also update too
            for (auto root : mRootBoneList) {
                root->timeBoneMatrix(name, time);
                cacheChildrenBonesMatrix(root, name, time);
            }
        }
        // update all bones transform
        mCacheBoneMats.clear();
        for (auto bone : mCacheBones) {
            // only add valid index for animation, -1 not used in vertex
            if (bone->getIndex() >= 0) {
                mCacheBoneMats.push_back(bone->getCacheMatrix());
            }
        }
    }
    
    void ISkeleton::cacheChildrenBonesMatrix(Bone* parent, const std::string& name, float time)
    {
        auto children = parent->getChildren();
        for (auto child : children) {
            child->timeBoneMatrix(name, time);
            cacheChildrenBonesMatrix(child, name, time);
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