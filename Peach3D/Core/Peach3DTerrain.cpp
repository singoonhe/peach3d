//
//  Peach3DTerrain.cpp
//  Peach3DLib
//
//  Created by singoon.he on 09/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#include "Peach3DTerrain.h"
#include "Peach3DIRender.h"
#include "Peach3DSceneManager.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    Terrain::Terrain(int width, int height, float pace, const float* data)
    {
        mWidthCount = width;
        mHeightCount = height;
        mPerPace = pace;
        mTerrainLength = Vector3(mWidthCount * mPerPace, 0, mHeightCount * mPerPace);
        mHighData = (float*)malloc(sizeof(data));
        memcpy(mHighData, data, sizeof(data));
        // set light enable default
        mIsLightingDirty = true;
        mAcceptShadow = true;
        mLightEnable = true;
    }
    
    Terrain::~Terrain()
    {
        if (mHighData) {
            delete mHighData;
            mHighData = nullptr;
        }
    }
    
    Terrain* Terrain::create(int width, int height, const float* data, float pace, const std::vector<TexturePtr>& map, const std::vector<TexturePtr>& texl)
    {
        Peach3DAssert(texl.size() <= (map.size() * 4), "Each 4 brushs need a map texture");
        if (width > 0 && height > 0 && pace > 0 && data && texl.size() <= (map.size()*4)) {
            Terrain* newT = new Terrain(width, height, pace, data);
            newT->buildTerrain(map, texl);
            return newT;
        }
        return nullptr;
    }
    
    void Terrain::buildTerrain(const std::vector<TexturePtr>& map, const std::vector<TexturePtr>& texl)
    {
        mBrushes = texl;
        mAlphaMap = map;
        
        // create texture
        mTerrainObj = IRender::getSingleton().createObject(("pd_Terrain_"+mName).c_str());
        uint vType = VertexType::Point3|VertexType::Normal|VertexType::UV;
        auto strideSize = IObject::getVertexStrideSize(vType);
        auto strideFloatSize = strideSize / 4;
        auto dataSize = mWidthCount * mHeightCount * strideSize;
        float* vertexData = (float*)malloc(dataSize);
        for (auto i=0; i<mHeightCount; ++i) {
            for (auto j=0; j<mWidthCount; ++j) {
                auto curIndex = i * mWidthCount + j;
                uint curStart = curIndex * strideFloatSize;
                // point
                vertexData[curStart] = mPerPace * i;
                vertexData[curStart + 1] = mHighData[curIndex];
                vertexData[curStart + 2] = -mPerPace * j;
                // normal
                vertexData[curStart + 3] = 0.f;
                vertexData[curStart + 4] = 0.f;
                vertexData[curStart + 5] = 0.f;
                // uv
                vertexData[curStart + 6] = j * 1.0 / mWidthCount;
                vertexData[curStart + 7] = i * 1.0 / mHeightCount;
            }
        }
        // fill vertex data
        mTerrainObj->setVertexBuffer(vertexData, dataSize, vType);
        free(vertexData);
        
        uint indexCount = (mWidthCount - 1) * (mHeightCount - 1) * 6;
        IndexType inxType = IndexType::eUShort;
        auto inxDataSize = indexCount * sizeof(ushort);
        if (indexCount > 65535) {
            inxType = IndexType::eUInt;
            inxDataSize = indexCount * sizeof(uint);
        }
        void* inxData = malloc(inxDataSize);
        for (auto i=0; i<(mHeightCount-1); ++i) {
            for (auto j=0; j<(mWidthCount-1); ++j) {
                uint aboveStart = ((i + 1) * mWidthCount + j), curStart = i * mWidthCount + j;
                uint aboveNext = aboveStart + 1, curNext = curStart + 1;
                uint inxStart = curStart * 6;
                // fill two trangles(0,1,3, 3,1,2)
                if (inxType == IndexType::eUShort) {
                    ushort* fillData = (ushort*)inxData;
                    fillData[inxStart] = curStart;
                    fillData[inxStart + 1] = aboveStart;
                    fillData[inxStart + 2] = curNext;
                    fillData[inxStart + 3] = curNext;
                    fillData[inxStart + 4] = aboveStart;
                    fillData[inxStart + 5] = aboveNext;
                }
                else {
                    uint* fillData = (uint*)inxData;
                    fillData[inxStart] = curStart;
                    fillData[inxStart + 1] = aboveStart;
                    fillData[inxStart + 2] = curNext;
                    fillData[inxStart + 3] = curNext;
                    fillData[inxStart + 4] = aboveStart;
                    fillData[inxStart + 5] = aboveNext;
                }
            }
        }
        for (auto i=0; i<indexCount; i+=3) {
            Vector3 v1, v2;
            if (inxType == IndexType::eUShort) {
                ushort* fillData = (ushort*)inxData;
                auto Index0 = fillData[i] * strideFloatSize;
                auto Index1 = fillData[i + 1] * strideFloatSize;
                auto Index2 = fillData[i + 2] * strideFloatSize;
                Vector3 v0 = Vector3(vertexData[Index0], vertexData[Index0+1], vertexData[Index0+2]);
                v1 = Vector3(vertexData[Index1], vertexData[Index1+1], vertexData[Index1+2]) - v0;
                v2 = Vector3(vertexData[Index2], vertexData[Index2+1], vertexData[Index2+2]) - v0;
                // calc face normal
                Vector3 cn = v1.cross(v2);
                cn.normalize();
                vertexData[Index0] += cn.x; vertexData[Index0+1] += cn.y; vertexData[Index0+2] += cn.z;
                vertexData[Index1] += cn.x; vertexData[Index1+1] += cn.y; vertexData[Index1+2] += cn.z;
                vertexData[Index2] += cn.x; vertexData[Index2+1] += cn.y; vertexData[Index2+2] += cn.z;
            }
            else {
                uint* fillData = (uint*)inxData;
                auto Index0 = fillData[i] * strideFloatSize;
                auto Index1 = fillData[i + 1] * strideFloatSize;
                auto Index2 = fillData[i + 2] * strideFloatSize;
                Vector3 v0 = Vector3(vertexData[Index0], vertexData[Index0+1], vertexData[Index0+2]);
                v1 = Vector3(vertexData[Index1], vertexData[Index1+1], vertexData[Index1+2]) - v0;
                v2 = Vector3(vertexData[Index2], vertexData[Index2+1], vertexData[Index2+2]) - v0;
                // calc face normal
                Vector3 cn = v1.cross(v2);
                cn.normalize();
                vertexData[Index0] += cn.x; vertexData[Index0+1] += cn.y; vertexData[Index0+2] += cn.z;
                vertexData[Index1] += cn.x; vertexData[Index1+1] += cn.y; vertexData[Index1+2] += cn.z;
                vertexData[Index2] += cn.x; vertexData[Index2+1] += cn.y; vertexData[Index2+2] += cn.z;
            }
        }
        // normal normalized
        for (auto i=0; i<mHeightCount; ++i) {
            for (auto j=0; j<mWidthCount; ++j) {
                auto curIndex = i * mWidthCount + j;
                uint curStart = curIndex * strideFloatSize;
                Vector3 on(vertexData[curStart], vertexData[curStart+1], vertexData[curStart+2]);
                on.normalize();
                vertexData[curStart] = on.x; vertexData[curStart+1] = on.y; vertexData[curStart+2] = on.z;
            }
        }
        // fill index data
        mTerrainObj->setIndexBuffer(inxData, inxDataSize, inxType);
        free(inxData);
    }
    
    float Terrain::getCurrentHeight(const Vector3& pos)
    {
        Vector3 maxR(mPerPace * (mWidthCount - 1), 0, mPerPace * (mHeightCount - 1));
        Vector3 offset = pos - mTerrainPos;
        if (offset.x >= 0 && offset.x <= maxR.x && offset.z >= 0 && offset.z <= maxR.z ) {
            float widthIndex = offset.x / mPerPace, heightIndex = offset.z / mPerPace;
            int widthInt = (int)widthIndex, heightInt = (int)heightIndex;
            float widthDec = widthIndex - widthInt, heightDec = heightIndex - heightInt;
            // calc 4 corners high data
            uint aboveStart = ((heightInt + 1) * mWidthCount + widthInt), curStart = heightInt * mWidthCount + widthInt;
            uint aboveNext = aboveStart + 1, curNext = curStart + 1;
            bool isOverflow = (heightInt >= mHeightCount) || (widthInt >= mWidthCount);
            float curStartHigh = mHighData[curStart];
            float aboveNextHigh = isOverflow ? curStartHigh : mHighData[aboveNext];
            // linear calc current high
            if (widthDec > heightDec) {
                float curNextHigh = isOverflow ? curStartHigh : mHighData[curNext];
                return curNextHigh + (aboveNextHigh - curNextHigh) * heightDec;
            }
            else {
                float aboveStartHigh = isOverflow ? curStartHigh : mHighData[aboveStart];
                return aboveStartHigh + (aboveNextHigh - aboveStartHigh) * widthDec;
            }
        }
        else {
            return 0.f;
        }
    }
    
    void Terrain::prepareForRender(float lastFrameTime)
    {
        if (mIsLightingDirty) {
            mRenderLights.clear();
            mShadowLights.clear();
            if (mLightEnable) {
                // save enabled lights name
                SceneManager::getSingleton().tranverseLights([&](const std::string& name, const LightPtr& l){
                    if (l->isIlluminePos(mTerrainPos, mIgnoreLights) ||
                        l->isIlluminePos(mTerrainPos + Vector3(mTerrainLength.x, 0, 0), mIgnoreLights) ||
                        l->isIlluminePos(mTerrainPos+ Vector3(0, 0, mTerrainLength.z), mIgnoreLights) ||
                        l->isIlluminePos(mTerrainPos + mTerrainLength, mIgnoreLights)) {
                        mRenderLights.push_back(l);
                        // is shadow valid
                        if (isAcceptShadow() && l->getShadowTexture()) {
                            mShadowLights.push_back(l);
                        }
                    }
                }, true);
            }
            // generate render program
            mRenderProgram = ResourceManager::getSingleton().getPresetProgram({{PROGRAM_FEATURE_POINT3, 1}, {PROGRAM_FEATURE_UV, 1}, {PROGRAM_FEATURE_LIGHT, mRenderLights.size()}, {PROGRAM_FEATURE_SHADOW, mShadowLights.size()}, {PROGRAM_FEATURE_TERRAIN, mBrushes.size()}});
            mIsLightingDirty = false;
        }
    }
}
