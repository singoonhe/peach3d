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
    Terrain::Terrain(int width, int height, float pace, const float* data) : mDrawMode(DrawMode::eTriangle)
    {
        mWidthCount = width;
        mHeightCount = height;
        mLandPace = pace;
        mTerrainLength = Vector3(mWidthCount * mLandPace, 0, mHeightCount * mLandPace);
        auto dataSize = width * height * sizeof(float);
        mHighData = (float*)malloc(dataSize);
        memcpy(mHighData, data, dataSize);
        // set light enable default
        mIsLightingDirty = true;
        mAcceptShadow = true;
        mLightEnable = true;
    }
    
    Terrain::~Terrain()
    {
        if (mHighData) {
            free(mHighData);
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
                // index : leftbottom(0), rightbottom(1), lefttop(2), righttop(3)
                auto curIndex = i * mWidthCount + j;
                uint curStart = curIndex * strideFloatSize;
                // point
                vertexData[curStart] = mLandPace * j;
                vertexData[curStart + 1] = mHighData[curIndex];
                vertexData[curStart + 2] = -mLandPace * i;
                // normal
                vertexData[curStart + 3] = 0.f;
                vertexData[curStart + 4] = 0.f;
                vertexData[curStart + 5] = 0.f;
                // uv
                vertexData[curStart + 6] = j * 1.0 / (mWidthCount-1);
                vertexData[curStart + 7] = i * 1.0 / (mHeightCount-1);
            }
        }
        // fill vertex data
        mTerrainObj->setVertexBuffer(vertexData, dataSize, vType);
        
        uint indexCount = (mWidthCount - 1) * (mHeightCount - 1) * 6;
        IndexType inxType = IndexType::eUShort;
        auto inxDataSize = 0;
        if (indexCount > 65535) {
            inxType = IndexType::eUInt;
            inxDataSize = indexCount * sizeof(uint);
        }
        else {
            inxDataSize = indexCount * sizeof(ushort);
        }
        auto inxIndex = 0;
        void* inxData = malloc(inxDataSize);
        for (auto i=0; i<(mHeightCount-1); ++i) {
            for (auto j=0; j<(mWidthCount-1); ++j) {
                uint aboveStart = (i + 1) * mWidthCount + j, curStart = i * mWidthCount + j;
                uint aboveNext = aboveStart + 1, curNext = curStart + 1;
                // fill two trangles(0,1,2, 2,1,3)
                if (inxType == IndexType::eUShort) {
                    ushort* fillData = (ushort*)inxData;
                    fillData[inxIndex++] = curStart;
                    fillData[inxIndex++] = curNext;
                    fillData[inxIndex++] = aboveStart;
                    fillData[inxIndex++] = aboveStart;
                    fillData[inxIndex++] = curNext;
                    fillData[inxIndex++] = aboveNext;
                }
                else {
                    uint* fillData = (uint*)inxData;
                    fillData[inxIndex++] = curStart;
                    fillData[inxIndex++] = curNext;
                    fillData[inxIndex++] = aboveStart;
                    fillData[inxIndex++] = aboveStart;
                    fillData[inxIndex++] = curNext;
                    fillData[inxIndex++] = aboveNext;
                }
            }
        }
        for (auto i=0; i<indexCount; i+=3) {
            uint index0, index1, index2;
            if (inxType == IndexType::eUShort) {
                ushort* fillData = (ushort*)inxData;
                index0 = fillData[i] * strideFloatSize;
                index1 = fillData[i + 1] * strideFloatSize;
                index2 = fillData[i + 2] * strideFloatSize;
            }
            else {
                uint* fillData = (uint*)inxData;
                index0 = fillData[i] * strideFloatSize;
                index1 = fillData[i + 1] * strideFloatSize;
                index2 = fillData[i + 2] * strideFloatSize;
            }
            Vector3 v0 = Vector3(vertexData[index0], vertexData[index0+1], vertexData[index0+2]);
            Vector3 v1 = Vector3(vertexData[index1], vertexData[index1+1], vertexData[index1+2]) - v0;
            Vector3 v2 = Vector3(vertexData[index2], vertexData[index2+1], vertexData[index2+2]) - v0;
            // calc face normal
            Vector3 cn = v2.cross(v1);
            cn.normalize();
            vertexData[index0+3] += cn.x; vertexData[index0+4] += cn.y; vertexData[index0+5] += cn.z;
            vertexData[index1+3] += cn.x; vertexData[index1+4] += cn.y; vertexData[index1+5] += cn.z;
            vertexData[index2+3] += cn.x; vertexData[index2+4] += cn.y; vertexData[index2+5] += cn.z;
        }
        // normal normalized
        for (auto i=0; i<mHeightCount; ++i) {
            for (auto j=0; j<mWidthCount; ++j) {
                auto curIndex = i * mWidthCount + j;
                uint curStart = curIndex * strideFloatSize;
                Vector3 on(vertexData[curStart+3], vertexData[curStart+4], vertexData[curStart+5]);
                on.normalize();
                vertexData[curStart+3] = on.x; vertexData[curStart+4] = on.y; vertexData[curStart+5] = on.z;
            }
        }
        // fill index data
        mTerrainObj->setIndexBuffer(inxData, inxDataSize, inxType);
        free(vertexData);
        free(inxData);
    }
    
    float Terrain::getCurrentHeight(const Vector3& pos)
    {
        Vector3 maxR(mLandPace * (mWidthCount - 1), 0, mLandPace * (mHeightCount - 1));
        Vector3 offset = pos - mTerrainPos;
        if (offset.x >= 0 && offset.x <= maxR.x && offset.z >= 0 && offset.z <= maxR.z ) {
            float widthIndex = offset.x / mLandPace, heightIndex = offset.z / mLandPace;
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
