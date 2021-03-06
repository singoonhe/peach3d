//
//  C3tLoader.cpp
//  test
//
//  Created by singoon.he on 8/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "C3tLoader.h"
#include "Peach3DResourceManager.h"
#include "Peach3DUtils.h"
#include "rapidjson/document.h"

using namespace rapidjson;
using namespace std;

struct C3tObjectValue {
    string  objName;
    string  matName;
    Matrix4 objTran;
    std::vector<std::string> boneNames;
};

static void c3tObjectDataParse(const Value& object, MeshPtr* mesh, const map<string, C3tObjectValue>& idNames, const map<string, Material>& materials)
{
    const Value& parts = object["parts"];
    for (SizeType i = 0; i < parts.Size(); i++) {
        auto partIdName = parts[i]["id"].GetString();
        string objName, materialName;
        std::vector<std::string> boneNames;
        for (auto iter = idNames.begin(); iter != idNames.end(); ++iter) {
            if (iter->second.objName.compare(partIdName) == 0) {
                objName = iter->first;
                materialName = iter->second.matName;
                boneNames = iter->second.boneNames;
                break;
            }
        }
        if (objName.size() > 0) {
            ObjectPtr dObj = (*mesh)->createObject(objName.c_str());
            
            // calc vertex type and data size
            uint floatCount = 0;
            uint verType = 0;
            uint colorOffset = 0;
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
                    verType = verType | VertexType::BWidget;
                }
                else if (strcmp(attrString, "VERTEX_ATTRIB_BLEND_INDEX") == 0) {
                    floatCount += 4;
                    verType = verType | VertexType::BIndex;
                }
                else if (strcmp(attrString, "VERTEX_ATTRIB_COLOR") == 0) {
                    // not support color, mark offset
                    colorOffset = 4;
                }
            }
            
            const Value& vertexValue = object["vertices"];
            int c3tFCount = floatCount + colorOffset; // mark offset
            auto vertexCount = vertexValue.Size()/c3tFCount;
            // malloc vertex data
            uint verDataSize = floatCount * sizeof(float) * vertexCount;
            float* verData = (float*)malloc(verDataSize);
            // read vertex data
            int c3tIndex = (verType & VertexType::Normal) ? 6 : 3;
            int c3tLastIndex = c3tFCount - c3tIndex - colorOffset;
            for (int k = 0; k < vertexCount; k++) {
                // copy point and normal
                for (int m = 0; m < c3tIndex; m++) {
                    verData[k * floatCount + m] = vertexValue[k * c3tFCount + m].GetDouble();
                }
                // ignore color attribute
                int curIndex = c3tIndex + colorOffset;
                // copy last data (UV and bone)
                for (int m = 0; m < c3tLastIndex; m++) {
                    verData[k * floatCount + c3tIndex + m] = vertexValue[k * c3tFCount + curIndex + m].GetDouble();
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
            
            // set bones
            if (boneNames.size() > 0) {
                dObj->setUsedBones(boneNames);
            }
        }
    }
}

static Bone* c3tBoneDataParse(const Value& boneValue, map<string, Matrix4>& boneMatrixMap, Bone* parent = nullptr)
{
    // read bone name and index
    auto boneName = boneValue["id"].GetString();
    // bone may not used for vertex, also need generate it
    Bone* newBone = new Bone(boneName);
    // read bone pose transform
    Matrix4 orignMat;
    const Value& transformValue = boneValue["transform"];
    for (auto i=0; i<transformValue.Size(); ++i) {
        orignMat.mat[i] = transformValue[i].GetDouble();
    }
    newBone->setPoseTransform(orignMat);
    // set bone animate inverse transform
    newBone->setInverseTransform(boneMatrixMap[boneName]);
    // add new bone to parent
    if (parent) {
        parent->addChild(newBone);
    }
    // fine children
    if (boneValue.HasMember("children")) {
        const Value& childrenValue = boneValue["children"];
        for (auto i=0; i<childrenValue.Size(); ++i) {
            c3tBoneDataParse(childrenValue[i], boneMatrixMap, newBone);
        }
    }
    return newBone;
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
                // set texture wrap UV
                string strWrapU = texValue["wrapModeU"].GetString();
                string strWrapV = texValue["wrapModeV"].GetString();
                if (strWrapU.compare("REPEAT") == 0 || strWrapV.compare("REPEAT") == 0) {
                    loadTex->setWrap(TextureWrap::eRepeat);
                }
            }
        }
    }
    
    
    MeshPtr* dMesh = (MeshPtr*)input.handler;
    SkeletonPtr c3tSkeleton = nullptr;
    map<string, Matrix4> bonesMatrixMap;
    // find object name
    map<string, C3tObjectValue> idNameList;
    const Value& nodes = document["nodes"];
    // read bones invert transform first
    for (SizeType i = 0; i < nodes.Size(); i++) {
        const Value& nodeValue = nodes[i];
        if (nodeValue["skeleton"].GetBool() == false && nodeValue.HasMember("parts")) {
            const Value& nodeParts = nodeValue["parts"];
            string baseName = nodeValue["id"].GetString();
            
            // load every object
            for (SizeType k = 0; k < nodeParts.Size(); ++k) {
                const Value& nodePart = nodeParts[k];
                // object name add index if count is not only one
                string curStringName = (nodeParts.Size() == 1) ? baseName : Utils::formatString("%s%d", baseName.c_str(), k);
                C3tObjectValue& nodeObj = idNameList[curStringName];
                nodeObj.objName = nodePart["meshpartid"].GetString();
                nodeObj.matName = nodePart["materialid"].GetString();
                // read object transform matrix
                const Value& objTranValue = nodeValue["transform"];
                for (int j=0; j<16; j++) {
                    nodeObj.objTran.mat[j] = objTranValue[j].GetDouble();
                }
                // read object bones
                if (nodePart.HasMember("bones")) {
                    const Value& bonesValue = nodePart["bones"];
                    for (int j=0; j<bonesValue.Size(); j++) {
                        // read used bone name
                        auto boneName = bonesValue[j]["node"].GetString();
                        nodeObj.boneNames.push_back(boneName);
                        // read used bone invert transform
                        const Value& boneTranValue = bonesValue[j]["transform"];
                        Matrix4 boneTransform;
                        for (int n=0; n<16; n++) {
                            boneTransform.mat[n] = boneTranValue[n].GetDouble();
                        }
                        bonesMatrixMap[boneName] = boneTransform;
                    }
                }
            }
        }
    }
    // must after inverted transform, save bones tree
    for (SizeType i = 0; i < nodes.Size(); i++) {
        const Value& nodeValue = nodes[i];
        if (nodeValue["skeleton"].GetBool() == true) {
            // read skeleton and fill bones
            c3tSkeleton = ResourceManager::getSingleton().createSkeleton((*dMesh)->getName());
            auto rootBone = c3tBoneDataParse(nodeValue, bonesMatrixMap);
            c3tSkeleton->addRootBone(rootBone);
            // set bone over, cache bones list
            c3tSkeleton->addBonesOver();
        }
    }
    
    // read mesh data
    const Value& objects = document["meshes"];
    if (objects.IsArray()) {
        for (SizeType i = 0; i < objects.Size(); i++) {
            c3tObjectDataParse(objects[i], dMesh, idNameList, materials);
        }
    }
    // read animation data
    if (c3tSkeleton && document.HasMember("animations")) {
        const Value& animsValue = document["animations"];
        for (auto m=0; m<animsValue.Size(); ++m) {
            const Value& animValue = animsValue[m];
            // add animation info
            auto animName = animValue["id"].GetString();
            auto animTime = animValue["length"].GetDouble();
            c3tSkeleton->addAnimateTime(animName, animTime);
            const Value& bonesValue = animValue["bones"];
            for (auto i=0; i<bonesValue.Size(); ++i) {
                const Value& bValue = bonesValue[i];
                Bone* findBone = c3tSkeleton->findBone(bValue["boneId"].GetString());
                if (findBone) {
                    // read all key frames
                    const Value& framesValue = bValue["keyframes"];
                    BoneKeyFrame firstFrame, curFrame;
                    for (auto j=0; j<framesValue.Size(); ++j) {
                        const Value& frameValue = framesValue[j];
                        curFrame = firstFrame;
                        curFrame.time = frameValue["keytime"].GetDouble() * animTime;
                        if (frameValue.HasMember("rotation")) {
                            const Value& rotationValue = frameValue["rotation"];
                            curFrame.rotate.x = rotationValue[0].GetDouble();
                            curFrame.rotate.y = rotationValue[1].GetDouble();
                            curFrame.rotate.z = rotationValue[2].GetDouble();
                            curFrame.rotate.w = rotationValue[3].GetDouble();
                        }
                        if (frameValue.HasMember("scale")) {
                            const Value& scaleValue = frameValue["scale"];
                            curFrame.scale.x = scaleValue[0].GetDouble();
                            curFrame.scale.y = scaleValue[1].GetDouble();
                            curFrame.scale.z = scaleValue[2].GetDouble();
                        }
                        if (frameValue.HasMember("translation")) {
                            const Value& transValue = frameValue["translation"];
                            curFrame.translate.x = transValue[0].GetDouble();
                            curFrame.translate.y = transValue[1].GetDouble();
                            curFrame.translate.z = transValue[2].GetDouble();
                        }
                        if (j == 0) {
                            firstFrame = curFrame;
                        }
                        findBone->addKeyFrame(animName, curFrame);
                    }
                }
            }
        }
        // bind skeleton to mesh
        (*dMesh)->bindSkeleton(c3tSkeleton);
    }
    return dMesh;
}
