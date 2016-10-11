//
//  Peach3DIObject.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-6-7.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DIObject.h"
#include "Peach3DIRender.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    IObject::IObject(const char* name, const char* meshName) : mObjectName(name), mVertexBufferSize(0), mIndexBufferSize(0), mVertexDataStride(0), mIndexDataType(IndexType::eUShort), mObjectProgram(nullptr)
    {
        if (meshName) {
            mUniqueName = meshName + mObjectName;
        }
        else {
            mUniqueName = mObjectName;
        }
    }

    bool IObject::setVertexBuffer(const void* data, uint size, uint type, bool isDynamic)
    {
        Peach3DAssert((type & VertexType::Point3) || (type & VertexType::Point2), "VertexType::Point must be included in IObject vertex buffer");
        // check vertex type, delete repeated type
        mVertexDataType = type;
        mVertexBufferSize = size;
        mVertexDataStride = 0;
        
        const std::vector<VertexAttrInfo>& infoList = ResourceManager::getVertexAttrInfoList();
        // calculate position stride
        for (auto& info : infoList) {
            uint typeValue = info.type, typeSize = info.size;
            if (typeValue & mVertexDataType) {
                mVertexDataStride += typeSize;
            }
        }
        
        // check data size
        if (mVertexDataStride >0 && (size % mVertexDataStride) > 0) {
            Peach3DLog(LogLevel::eError, "Object %s set vertx failed, type and data size not matched", mObjectName.c_str());
            return false;
        }
        
        if (!(mVertexDataType & VertexType::PSprite)) {
            // calc border
            const float* vData = (float*)data;
            for (size_t i=0; i<(size / mVertexDataStride); ++i) {
                // x
                if (vData[0] < mBorderMin.x) {
                    mBorderMin.x = vData[0];
                }
                if (vData[0] > mBorderMax.x) {
                    mBorderMax.x = vData[0];
                }
                // y
                if (vData[1] < mBorderMin.y) {
                    mBorderMin.y = vData[1];
                }
                if (vData[1] > mBorderMax.y) {
                    mBorderMax.y = vData[1];
                }
                // z
                if (vData[2] < mBorderMin.z) {
                    mBorderMin.z = vData[2];
                }
                if (vData[2] > mBorderMax.z) {
                    mBorderMax.z = vData[2];
                }
                vData = vData + mVertexDataStride/sizeof(float);
            }
        }
        return true;
    }
    
    void IObject::setIndexBuffer(const void*data, uint size, IndexType type)
    {
        mIndexBufferSize = size;
        mIndexDataType = type;
    }
}
