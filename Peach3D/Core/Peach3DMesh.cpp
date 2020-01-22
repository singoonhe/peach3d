//
//  Peach3DIMesh.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-6-7.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DMesh.h"
#include "Peach3DIPlatform.h"

namespace Peach3D
{
    ObjectPtr Mesh::createObject(const char* name)
    {
        const char* newName = name;
        if (!newName) {
            // generate mesh name if name==nullptr
            char pName[100] = { 0 };
            static uint meshAutoCount = 0;
            sprintf(pName, "pd_Object%d", meshAutoCount++);
            newName = pName;
        }
        // add Object to mesh
        ObjectPtr childObject = IRender::getSingletonPtr()->createObject(newName, mMeshName.c_str());
        mObjectMap[newName] = childObject;
        return childObject;
    }
    
    ObjectPtr Mesh::getObjectByName(const char* name)
    {
        if (mObjectMap.find(name) != mObjectMap.end()) {
            return mObjectMap[name];
        }
        return nullptr;
    }
    
    void Mesh::tranverseObjects(std::function<void(const char*, const ObjectPtr&)> callFunc)
    {
        for (auto& iter : mObjectMap) {
            // tranverse all child with param func
            callFunc(iter.first.c_str(), iter.second);
        }
    }
    
    uint Mesh::getAnyVertexType()
    {
        if (mObjectMap.size() > 0) {
            return mObjectMap.begin()->second->getVertexType();
        }
        return 0;
    }
    
    Mesh::~Mesh()
    {
        // object will auto release
        mObjectMap.clear();
    }
}
