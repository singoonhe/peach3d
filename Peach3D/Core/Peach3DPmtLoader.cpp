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

namespace Peach3D
{
    bool PmtLoader::pmtMeshDataParse(uchar* orignData, ulong length, const std::string& dir, Mesh* dMesh)
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
                    PmtLoader::objVertexDataParse(nodeEle, dMesh);
                    nodeEle = nodeEle->NextSiblingElement();
                }
                loadRes = true;
            } while (0);
        }
        return loadRes;
    }
    
    void PmtLoader::objVertexDataParse(const XMLElement* objEle, Mesh* dMesh)
    {
        auto objName = objEle->Attribute("name");
        // read vertex type element
        auto vTypeEle = objEle->FirstChildElement();
        uint verType = (uint)atoi(vTypeEle->GetText());
        
        // Object must have name and Type must include Point3 attribute
        if (objName && (verType & VertexType::Point3)) {
            // create IObject
            IObject* obj = dMesh->createObject(objName);
            do {
                // read vertexes count
                auto countEle = vTypeEle->NextSiblingElement();
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
                
                // read indexes count
                auto indexCountEle = verDataEle->NextSiblingElement();
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
                
                // read material data
                PmtLoader::objMaterialDataParse(inxDataEle, obj);
            } while (0);
        }
    }
    
    void PmtLoader::objMaterialDataParse(const XMLElement* parentEle, IObject* obj)
    {
    }
}