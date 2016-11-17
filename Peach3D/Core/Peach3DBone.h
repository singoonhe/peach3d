//
//  Peach3DBone.h
//  Peach3DLib
//
//  Created by singoon.he on 5/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_BONE_H
#define PEACH3D_BONE_H

#include "Peach3DVector3.h"
#include "Peach3DMatrix4.h"
#include "Peach3DQuaternion.h"

namespace Peach3D
{
    struct PEACH3D_DLL BoneKeyFrame
    {
        BoneKeyFrame() {}
        BoneKeyFrame(float _time, const Quaternion& _rotate, const Vector3& _scale, const Vector3& _translate) : time(_time), rotate(_rotate), scale(_scale), translate(_translate) {}
        BoneKeyFrame &operator=(const BoneKeyFrame& other) { time = other.time; rotate = other.rotate; scale = other.scale; translate = other.translate; return *this; }
        float       time;
        Quaternion  rotate;
        Vector3     scale;
        Vector3     translate;
    };
    
    class SceneNode;
    class PEACH3D_DLL Bone
    {
    public:
        // used for split animation
        struct PEACH3D_DLL KeyFrameRange
        {
            KeyFrameRange() {}
            KeyFrameRange(const std::string& _name, float _startT, float _endT) : name(_name), startTime(_startT), endTime(_endT) {}
            std::string name;   // animation name
            float startTime;
            float endTime;
        };
        
    public:
        Bone(const char* name) : mName(name), mParentBone(nullptr) {}
        ~Bone();
        const std::string& getName() { return mName; }
        Bone* getParentBone() { return mParentBone; }
        
        /** Set bone pose matrix when import file. */
        void setPoseTransform(const Matrix4& transform) { mPoseMatrix = transform; mWorldMatrix = transform; }
        /** Set bone pose inverse transform when import file. */
        void setInverseTransform(const Matrix4& transform) { mInvTransform = transform; }
        /** Add new frame for animation when import file. */
        void addKeyFrame(const char* name, const BoneKeyFrame& frame) { mAnimationFrames[name].push_back(frame); }
        /** Split named frames to more, delete old frames.
         * @return New animation running time list.
         */
        std::vector<float> splitFrames(const std::string& name, const std::vector<Bone::KeyFrameRange>& split);
        
        void addChild(Bone* child);
        void tranverseChildBone(std::function<void(Bone*)> callFunc);
        uint getChildrenCount() { return (uint)mChildren.size(); }
        const std::vector<Bone*>& getChildren() {return mChildren;}
        
        /** Calc bone current transform matrix. */
        const Matrix4& timeBoneMatrix(const std::string& name, float time);
        const Matrix4& getWorldMatrix() { return mWorldMatrix; }
        
    private:
        /** Set parent bone, only called for addChild. */
        void setParentBone(Bone* pBone) { mParentBone = pBone; }
        
    private:
        std::string mName;
        Matrix4     mInvTransform;  // inverse of bone transform
        Matrix4     mPoseMatrix;    // bone pose transform
        Matrix4     mCacheMatrix;   // cache bone time transform, pass to program
        Matrix4     mWorldMatrix;
        Bone*       mParentBone;
        
        std::vector<Bone*> mChildren;
        std::map<std::string, std::vector<BoneKeyFrame>> mAnimationFrames;
    };
}

#endif /* PEACH3D_BONE_H */
