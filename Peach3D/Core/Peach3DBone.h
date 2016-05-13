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
        float       time;
        Quaternion  rotate;
        Vector3     scale;
        Vector3     translate;
    };
    
    class SceneNode;
    class PEACH3D_DLL Bone
    {
    public:
        Bone(const char* name) : mName(name), mAttachedNode(nullptr), mParentBone(nullptr) {}
        ~Bone();
        const std::string& getName() { return mName; }
        Bone* getParentBone() { return mParentBone; }
        
        void addChild(Bone* child);
        void tranverseChildBone(std::function<void(Bone*)> callFunc);
        uint getChildrenCount() { return (uint)mChildren.size(); }
        const std::vector<Bone*>& getChildren() {return mChildren;}
        /** Find child node by name, will iterative search children. */
        Bone* findChildByName(const char* name);
        /** Calc bone current transform matrix. */
        Matrix4 calcBoneMatrix(const std::string& name, float time);
        const Matrix4& getCacheMatrix() { return mCacheMatrix; }
        /** Attach scene node to bone, such as weapon. */
        void attachNode(SceneNode* tn) { mAttachedNode = tn; }
        void detachNode() { mAttachedNode = nullptr; }
        
    private:
        /** Set parent bone, only called for addChild. */
        void setParentBone(Bone* pBone) { mParentBone = pBone; }
        
    private:
        Matrix4     mTransform;
        Matrix4     mCacheMatrix;
        std::string mName;
        Bone*       mParentBone;
        SceneNode*  mAttachedNode;
        
        std::vector<Bone*> mChildren;
        std::map<std::string, std::vector<BoneKeyFrame>> mAnimationFrames;
    };
}

#endif /* PEACH3D_BONE_H */
