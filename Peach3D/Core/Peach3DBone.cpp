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
    void Bone::tranverseChildBone(std::function<void(Bone*)> callFunc)
    {
        for (size_t i=0; i<mChildren.size(); ++i) {
            callFunc(mChildren[i]);
        }
    }
    
    Bone* Bone::findChildByName(const char* name)
    {
        Peach3DAssert(strlen(name) > 0, "Can't find child bone without name");
        Bone* findChild = nullptr;
        if (mChildren.size() > 0) {
            // find node in children
            for (auto child : mChildren) {
                if (child->getName() == name) {
                    findChild = child;
                    break;
                }
            }
            if (!findChild) {
                // find node in children of child
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
}