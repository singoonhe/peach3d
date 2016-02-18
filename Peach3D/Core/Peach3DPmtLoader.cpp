//
//  Peach3DPmtLoader.cpp
//  Peach3DLib
//
//  Created by singoon.he on 2/16/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include <sstream>
#include "Peach3DPmtLoader.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    bool PmtLoader::pmtMeshDataParse(uchar* orignData, ulong length, const char* dir, Mesh* dMesh)
    {
        bool loadRes = false;
        XMLDocument readDoc;
        if (readDoc.Parse((const char*)orignData, length) == XML_SUCCESS) {
            do {
                XMLElement* meshEle = readDoc.FirstChildElement("Mesh");
                IF_BREAK(!meshEle, "Mesh file format error");
                
                // read scene node element
                XMLElement* nodeEle = meshEle->FirstChildElement();
                while (nodeEle) {
                    PmtLoader::objDataParse(nodeEle, dir, dMesh);
                    nodeEle = nodeEle->NextSiblingElement();
                }
                loadRes = true;
            } while (0);
        }
        return loadRes;
    }
    
    void PmtLoader::objDataParse(const XMLElement* objEle, const char* dir, Mesh* dMesh)
    {
        auto objName = objEle->Attribute("name");
        // read vertex type element
        auto vTypeEle = objEle->FirstChildElement();
        uint verType = (uint)atoi(vTypeEle->GetText());
        
        // Object must have name and Type must include Point3 attribute
        if (objName && (verType & VertexType::Point3)) {
            // create IObject
            IObject* obj = dMesh->createObject(objName);
            // read vertex data
            auto nextEle = PmtLoader::objVertexDataParse(vTypeEle, verType, obj);
            // read index data
            nextEle = PmtLoader::objIndexDataParse(nextEle, obj);
            // read material data
            nextEle = PmtLoader::objMaterialDataParse(nextEle, dir, obj);
        }
    }
    
    const XMLElement* PmtLoader::objVertexDataParse(const XMLElement* prevEle, uint verType, IObject* obj)
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
                if ((verType & VertexType::UV) && (verType & VertexType::Normal)) {
                    sscanf(line.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f,", verOrignData, verOrignData + 1, verOrignData + 2, verOrignData + 3, verOrignData + 4, verOrignData + 5, verOrignData + 6, verOrignData + 7);
                }
                else if (verType & VertexType::UV) {
                    sscanf(line.c_str(), "%f,%f,%f,%f,%f,", verOrignData, verOrignData + 1, verOrignData + 2, verOrignData + 3, verOrignData + 4);
                }
                else if (verType & VertexType::Normal) {
                    sscanf(line.c_str(), "%f,%f,%f,%f,%f,%f,", verOrignData, verOrignData + 1, verOrignData + 2, verOrignData + 3, verOrignData + 4, verOrignData + 5);
                }
                else {
                    sscanf(line.c_str(), "%f,%f,%f,", verOrignData, verOrignData + 1, verOrignData + 2);
                }
                verOrignData = verOrignData + floatCount;
            }
        }
        obj->setVertexBuffer(verData, verDataSize, verType);
        free(verData);
        
        return verDataEle;
    }
    
    const XMLElement* PmtLoader::objIndexDataParse(const XMLElement* prevEle, IObject* obj)
    {
        // read indexes count
        auto indexCountEle = prevEle->NextSiblingElement();
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
    
    const XMLElement* PmtLoader::objMaterialDataParse(const XMLElement* prevEle, const char* dir, IObject* obj)
    {
        Material objMat;
        auto matEle = prevEle->NextSiblingElement();
        // read ambient attribute
        auto ambientEle = matEle->FirstChildElement();
        sscanf(ambientEle->GetText(), "%f,%f,%f", &objMat.ambient.r, &objMat.ambient.g, &objMat.ambient.b);
        // read diffuse attribute
        auto diffuseEle = ambientEle->NextSiblingElement();
        sscanf(diffuseEle->GetText(), "%f,%f,%f", &objMat.diffuse.r, &objMat.diffuse.g, &objMat.diffuse.b);
        // read specular attribute
        auto specularEle = diffuseEle->NextSiblingElement();
        sscanf(specularEle->GetText(), "%f,%f,%f", &objMat.specular.r, &objMat.specular.g, &objMat.specular.b);
        // read shininess attribute
        auto shininessEle = specularEle->NextSiblingElement();
        sscanf(shininessEle->GetText(), "%f", &objMat.shininess);
        // read emissive attribute
        auto emissiveEle = shininessEle->NextSiblingElement();
        sscanf(emissiveEle->GetText(), "%f,%f,%f", &objMat.emissive.r, &objMat.emissive.g, &objMat.emissive.b);
        
        // read texture attribute
        auto textureEle = emissiveEle->NextSiblingElement();
        if (textureEle) {
            auto texFileEle = textureEle->FirstChildElement();
            std::string fileName = texFileEle->GetText();
            ITexture* tex = ResourceManager::getSingleton().addTexture(fileName.c_str());
            if (!tex && strlen(dir) > 0){
                // read texture from absolute path
                tex = ResourceManager::getSingleton().addTexture((dir + fileName).c_str());
            }
            if (tex) {
                // add texture to material
                objMat.textureList.push_back(tex);
                
                // read texture warp
                auto warpUVEle = texFileEle->NextSiblingElement();
                tex->setWrap((TextureWrap)atoi(warpUVEle->GetText()));
            }
        }
        obj->setMaterial(objMat);
        return matEle;
    }
}