//
//  C3tLoader.cpp
//  test
//
//  Created by singoon.he on 8/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "C3tLoader.h"
#include "Peach3DResourceManager.h"
#include "rapidjson/document.h"

using namespace rapidjson;
using namespace std;

struct C3tObjectValue {
    string  objName;
    string  matName;
    Matrix4 objTran;
};

static void c3tObjectDataParse(const Value& object, MeshPtr* mesh, const map<string, C3tObjectValue>& idNames, const map<string, Material>& materials)
{
    const Value& parts = object["parts"];
    for (SizeType i = 0; i < parts.Size(); i++) {
        auto partIdName = parts[i]["id"].GetString();
        string objName, materialName;
        for (auto iter = idNames.begin(); iter != idNames.end(); ++iter) {
            if (iter->second.objName.compare(partIdName) == 0) {
                objName = iter->first;
                materialName = iter->second.matName;
                break;
            }
        }
        Peach3DAssert(objName.size() > 0, "Parse object name error");
        if (objName.size() > 0) {
            ObjectPtr dObj = (*mesh)->createObject(objName.c_str());
            
            // calc vertex type and data size
            uint floatCount = 0;
            uint verType = 0;
            const Value& vTypeList = object["attributes"];
            for (SizeType j = 0; j < vTypeList.Size(); j++) {
                auto attrString = vTypeList[j]["attribute"].GetString();
                if (strcmp(attrString, "VERTEX_ATTRIB_POSITION") == 0) {
                    floatCount += 3;
                    verType = verType | VertexType::Point3;
                }
                else if (strcmp(attrString, "VERTEX_ATTRIB_NORMAL") == 0) {
                    floatCount += 3;
                    verType = verType | VertexType::Normal;
                }
                else if (strcmp(attrString, "VERTEX_ATTRIB_TEX_COORD") == 0) {
                    floatCount += 2;
                    verType = verType | VertexType::UV;
                }
                else if (strcmp(attrString, "VERTEX_ATTRIB_BLEND_WEIGHT") == 0) {
                    floatCount += 4;
                    verType = verType | VertexType::Bone;
                }
                Peach3DAssert(strcmp(attrString, "VERTEX_ATTRIB_COLOR") != 0, "Not support vertex color");
            }
            
            const Value& vertexValue = object["vertices"];
            int c3tFCount = (verType & VertexType::Bone ) ? (floatCount + 4) : floatCount;
            auto vertexCount = vertexValue.Size()/c3tFCount;
            // malloc vertex data
            uint verDataSize = floatCount * sizeof(float) * vertexCount;
            float* verData = (float*)malloc(verDataSize);
            // read vertex data, c3t use 4 float for BONE, Peach3D use 2 float.
            for (int k = 0; k < vertexCount; k++) {
                int copyCount = (verType & VertexType::Bone) ? (floatCount - 2) : floatCount;
                for (int m = 0; m < copyCount; m++) {
                    verData[k * floatCount + m] = vertexValue[k * c3tFCount + m].GetDouble();
                }
                if (verType & VertexType::Bone) {
                    verData[k * floatCount + copyCount] = vertexValue[k * c3tFCount + copyCount + 2].GetDouble();
                    verData[k * floatCount + copyCount + 1] = vertexValue[k * c3tFCount + copyCount + 3].GetDouble();
                }
            }
            dObj->setVertexBuffer(verData, verDataSize, verType);
            free(verData);
            
            // read index data
            const Value& indexLists = parts[i]["indices"];
            auto indexCount = indexLists.Size();
            IndexType inxType = IndexType::eUShort;
            auto inxDataSize = indexCount * sizeof(ushort);
            if (indexCount > 65535) {
                inxType = IndexType::eUInt;
                inxDataSize = indexCount * sizeof(uint);
            }
            // malloc vertex data
            void* inxData = malloc(inxDataSize);
            for (int j=0; j<indexCount; j++) {
                if (inxType == IndexType::eUShort) {
                    ((ushort*)inxData)[j] = indexLists[j].GetInt();
                }
                else if (inxType == IndexType::eUInt) {
                    ((uint*)inxData)[j] = indexLists[j].GetUint();
                }
            }
            dObj->setIndexBuffer(inxData, (uint)inxDataSize, inxType);
            free(inxData);
            
            // set material
            auto findMat = materials.find(materialName);
            if (materialName.size() > 0 && findMat != materials.end()) {
                dObj->setMaterial(findMat->second);
            }
        }
    }
}

void* C3tLoader::c3tMeshDataParse(const ResourceLoaderInput& input)
{
    Document document;
    if (document.ParseInsitu((char*)input.data).HasParseError())
        return nullptr;
    
    // read all materials
    map<string, Material> materials;
    const Value& matValues = document["materials"];
    for (SizeType i = 0; i < matValues.Size(); i++) {
        const Value& matV = matValues[i];
        Material& readMat = materials[matV["id"].GetString()];
        const Value& ambientV = matV["ambient"];
        readMat.ambient = Color3(ambientV[0].GetDouble(), ambientV[1].GetDouble(), ambientV[2].GetDouble());
        const Value& diffuseV = matV["diffuse"];
        readMat.diffuse = Color3(diffuseV[0].GetDouble(), diffuseV[1].GetDouble(), diffuseV[2].GetDouble());
        const Value& emissiveV = matV["emissive"];
        readMat.emissive = Color3(emissiveV[0].GetDouble(), emissiveV[1].GetDouble(), emissiveV[2].GetDouble());
        const Value& specularV = matV["specular"];
        readMat.specular = Color3(specularV[0].GetDouble(), specularV[1].GetDouble(), specularV[2].GetDouble());
        readMat.shininess = matV["shininess"].GetDouble();
        readMat.alpha = matV["opacity"].GetDouble();
        if (matV.HasMember("textures")) {
            const Value& texValue = matV["textures"][0];
            string texFile = texValue["filename"].GetString();
            auto loadTex = ResourceManager::getSingleton().addTexture((input.dir + texFile).c_str());\
            if (loadTex) {
                readMat.textureList.push_back(loadTex);
            }
            string strWrapU = texValue["wrapModeU"].GetString();
            string strWrapV = texValue["wrapModeV"].GetString();
            if (strWrapU.compare("REPEAT") == 0 || strWrapV.compare("REPEAT") == 0) {
                loadTex->setWrap(TextureWrap::eRepeat);
            }
        }
    }
    
    // find object name
    map<string, C3tObjectValue> idNameList;
    const Value& nodes = document["nodes"];
    for (SizeType i = 0; i < nodes.Size(); i++) {
        const Value& nodeValue = nodes[i];
        if (nodeValue["skeleton"].GetBool() == false) {
            const Value& nodeParts = nodeValue["parts"][0];
            C3tObjectValue& nodeObj = idNameList[nodeValue["id"].GetString()];
            nodeObj.objName = nodeParts["meshpartid"].GetString();
            nodeObj.matName = nodeParts["materialid"].GetString();
            const Value& objTranValue = nodeValue["transform"];
            for (int j=0; j<16; j++) {
                nodeObj.objTran.mat[j] = objTranValue[j].GetDouble();
            }
        }
    }
    
    MeshPtr* dMesh = (MeshPtr*)input.handler;
    const Value& objects = document["meshes"];
    if (objects.IsArray()) {
        for (SizeType i = 0; i < objects.Size(); i++) {
            c3tObjectDataParse(objects[i], dMesh, idNameList, materials);
        }
    }
    return dMesh;
}