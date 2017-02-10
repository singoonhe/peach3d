//
//  Peach3DTerrain.cpp
//  Peach3DLib
//
//  Created by singoon.he on 09/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#include "Peach3DTerrain.h"
#include "Peach3DIRender.h"

namespace Peach3D
{
    Terrain::Terrain(int count, float pace, const float* data)
    {
        mHighCount = count;
        mPerPace = pace;
        memcpy(mHighData, data, sizeof(data));
    }
    
    Terrain::~Terrain()
    {
        if (mHighData) {
            delete mHighData;
            mHighData = nullptr;
        }
    }
    
    Terrain* Terrain::create(int count, float pace, const float* data, const uint* uvdata, const std::vector<TexturePtr>& texl)
    {
        if (count > 0 && pace > 0 && data && uvdata) {
            Terrain* newT = new Terrain(count, pace, data);
            newT->buildTerrain(uvdata, texl);
            return newT;
        }
        return nullptr;
    }
    
    void Terrain::buildTerrain(const uint* uvdata, const std::vector<TexturePtr>& texl)
    {
        mTerrainObj = IRender::getSingleton().createObject(("pd_Terrain_"+mName).c_str());
        uint vType = VertexType::Point3|VertexType::Normal|VertexType::UV;
        float* vertexData = (float*)malloc(mHighCount * mHighCount * IObject::getVertexStrideSize(vType));
        // fill vertex data
        mTerrainObj->setVertexBuffer(vertexData, sizeof(vertexData), vType);
        free(vertexData);
        
        uint indexCount = (mHighCount - 1) * (mHighCount - 1) * 6;
        IndexType inxType = IndexType::eUShort;
        auto inxDataSize = indexCount * sizeof(ushort);
        if (indexCount > 65535) {
            inxType = IndexType::eUInt;
            inxDataSize = indexCount * sizeof(uint);
        }
        void* inxData = malloc(inxDataSize);
        // fill index data
        mTerrainObj->setIndexBuffer(inxData, inxDataSize, inxType);
        free(inxData);
    }
    
    float Terrain::getCurrentHeight(const Vector3& pos)
    {
        return 0.f;
    }
}
