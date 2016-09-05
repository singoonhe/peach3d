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
        for (auto child : mChildren) {
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
    
    const Matrix4& Bone::timeBoneMatrix(const std::string& name, float time)
    {
        if (mAnimationFrames.find(name) != mAnimationFrames.end()) {
            auto frameList = mAnimationFrames[name];
            // find based frame
            size_t curIndex = 0, maxSize = frameList.size();
            for (auto i=0; i<frameList.size(); ++i) {
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