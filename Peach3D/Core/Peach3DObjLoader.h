//
//  Peach3DObjLoader.h
//  Peach3DLib
//
//  Created by singoon.he on 9/26/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_OBJLOADER_H
#define PEACH3D_OBJLOADER_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DMesh.h"

namespace Peach3D
{
    class ObjLoader
    {
        struct ObjDataInfo
        {
            ObjDataInfo() : vertexType(0), vertexStep(0), vertexBuffer(nullptr), vertexSize(0), texWrap(TextureWrap::eClampToEdge),
                indexCache(nullptr), indexCount(0), indexSize(0) {}
            ~ObjDataInfo()
            {
                if (indexCache) {
                    free(indexCache);
                }
                if (vertexBuffer) {
                    free(vertexBuffer);
                }
            }
            std::string mtlName;    // material name
            std::map<std::string, uint> pointsIndexMap; // trangle point indexes string <-> index
            
            uint        vertexType;     // vertex type
            int         vertexStep;     // per vertex step
            float*      vertexBuffer;   // vertex's data buffer
            uint        vertexSize;     // vertex's buffer size
            
            uint        indexCount;     // index count
            void*       indexCache;     // triangle index cache, uint* or ushort*
            IndexType   indexType;      // triangle index type, uint or ushort
            uint        indexSize;      // indexes buffer size
            TextureWrap texWrap;        // texture wrap, decide by uv, clamp default
        };
        
    public:
        // parse .obj mesh data
        static bool objMeshDataParse(uchar* orignData, ulong length, const char* dir, Mesh* dMesh);
        
    private:
        // parse .obj vertex count, objInfoMap need be modified
        static void objMeshVertexCountParse(uchar* orignData, std::map<std::string, ObjDataInfo*>& objInfoMap, std::string& mtlFileName,
                                            uint& meshPosCount, uint& meshNormalCount, uint& meshUVCount);
        // parse .obj vertex data, objInfoMap need be modified
        static void objMeshVertexDataParse(uchar* orignData, std::map<std::string, ObjDataInfo*>& objInfoMap,
                                           uint meshPosCount, uint meshNormalCount, uint meshUVCount,
                                           float** meshPosCache, float** meshNormalCache, float** meshUVCache);
        // load obj mtl file
        static void readMtlFile(const std::string& mtlFile, const std::string& dir, Mesh* dMesh, const std::map<std::string, ObjDataInfo*>& objInfoMap);
        /** Set material to Mesh Object. */
        static void setMaterialToObject(Material* mat, Mesh* dMesh, const std::map<std::string, ObjDataInfo*>& objInfoMap);
    };
}

#endif // PEACH3D_OBJLOADER_H
