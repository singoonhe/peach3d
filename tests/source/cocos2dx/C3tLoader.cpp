//
//  C3tLoader.cpp
//  test
//
//  Created by singoon.he on 8/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "C3tLoader.h"
#include "rapidjson/document.h"

using namespace rapidjson;
using namespace std;

struct C3tObjectValue {
    string  objName;
    string  matName;
    Matrix4 objTran;
};

static void c3tObjectDataParse(const Value& object, MeshPtr* mesh, const map<string, C3tObjectValue>& idNames)
{
    const Value& parts = object["parts"];
    for (SizeType i = 0; i < parts.Size(); i++) {
        auto partIdName = parts[i]["id"].GetString();
        string objName;
        for (auto iter = idNames.begin(); iter != idNames.end(); ++iter) {
            if (iter->second.objName.compare(partIdName) == 0) {
                objName = iter->first;
                break;
            }
        }
        Peach3DAssert(objName.size() > 0, "Parse object name error");
        if (objName.size() > 0) {
            ObjectPtr dObj = (*mesh)->createObject(objName.c_str());
            
            // calc vertex type and data size
            uint floatCount = 0, c3tFCount = 0;
            uint verType = 0;
            const Value& vTypeList = object["attributes"];
            for (SizeType j = 0; j < vTypeList.Size(); j++) {
                auto attrString = vTypeList[j]["attribute"].GetString();
                if (strcmp(attrString, "VERTEX_ATTRIB_POSITION") == 0) {
                    floatCount += 3; c3tFCount += 3;
                    verType = verType | VertexType::Point3;
                }
                else if (strcmp(attrString, "VERTEX_ATTRIB_NORMAL") == 0) {
                    floatCount += 3; c3tFCount += 3;
                    verType = verType | VertexType::Normal;
                }
                else if (strcmp(attrString, "VERTEX_ATTRIB_TEX_COORD") == 0) {
                    floatCount += 2; c3tFCount += 2;
                    verType = verType | VertexType::UV;
                }
                else if (strcmp(attrString, "VERTEX_ATTRIB_BLEND_WEIGHT") == 0) {
                    floatCount += 4; c3tFCount += 8;
                    verType = verType | VertexType::Bone;
                }
            }
            
            auto vertexCount = object["vertices"].Size()/c3tFCount;
            // malloc vertex data
            uint verDataSize = floatCount * sizeof(float) * vertexCount;
            float* verData = (float*)malloc(verDataSize);
            
            for (int k = 0; k < vertexCount; k++) {
                for (int m = 0; m < floatCount; m++) {
                    verData[k * floatCount + m] = object["vertices"][k * c3tFCount + m].GetDouble();
                }
            }
            dObj->setVertexBuffer(verData, verDataSize, verType);
            free(verData);
        }
    }
}

void* C3tLoader::c3tMeshDataParse(const ResourceLoaderInput& input)
{
    Document document;
    if (document.ParseInsitu((char*)input.data).HasParseError())
        return nullptr;
    
    map<string, C3tObjectValue> idNameList;
    const Value& nodes = document["nodes"];
    for (SizeType i = 0; i < nodes.Size(); i++) {
        const Value& nodeValue = nodes[i];
        if (nodeValue["skeleton"].GetBool() == false) {
            const Value& nodeParts = nodeValue["parts"][0];
            C3tObjectValue& nodeObj = idNameList[nodeValue["id"].GetString()];
            nodeObj.objName = nodeParts["meshpartid"].GetString();
            nodeObj.matName = nodeParts["materialid"].GetString();
            auto objTranString = nodeValue["transform"].GetString();
            auto transArray = nodeObj.objTran.mat;
            sscanf(objTranString, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", transArray, transArray+1, transArray+2, transArray+3, transArray+4, transArray+5, transArray+6, transArray+7, transArray+8, transArray+9, transArray+10, transArray+11, transArray+12, transArray+13, transArray+14, transArray+15);
        }
    }
    
    MeshPtr* dMesh = (MeshPtr*)input.handler;
    const Value& objects = document["meshes"];
    if (objects.IsArray()) {
        for (SizeType i = 0; i < objects.Size(); i++) {
            c3tObjectDataParse(objects[i], dMesh, idNameList);
        }
        
    }
    return dMesh;
}