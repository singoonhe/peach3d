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

namespace Peach3D
{
    class SceneNode;
    class PEACH3D_DLL Bone
    {
    public:
        Bone(const char* name) : mName(name), mAttachedNode(nullptr) {}
        const std::string& getName() { return mName; }
        
        void addChild(Bone* child) { mChildren.push_back(child); }
        void tranverseChildBone(std::function<void(Bone*)> callFunc);
        uint getChildrenCount() { return (uint)mChildren.size(); }
        const std::vector<Bone*>& getChildren() {return mChildren;}
        /** Find child node by name, will iterative search children. */
        Bone* findChildByName(const char* name);
        /** Attach scene node to bone, such as weapon. */
        void attachNode(SceneNode* tn) { mAttachedNode = tn; }
        void detachNode() { mAttachedNode = nullptr; }
        
    private:
        Matrix4     mTransform;
        std::string mName;
        SceneNode*  mAttachedNode;
        
        std::vector<Bone*> mChildren;
    };
}

#endif /* PEACH3D_BONE_H */
