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
    IObject::IObject(const char* name) : mObjectName(name), mVertexBufferSize(0), mIndexBufferSize(0), mVertexDataStride(0), mIndexDataType(IndexType::eUShort), mObjectProgram(nullptr)
    {
    }

    bool IObject::setVertexBuffer(const void* data, uint size, uint type)
    {
        Peach3DAssert((type & VertexType::Point3) || (type & VertexType::Point2), "VertexType::Point must be included in IObject vertex buffer");
        // check vertex type, delete repeated type
        mVertexDataType = type;
        mVertexBufferSize = size;
        mVertexDataStride = 0;
        
        const std::vector<VertexAttrInfo>& infoList = ResourceManager::getVertexAttrInfoList();
        // calculate position stride
        for (auto info : infoList) {
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
        return true;
    }

    void IObject::setIndexBuffer(const void*data, uint size, IndexType type)
    {
        mIndexBufferSize = size;
        mIndexDataType = type;
    }
    
    AABB IObject::getAABB(const Matrix4& transform)
    {
        if (mObjectAABB.isValid()) {
            // calc AABB applied node translate matrix
            const float* mat = &transform.mat[0];
            Vector3 min(mat[12], mat[13], mat[14]), max(mat[12], mat[13], mat[14]);
            if (mat[0] > 0.0f) {
                min.x += mat[0] * mObjectAABB.min.x; max.x += mat[0] * mObjectAABB.max.x;
            }
            else {
                min.x += mat[0] * mObjectAABB.max.x; max.x += mat[0] * mObjectAABB.min.x;
            }
            if (mat[4] > 0.0f) {
                min.x += mat[4] * mObjectAABB.min.y; max.x += mat[4] * mObjectAABB.max.y;
            }
            else {
                min.x += mat[4] * mObjectAABB.max.y; max.x += mat[4] * mObjectAABB.min.y;
            }
            if (mat[8] > 0.0f) {
                min.x += mat[8] * mObjectAABB.min.z; max.x += mat[8] * mObjectAABB.max.z;
            }
            else {
                min.x += mat[8] * mObjectAABB.max.z; max.x += mat[8] * mObjectAABB.min.z;
            }
            if (mat[1] > 0.0f) {
                min.y += mat[1] * mObjectAABB.min.x; max.y += mat[1] * mObjectAABB.max.x;
            }
            else {
                min.y += mat[1] * mObjectAABB.max.x; max.y += mat[1] * mObjectAABB.min.x;
            }
            if (mat[5] > 0.0f) {
                min.y += mat[5] * mObjectAABB.min.y; max.y += mat[5] * mObjectAABB.max.y;
            }
            else {
                min.y += mat[5] * mObjectAABB.max.y; max.y += mat[5] * mObjectAABB.min.y;
            }
            if (mat[9] > 0.0f) {
                min.y += mat[9] * mObjectAABB.min.z; max.y += mat[9] * mObjectAABB.max.z;
            }
            else {
                min.y += mat[9] * mObjectAABB.max.z; max.y += mat[9] * mObjectAABB.min.z;
            }
            if (mat[2] > 0.0f) {
                min.z += mat[2] * mObjectAABB.min.x; max.z += mat[2] * mObjectAABB.max.x;
            }
            else {
                min.z += mat[2] * mObjectAABB.max.x; max.z += mat[2] * mObjectAABB.min.x;
            }
            if (mat[6] > 0.0f) {
                min.z += mat[6] * mObjectAABB.min.y; max.z += mat[6] * mObjectAABB.max.y;
            }
            else {
                min.z += mat[6] * mObjectAABB.max.y; max.z += mat[6] * mObjectAABB.min.y;
            }
            if (mat[10] > 0.0f) {
                min.z += mat[10] * mObjectAABB.min.z; max.z += mat[10] * mObjectAABB.max.z;
            }
            else {
                min.z += mat[10] * mObjectAABB.max.z; max.z += mat[10] * mObjectAABB.min.z;
            }
            // return new AABB
            return AABB(min, max);
        }
        return mObjectAABB;
    }
}