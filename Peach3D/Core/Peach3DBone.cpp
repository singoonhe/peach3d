//
//  Peach3DBone.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DBone.h"

namespace Peach3D
{
    Bone::~Bone()
    {
        // delete children
        for (auto& child : mChildren) {
            delete child;
        }
        mChildren.clear();
    }
    
    void Bone::addChild(Bone* child)
    {
        Peach3DAssert(child, "Can't add a null bone!");
        Peach3DAssert(!child->getParentBone(), "Child already add to another bone!");
        
        // add child bone
        if (!child->getParentBone()) {
            mChildren.push_back(child);
            child->setParentBone(this);
        }
    }
    
    void Bone::tranverseChildBone(std::function<void(Bone*)> callFunc)
    {
        for (size_t i=0; i<mChildren.size(); ++i) {
            callFunc(mChildren[i]);
        }
    }
    
    std::vector<float> Bone::splitFrames(const std::string& name, const std::vector<Bone::KeyFrameRange>& split)
    {
        std::vector<float> times;
        auto findIter = mAnimationFrames.find(name);
        if (findIter == mAnimationFrames.end()) {
            return times;
        }
        for (auto& range : split) {
            std::vector<BoneKeyFrame> frames;
            float startCheckTime = range.startTime - FLT_EPSILON;
            float endCheckTime = range.endTime + FLT_EPSILON;
            float totalTime = 0.f;
            for (auto frame : findIter->second) {
                // time in range
                if (frame.time >= startCheckTime && frame.time <= endCheckTime) {
                    // make frames time start with 0
                    if (frames.empty()) {
                        frame.time = 0.f;
                    }
                    else if (range.startTime > 0.f) {
                        frame.time -= range.startTime;
                    }
                    frames.push_back(frame);
                    // save max time as animation length
                    if (frame.time > totalTime) {
                        totalTime = frame.time;
                    }
                }
            }
            times.push_back(totalTime);
            mAnimationFrames[range.name] = frames;
        }
        // delete old animation
        mAnimationFrames.erase(findIter);
        return times;
    }
    
    const Matrix4& Bone::timeBoneMatrix(const std::string& name, float time)
    {
        if (mAnimationFrames.find(name) != mAnimationFrames.end()) {
            auto& frameList = mAnimationFrames[name];
            // find based frame
            size_t curIndex = 0, maxSize = frameList.size();
            for (auto i=0; i<maxSize; ++i) {
                if (time >= frameList[i].time) {
                    curIndex = i;
                }
            }
            auto baseFrame = frameList[curIndex];
            // last frame not need calc interpolation
            Quaternion calcR = baseFrame.rotate;
            Vector3 calcS = baseFrame.scale, calcT = baseFrame.translate;
            if (curIndex < (maxSize - 1)) {
                // calc last interpolation between current and next frame
                auto nextFrame = frameList[curIndex + 1];
                auto interval = time - baseFrame.time, frameInterval = nextFrame.time - baseFrame.time;
                auto calcInterval = interval/frameInterval;
                // not calc if not change, slerp take much time
                if (calcR != nextFrame.rotate) {
                    calcR = calcR.slerp(nextFrame.rotate, calcInterval);
                }
                calcS = (nextFrame.scale - baseFrame.scale) * calcInterval + baseFrame.scale;
                calcT = (nextFrame.translate - baseFrame.translate) * calcInterval + baseFrame.translate;
            }
            // TODO: rotation with quaternion need transpose, it's strange.
            auto rotateM = Matrix4::createRotationQuaternion(calcR).getTranspose();
            auto scaleM = Matrix4::createScaling(calcS);
            auto translateM = Matrix4::createTranslation(calcT);
            mWorldMatrix = translateM  * scaleM * rotateM;
        }
        else {
            mWorldMatrix = mPoseMatrix;
        }
        
        // multiply parent bone matrix
        if (mParentBone) {
            mWorldMatrix = mParentBone->getWorldMatrix() * mWorldMatrix;
        }
        // current matrix = transform * pose matrix, transform = current matrix * pose inverse matrix
        mCacheMatrix = mWorldMatrix  *  mInvTransform;
        return mCacheMatrix;
    }
}
