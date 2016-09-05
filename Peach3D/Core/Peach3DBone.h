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
        Bone(const char* name, int index) : mName(name), mAttachedNode(nullptr), mParentBone(nullptr) {}
        ~Bone();
        const std::string& getName() { return mName; }
        Bone* getParentBone() { return mParentBone; }
        
        /** Set bone pose matrix when import file. */
        void setPoseTransform(const Matrix4& transform) { mPoseMatrix = transform; mWorldMatrix = transform; }
        /** Set bone pose inverse transform when import file. */
        void setInverseTransform(const Matrix4& transform) { mInvTransform = transform; }
        /** Add new frame for animation when import file. */
        void addKeyFrame(const char* name, const BoneKeyFrame& frame) { mAnimationFrames[name].push_back(frame); }
        
        void addChild(Bone* child);
        void tranverseChildBone(std::function<void(Bone*)> callFunc);
        uint getChildrenCount() { return (uint)mChildren.size(); }
        const std::vector<Bone*>& getChildren() {return mChildren;}
        
        /** Calc bone current transform matrix. */
        const Matrix4& timeBoneMatrix(const std::string& name, float time);
        const Matrix4& getWorldMatrix() { return mWorldMatrix; }
        /** Attach scene node to bone, such as weapon. */
        void attachNode(SceneNode* tn) { mAttachedNode = tn; }
        void detachNode() { mAttachedNode = nullptr; }
        
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
        SceneNode*  mAttachedNode;
        
        std::vector<Bone*> mChildren;
        std::map<std::string, std::vector<BoneKeyFrame>> mAnimationFrames;
    };
}

#endif /* PEACH3D_BONE_H */
