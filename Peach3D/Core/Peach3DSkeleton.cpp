//
//  Peach3DSkeleton.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DSkeleton.h"
#include "Peach3DLogPrinter.h"

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
    
    bool Skeleton::splitAnimation(const std::string& name, const std::vector<Bone::KeyFrameRange>& split)
    {
        auto findIter = mAnimations.find(name);
        if (findIter == mAnimations.end()) {
            return false;
        }
#if PEACH3D_DEBUG == 1
        // check is all range right
        for (auto range : split) {
            Peach3DAssert(name.size() > 0, "Split animation name not valid");
            if (range.startTime > findIter->second || range.startTime > range.endTime) {
                Peach3DErrorLog("Frame range time is not valid when spliting animation %s", name.c_str());
                return false;
            }
        }
#endif
        // split all bones
        std::vector<float> maxTimes(split.size(), 0.f);
        for (auto& bone : mCacheBones) {
            auto times = bone->splitFrames(name, split);
            for (auto i=0; i<times.size(); ++i) {
                if (times[i] > maxTimes[i]) {
                    maxTimes[i] = times[i];
                }
            }
        }
        // add new animations
        for (auto i=0; i<split.size(); ++i) {
            mAnimations[split[i].name] = maxTimes[i];
        }
        // delete old animation
        mAnimations.erase(findIter);
        return true;
    }
}
