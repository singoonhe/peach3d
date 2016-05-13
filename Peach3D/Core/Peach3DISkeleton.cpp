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
        mCacheBones.clear();
        if (mRootBone) {
            delete mRootBone;
            mRootBone = nullptr;
        }
    }
    
    void ISkeleton::addBonesOver()
    {
        mCacheBones.clear();
        if (mRootBone) {
            mCacheBones.push_back(mRootBone);
            // cache all bones and clac count
            cacheChildrenBonesList(mRootBone);
        }
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
        mCacheBoneMats.clear();
        if (mRootBone) {
            mCacheBoneMats.push_back(mRootBone->calcBoneMatrix(name, time));
            cacheChildrenBonesMatrix(mRootBone, name, time);
        }
    }
    
    void ISkeleton::cacheChildrenBonesMatrix(Bone* parent, const std::string& name, float time)
    {
        auto children = parent->getChildren();
        for (auto child : children) {
            mCacheBoneMats.push_back(child->calcBoneMatrix(name, time));
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