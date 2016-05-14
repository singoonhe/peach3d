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
    
    Bone* Bone::findChildByName(const char* name)
    {
        Peach3DAssert(strlen(name) > 0, "Can't find child bone without name");
        if (mName.compare(name) == 0) {
            return this;
        }
        Bone* findChild = nullptr;
        if (mChildren.size() > 0) {
            // find bone in children
            for (auto child : mChildren) {
                if (child->getName() == name) {
                    findChild = child;
                    break;
                }
            }
            if (!findChild) {
                // find bone in children of child
                for (auto child : mChildren) {
                    findChild = child->findChildByName(name);
                    if (findChild) {
                        break;
                    }
                }
            }
        }
        return findChild;
    }
    
    Matrix4 Bone::calcBoneMatrix(const std::string& name, float time)
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
                calcR = calcR.slerp(nextFrame.rotate, calcInterval);
                calcS = (nextFrame.scale - baseFrame.scale) * calcInterval + baseFrame.scale;
                calcT = (nextFrame.translate - baseFrame.translate) * calcInterval + baseFrame.translate;
            }
            auto rotateM = Matrix4::createRotationQuaternion(calcR);
            auto scaleM = Matrix4::createScaling(calcS);
            auto translateM = Matrix4::createTranslation(calcT);
            mCacheMatrix = translateM * scaleM * rotateM;
            
            // multiply parent bone matrix
            if (mParentBone) {
                mCacheMatrix = mCacheMatrix * mParentBone->getCacheMatrix();
            }
        }
        return mCacheMatrix;
    }
}