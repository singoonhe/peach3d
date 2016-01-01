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
    Color4  IObject::mAABBColor = Color4Black;

    IObject::IObject(const char* name) : mObjectName(name), mRenderProgram(nullptr),mIsPresetProgram(true),mVertexBufferSize(0), mIndexBufferSize(0), mVertexDataStride(0), mIndexDataType(IndexType::eUShort)
    {
    }

    void IObject::useProgramForRender(IProgram* program)
    {
        Peach3DAssert(program, "Object could not use a null program!");
        // use new program
        mRenderProgram = program;
        // use custom program, update program no longer
        mIsPresetProgram = false;
    }

    bool IObject::setVertexBuffer(const void* data, uint size, uint type)
    {
        Peach3DAssert((type & VertexTypePosition3) || (type & VertexTypePosition2), "VertexTypePosition must be included in IObject vertex buffer");
        // check vertex type, delete repeated type
        mVertexDataType = type;
        mVertexBufferSize = size;
        mVertexDataStride = 0;
        
        const std::vector<VertexAttrInfo>& infoList = ResourceManager::getVertexAttrInfoList();
        // calculate position stride
        for (auto info=infoList.begin(); info!=infoList.end(); ++info)
        {
            uint typeValue = (*info).type, typeSize = (*info).size;
            if (typeValue & mVertexDataType)
            {
                mVertexDataStride += typeSize;
            }
        }
        
        // check data size
        if (mVertexDataStride >0 && (size % mVertexDataStride) > 0)
        {
            Peach3DLog(LogLevel::eError, "Object %s set vertx failed, type and data size not matched", mObjectName.c_str());
            return false;
        }
        
        // calc AABB
        const float* vData = (float*)data;
        for (size_t i=0; i<(size / mVertexDataStride); ++i)
        {
            // x
            if (vData[0] < mObjectAABB.min.x) {
                mObjectAABB.min.x = vData[0];
            }
            if (vData[0] > mObjectAABB.max.x) {
                mObjectAABB.max.x = vData[0];
            }
            // y
            if (vData[1] < mObjectAABB.min.y) {
                mObjectAABB.min.y = vData[1];
            }
            if (vData[1] > mObjectAABB.max.y) {
                mObjectAABB.max.y = vData[1];
            }
            // z
            if (vData[2] < mObjectAABB.min.z) {
                mObjectAABB.min.z = vData[2];
            }
            if (vData[2] > mObjectAABB.max.z) {
                mObjectAABB.max.z = vData[2];
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
    
    void IObject::addTextureToMaterial(ITexture* texture)
    {
        Peach3DAssert(texture, "Can't add null texture to material");
        if (texture) {
            if (mObjectMtl.textureList.size() < PEACH3D_OBJECT_TEXTURE_COUNT_MAX) {
                mObjectMtl.textureList.push_back(texture);
                // rechoose program when Itexture changed
                if (mIsPresetProgram) {
//                    mRenderProgram = ResourceManager::getSingleton().getObjectPresetProgram(mVertexDataType, mObjectMtl);
                }
            }
            else {
                Peach3DLog(LogLevel::eWarn, "Peach3D only support count %d object textures", PEACH3D_OBJECT_TEXTURE_COUNT_MAX);
            }
        }
    }
    
    void IObject::useIndexTextureToScroll(uint index)
    {
        if (index < mObjectMtl.textureList.size())
        {
            // enable uv scroll and set the texture index affected
            mObjectMtl.UVScrollTexIndex = index;
        }
    }
}