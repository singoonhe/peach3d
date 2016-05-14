//
//  Peach3DPmtLoader.cpp
//  Peach3DLib
//
//  Created by singoon.he on 2/16/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include <sstream>
#include "Peach3DUtils.h"
#include "Peach3DPmtLoader.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    bool PmtLoader::pmtMeshDataParse(uchar* orignData, ulong length, const char* dir, const MeshPtr& dMesh)
    {
        bool loadRes = false;
        XMLDocument readDoc;
        if (readDoc.Parse((const char*)orignData, length) == XML_SUCCESS) {
            do {
                XMLElement* meshEle = readDoc.FirstChildElement("Mesh");
                IF_BREAK(!meshEle, "Mesh file format error");
                
                // read scene node element
                XMLElement* nodeEle = meshEle->FirstChildElement();
                if (nodeEle && strcmp(nodeEle->Name(), "Skeleton") == 0) {
                    // load skeleton if exist
                    auto ske = ResourceManager::getSingleton().addSkeleton(nodeEle->GetText());
                    dMesh->bindSkeleton(ske);
                    nodeEle = nodeEle->NextSiblingElement();
                }
                while (nodeEle) {
                    PmtLoader::objDataParse(nodeEle, dir, dMesh);
                    nodeEle = nodeEle->NextSiblingElement();
                }
                loadRes = true;
            } while (0);
        }
        return loadRes;
    }
    
    void PmtLoader::objDataParse(const XMLElement* objEle, const char* dir, const MeshPtr& dMesh)
    {
        auto objName = objEle->Attribute("name");
        // read vertex type element
        auto vTypeEle = objEle->FirstChildElement();
        uint verType = (uint)atoi(vTypeEle->GetText());
        
        // Object must have name and Type must include Point3 attribute
        if (objName && (verType & VertexType::Point3)) {
            // create IObject
            ObjectPtr obj = dMesh->createObject(objName);
            // read vertex data, vertex data must be valid
            auto nextEle = PmtLoader::objVertexDataParse(vTypeEle, verType, obj);
            // read index data, index data must be valid
            nextEle = PmtLoader::objIndexDataParse(nextEle, obj);
            // read material data, could be empty
            nextEle = PmtLoader::objMaterialDataParse(nextEle, dir, obj);
        }
    }
    
    const XMLElement* PmtLoader::objVertexDataParse(const XMLElement* prevEle, uint verType, const ObjectPtr& obj)
    {
        // read vertexes count
        auto countEle = prevEle->NextSiblingElement();
        auto verCount = atoi(countEle->GetText());
        // calc float count per vertex
        uint floatCount = 3;
        if (verType & VertexType::Normal) {
            floatCount += 3;
        }
        if (verType & VertexType::UV) {
            floatCount += 2;
        }
        if (verType & VertexType::Skeleton) {
            floatCount += 4;
        }
        // malloc vertex data
        uint verDataSize = verCount * sizeof(float) * floatCount;
        float* verData = (float*)malloc(verDataSize);
        float* verOrignData = verData;
        // read vertexes data string
        auto verDataEle = countEle->NextSiblingElement();
        std::stringstream verDataText(verDataEle->GetText());
        std::string line;
        while (std::getline(verDataText, line, '\n')) {
            // delete the first '\r'
            if (line.find('\r')!=std::string::npos) {
                line = line.substr(0, line.size() - 1);
            }
            if (line.size() > 4) {
                auto floatStrList = Utils::split(line, ',');
                if (floatStrList.size() >= floatCount) {
                    for (auto i=0; i<floatCount; ++i) {
                        *(verOrignData + i) = atof(floatStrList[i].c_str());
                    }
                }
                verOrignData = verOrignData + floatCount;
            }
        }
        obj->setVertexBuffer(verData, verDataSize, verType);
        free(verData);
        
        return verDataEle;
    }
    
    const XMLElement* PmtLoader::objIndexDataParse(const XMLElement* prevEle, const ObjectPtr& obj)
    {
        // read indexes count
        auto indexCountEle = prevEle->NextSiblingElement("IndexCount");
        if (!indexCountEle) {
            Peach3DErrorLog("Pmt file parse indexes data error");
            return prevEle;
        }
        auto indexCount = atol(indexCountEle->GetText());
        IndexType inxType = IndexType::eUShort;
        auto inxDataSize = indexCount * sizeof(ushort);
        if (indexCount > 65535) {
            inxType = IndexType::eUInt;
            inxDataSize = indexCount * sizeof(uint);
        }
        // malloc vertex data
        void* inxData = malloc(inxDataSize);
        // read indexes data string
        auto inxDataEle = indexCountEle->NextSiblingElement();
        std::stringstream inxDataText(inxDataEle->GetText());
        std::string line;
        auto curIndex = 0;
        while (std::getline(inxDataText, line, ',')) {
            // delete the first '\r'
            if (line.find('\r')!=std::string::npos) {
                line = line.substr(0, line.size() - 1);
            }
            if (line.size() > 1) {
                if (inxType == IndexType::eUShort) {
                    sscanf(line.c_str(), "%hd", (ushort*)(inxData) + curIndex);
                }
                else if (inxType == IndexType::eUInt) {
                    sscanf(line.c_str(), "%u", (uint*)(inxData) + curIndex);
                }
                curIndex ++;
            }
        }
        obj->setIndexBuffer(inxData, (uint)inxDataSize, inxType);
        free(inxData);
        
        return inxDataEle;
    }
    
    const XMLElement* PmtLoader::objMaterialDataParse(const XMLElement* prevEle, const char* dir, const ObjectPtr& obj)
    {
        Material objMat;
        auto matEle = prevEle->NextSiblingElement("Material");
        // material may be empty
        if (!matEle) {
            return prevEle;
        }
        // read ambient attribute
        auto attrEle = matEle->FirstChildElement();
        while (attrEle && attrEle->Name()) {
            const char* eleName = attrEle->Name();
            if (strcmp(eleName, "Ambient") == 0) {
                sscanf(attrEle->GetText(), "%f,%f,%f", &objMat.ambient.r, &objMat.ambient.g, &objMat.ambient.b);
            }
            else if (strcmp(eleName, "Diffuse") == 0) {
                sscanf(attrEle->GetText(), "%f,%f,%f", &objMat.diffuse.r, &objMat.diffuse.g, &objMat.diffuse.b);
            }
            else if (strcmp(eleName, "Specular") == 0) {
                sscanf(attrEle->GetText(), "%f,%f,%f", &objMat.specular.r, &objMat.specular.g, &objMat.specular.b);
            }
            else if (strcmp(eleName, "Shininess") == 0) {
                sscanf(attrEle->GetText(), "%f", &objMat.shininess);
            }
            else if (strcmp(eleName, "Emissive") == 0) {
                sscanf(attrEle->GetText(), "%f,%f,%f", &objMat.emissive.r, &objMat.emissive.g, &objMat.emissive.b);
            }
            else if (strcmp(eleName, "Alpha") == 0) {
                sscanf(attrEle->GetText(), "%f", &objMat.alpha);
            }
            else if (strcmp(eleName, "Texture") == 0) {
                auto texFileEle = attrEle->FirstChildElement();
                std::string fileName = texFileEle->GetText();
                TexturePtr tex = ResourceManager::getSingleton().addTexture(fileName.c_str());
                if (!tex && strlen(dir) > 0){
                    // read texture from absolute path
                    tex = ResourceManager::getSingleton().addTexture((dir + fileName).c_str());
                }
                if (tex) {
                    // add texture to material
                    objMat.textureList.push_back(tex);
                    
                    // read texture warp
                    auto warpUVEle = texFileEle->NextSiblingElement();
                    if (texFileEle) {
                        tex->setWrap((TextureWrap)atoi(warpUVEle->GetText()));
                    }
                }
            }
            
            attrEle = attrEle->NextSiblingElement();
        }
        obj->setMaterial(objMat);
        return matEle;
    }
}