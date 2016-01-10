//
//  Peach3DObjLoader.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/26/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include <sstream>
#include "Peach3DUtils.h"
#include "Peach3DObjLoader.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    void ObjLoader::objMeshVertexCountParse(uchar* orignData, std::map<std::string, ObjDataInfo*>& objInfoMap, std::string& mtlFileName,
                                            uint& meshPosCount, uint& meshNormalCount, uint& meshUVCount)
    {
        std::stringstream strData((const char*)orignData);
        
        ObjDataInfo* info = nullptr;
        uint objVertexCount = 0;
        std::string line;
        while (std::getline(strData, line, '\n'))
        {
            // delete the last '\r'
            if (line.rfind('\r')!=std::string::npos) {
                line = line.substr(0, line.size() - 1);
            }
            
            switch (line[0])
            {
                case 'o':
                case '#':
                {
                    std::string name;
                    if (strncmp(line.c_str(), "# object ", 9) == 0) {
                        name = line.substr(9).c_str();
                    }
                    else if (strncmp(line.c_str(), "o ", 2) == 0) {
                        name = line.substr(2).c_str();
                    }
                    // create new ObjDataInfo
                    if (name.size() > 0) {
                        objVertexCount = 0;
                        info = new ObjDataInfo();
                        objInfoMap[name] = info;
                    }
                } break;
                case 'm':
                {
                    // get mtl lib file
                    std::vector<std::string> mtlList = Utils::split(line.c_str(), ' ');
                    if (mtlList[0].compare("mtllib") == 0 && mtlList.size() >= 2)
                    {
                        mtlFileName = mtlList[1];
                    }
                }break;
                case 'u':
                {
                    if (!info) break;
                    // get obj used mtl
                    std::vector<std::string> mtlList = Utils::split(line.c_str(), ' ');
                    if (mtlList[0].compare("usemtl") == 0 && mtlList.size() >= 2)
                    {
                        info->mtlName = mtlList[1];
                    }
                }break;
                case 'v':
                {
                    if (!info) break;
                    switch (line[1])
                    {
                        case 'n':
                            meshNormalCount++;
                            break;
                        case 't':
                            meshUVCount++;
                            break;
                        case ' ':
                            meshPosCount++;
                            break;
                    }
                } break;
                case 'f':
                {
                    if (!info) break;
                    // calc point count and add to object
                    std::stringstream vecStr(line.substr(2));
                    std::string line;
                    int lineCount = 0;
                    while (std::getline(vecStr, line, ' '))
                    {
                        if (info->pointsIndexMap.find(line) == info->pointsIndexMap.end()) {
                            info->pointsIndexMap.insert(std::pair<std::string, uint>(line, objVertexCount));
                            objVertexCount ++;
                            
                            // calc cur vertex type
                            if (!info->vertexType)
                            {
                                info->vertexStep = 3;
                                info->vertexType = VertexType::Point3;
                                if (line.find("//") != std::string::npos) {
                                    info->vertexType |= VertexType::Normal;
                                    info->vertexStep += 3;
                                }
                                else if (line.find('/') != std::string::npos) {
                                    info->vertexType |= VertexType::UV;
                                    info->vertexStep += 2;
                                    if (line.find('/') != line.rfind('/')) {
                                        info->vertexType |= VertexType::Normal;
                                        info->vertexStep += 3;
                                    }
                                }
                            }
                        }
                        lineCount ++;
                    }
                    // save point index count, it may not equal to objVertexCount(point often reuse)
                    info->indexCount += 3;
                    if (lineCount > 3) {
                        info->indexCount += 3;
                    }
                }break;
            }
        }
    }
    
    void ObjLoader::objMeshVertexDataParse(uchar* orignData, std::map<std::string, ObjDataInfo*>& objInfoMap,
                                           uint meshPosCount, uint meshNormalCount, uint meshUVCount,
                                           float** meshPosCache, float** meshNormalCache, float** meshUVCache)
    {
        std::stringstream retryData((const char*)orignData);
        
        ObjDataInfo* info = nullptr;
        uint readPosCount = 0, readNorCount = 0, readUVCount = 0, readIndexCount = 0;
        std::string line;
        while (std::getline(retryData, line, '\n'))
        {
            // delete the last '\r'
            if (line.rfind('\r')!=std::string::npos) {
                line = line.substr(0, line.size() - 1);
            }
            
            switch (line[0])
            {
                case 'o':
                case '#':
                {
                    std::string name;
                    if (strncmp(line.c_str(), "# object ", 9) == 0) {
                        name = line.substr(9).c_str();
                    }
                    else if (strncmp(line.c_str(), "o ", 2) == 0) {
                        name = line.substr(2).c_str();
                    }
                    
                    if (name.size() > 0) {
                        // create object with last data
                        if (info) {
                            info = nullptr;
                            readIndexCount = 0;
                        }
                        // reset current info data
                        if (objInfoMap.find(name) != objInfoMap.end()) {
                            info = objInfoMap[name];
                        }
                    }
                } break;
                case 'v':
                {
                    if (!info) break;
                    switch (line[1])
                    {
                        case 'n':
                            if (readNorCount < meshNormalCount*3) {
                                std::string vecStr = line.substr(3).c_str();
                                float normal[3];
                                sscanf(vecStr.c_str(), "%f %f %f", &normal[0], &normal[1], &normal[2]);
                                memcpy((*meshNormalCache) + readNorCount, normal, sizeof(float) * 3);
                                readNorCount += 3;
                            }
                            break;
                        case 't':
                            if (readUVCount < meshUVCount*3) {
                                std::string vecStr = line.substr(3).c_str();
                                float uv[2];
                                sscanf(vecStr.c_str(), "%f %f", &uv[0], &uv[1]);
                                // upside down tex coord V
                                uv[1] = 1.0f - uv[1];
                                memcpy((*meshUVCache) + readUVCount, uv, sizeof(float) * 2);
                                readUVCount += 2;
                                // set texture warp
                                if (info->texWrap == TextureWrap::eClampToEdge && (uv[0] < (-FLT_EPSILON) || uv[1] < (-FLT_EPSILON) || uv[0] > (1+FLT_EPSILON) || uv[1] > (1+FLT_EPSILON))) {
                                    info->texWrap = TextureWrap::eRepeat;
                                }
                            }
                            break;
                        case ' ':
                            if (readPosCount < meshPosCount*3) {
                                std::string vecStr = line.substr(2).c_str();
                                float pos[3];
                                sscanf(vecStr.c_str(), "%f %f %f", &pos[0], &pos[1], &pos[2]);
                                memcpy((*meshPosCache) + readPosCount, pos, sizeof(float) * 3);
                                readPosCount += 3;
                            }
                            break;
                    }
                } break;
                case 'f':
                {
                    if (!info) break;
                    std::vector<std::string> indexList = Utils::split(line.substr(2), ' ');
                    if (indexList.size() > 2) {
                        // save index data
                        if (info->indexType == IndexType::eUShort) {
                            *((ushort*)info->indexCache + readIndexCount + 0) = (ushort)info->pointsIndexMap[indexList[0]];
                            *((ushort*)info->indexCache + readIndexCount + 1) = (ushort)info->pointsIndexMap[indexList[1]];
                            *((ushort*)info->indexCache + readIndexCount + 2) = (ushort)info->pointsIndexMap[indexList[2]];
                            readIndexCount += 3;
                            if (indexList.size() > 3) {
                                *((ushort*)info->indexCache + readIndexCount + 0) = (ushort)info->pointsIndexMap[indexList[0]];
                                *((ushort*)info->indexCache + readIndexCount + 1) = (ushort)info->pointsIndexMap[indexList[2]];
                                *((ushort*)info->indexCache + readIndexCount + 2) = (ushort)info->pointsIndexMap[indexList[3]];
                                readIndexCount += 3;
                            }
                        }
                        else {
                            *((uint*)info->indexCache + readIndexCount + 0) = (uint)info->pointsIndexMap[indexList[0]];
                            *((uint*)info->indexCache + readIndexCount + 1) = (uint)info->pointsIndexMap[indexList[1]];
                            *((uint*)info->indexCache + readIndexCount + 2) = (uint)info->pointsIndexMap[indexList[2]];
                            readIndexCount += 3;
                            if (indexList.size() > 3) {
                                *((uint*)info->indexCache + readIndexCount + 0) = (uint)info->pointsIndexMap[indexList[0]];
                                *((uint*)info->indexCache + readIndexCount + 1) = (uint)info->pointsIndexMap[indexList[2]];
                                *((uint*)info->indexCache + readIndexCount + 2) = (uint)info->pointsIndexMap[indexList[3]];
                                readIndexCount += 3;
                            }
                        }
                    }
                }break;
            }
        }
    }
    
    bool ObjLoader::objMeshDataParse(uchar* orignData, ulong length, const std::string& dir, Mesh* dMesh)
    {
        std::string mtlFileName;
        uint meshPosCount = 0, meshNormalCount = 0, meshUVCount = 0;
        std::map<std::string, ObjDataInfo*> objInfoMap;
        // parse pos/normal/uv counts
        objMeshVertexCountParse(orignData, objInfoMap, mtlFileName, meshPosCount, meshNormalCount, meshUVCount);
        
        // not have object or count not valid, return false
        if (objInfoMap.size() == 0 && meshPosCount > 0 && objInfoMap[0]->pointsIndexMap.size() > 0) {
            return false;
        }
        
        // malloc vertex buffer and index buffer
        for (auto iter : objInfoMap)
        {
            auto pointCount = iter.second->pointsIndexMap.size();
            if (pointCount > 0 && iter.second->indexCount > 0) {
                // create vertex buffer
                iter.second->vertexSize = sizeof(float) * iter.second->vertexStep * uint(pointCount);
                if (iter.second->vertexSize > 0) {
                    iter.second->vertexBuffer = (float*)malloc(iter.second->vertexSize);
                }
                // create index buffer
                if (iter.second->indexCount > 65535) {
                    iter.second->indexType = IndexType::eUInt;
                    iter.second->indexSize = sizeof(uint) * iter.second->indexCount;
                }
                else {
                    iter.second->indexType = IndexType::eUShort;
                    iter.second->indexSize = sizeof(ushort) * iter.second->indexCount;
                }
                if (iter.second->indexSize > 0) {
                    iter.second->indexCache = malloc(iter.second->indexSize);
                }
            }
        }
        float* meshPosCache = nullptr, *meshNormalCache = nullptr, *meshUVCache = nullptr;
        if (meshPosCount > 0) {
            meshPosCache = (float*)malloc(sizeof(float) * meshPosCount * 3);
        }
        if (meshNormalCount > 0) {
            meshNormalCache = (float*)malloc(sizeof(float) * meshNormalCount * 3);
        }
        if (meshUVCount > 0) {
            meshUVCache = (float*)malloc(sizeof(float) * meshUVCount * 2);
        }
        
        // parse pos/normal/uv data
        objMeshVertexDataParse(orignData, objInfoMap, meshPosCount, meshNormalCount, meshUVCount, &meshPosCache, &meshNormalCache, &meshUVCache);
        
        // calc vertex format and index type
        for (auto iter : objInfoMap) {
            std::map<std::string, uint>& pointsIndexMap = iter.second->pointsIndexMap;
            if (pointsIndexMap.size() > 0 && iter.second->indexCount > 0) {
                // create IObject
                IObject* obj = dMesh->createObject(iter.first.c_str());
                
                // fill vertex buffer
                for (auto strPoint : pointsIndexMap) {
                    int curStep = 0;
                    int posIndex = 0, norIndex = 0, uvIndex = 0;
                    if (strPoint.first.find("//") != std::string::npos ) {
                        auto firstPos = strPoint.first.find("//");
                        posIndex = atoi(strPoint.first.substr(0, firstPos).c_str());
                        norIndex = atoi(strPoint.first.substr(firstPos+2).c_str());
                    }
                    else {
                        std::vector<std::string> lineList = Utils::split(strPoint.first, '/');
                        posIndex = atoi(lineList[0].c_str());
                        if (lineList.size() > 1) {
                            uvIndex = atoi(lineList[1].c_str());
                            if (lineList.size() > 2) {
                                norIndex = atoi(lineList[2].c_str());
                            }
                        }
                    }
                    if (posIndex > 0 && meshPosCache && iter.second->vertexBuffer) {
                        memcpy(iter.second->vertexBuffer + strPoint.second*iter.second->vertexStep + curStep, meshPosCache + (posIndex-1)*3, sizeof(float) * 3);
                        curStep += 3;
                    }
                    if (norIndex > 0 && meshNormalCache && iter.second->vertexBuffer) {
                        memcpy(iter.second->vertexBuffer + strPoint.second*iter.second->vertexStep + curStep, meshNormalCache + (norIndex-1)*3, sizeof(float) * 3);
                        curStep += 3;
                    }
                    if (uvIndex > 0 && meshUVCache && iter.second->vertexBuffer) {
                        memcpy(iter.second->vertexBuffer + strPoint.second*iter.second->vertexStep + curStep, meshUVCache + (uvIndex-1)*2, sizeof(float) * 2);
                    }
                }
                // fill vertex buffer and index buffer
                obj->setVertexBuffer((const void*)iter.second->vertexBuffer, iter.second->vertexSize, iter.second->vertexType);
                obj->setIndexBuffer((const void*)iter.second->indexCache, iter.second->indexSize, iter.second->indexType);
            }
        }
        
        // read mtl file
        if (mtlFileName.size() > 0) {
            readMtlFile(mtlFileName, dir, dMesh, objInfoMap);
        }
        
        // free cache memory
        for (auto iter : objInfoMap) {
            delete iter.second;
        }
        if (meshPosCache) {
            free(meshPosCache);
        }
        if (meshNormalCache) {
            free(meshNormalCache);
        }
        if (meshUVCache) {
            free(meshUVCache);
        }
        return true;
    }
    
    void ObjLoader::readMtlFile(const std::string& mtlFile, const std::string& dir, Mesh* dMesh, const std::map<std::string, ObjDataInfo*>& objInfoMap)
    {
        ulong fileLength = 0;
        // get mtl file data
        uchar *fileData = ResourceManager::getSingleton().getFileData(mtlFile.c_str(), &fileLength);
        if ((!fileData || fileLength == 0) && dir.size() > 0) {
            // read mtl from absolute path
            Peach3DInfoLog("Search mtl file in obj dir : %s", dir.c_str());
            fileData = ResourceManager::getSingleton().getFileData((dir+mtlFile).c_str(), &fileLength);
        }
        if (fileLength > 0 && fileData) {
            // split mtl data
            std::vector<std::string> lineList = Utils::split((const char*)fileData, '\n');
            
            Material* currentMtl = nullptr;
            for (auto iter=lineList.begin(); iter!=lineList.end(); ++iter) {
                // delete the last '\r'
                if (iter->rfind('\r')!=std::string::npos) {
                    *iter = iter->substr(0, iter->size() - 1);
                }
                switch ((*iter)[0]) {
                    case 'n':
                    {
                        // set new material
                        std::vector<std::string> mtlList = Utils::split(iter->c_str(), ' ');
                        if (mtlList[0].compare("newmtl") == 0 && mtlList.size() >= 2) {
                            if (currentMtl) {
                                // set material to object
                                setMaterialToObject(currentMtl, dMesh, objInfoMap);
                                
                                delete currentMtl;
                                currentMtl = nullptr;
                            }
                            currentMtl = new Material();
                        }
                    }
                        break;
                    case 'N':
                    {
                        if ((*iter)[1] == 's') {
                            // set material shininess
                            std::string vecStr = (*iter).substr(3).c_str();
                            currentMtl->shininess = (float)atof(vecStr.c_str());
                        }
                    }
                        break;
                    case 'K':
                    {
                        std::string vecStr = (*iter).substr(3).c_str();
                        std::vector<std::string> colorList = Utils::split(vecStr.c_str(), ' ');
                        switch ((*iter)[1]) {
                            case 'a':
                                // set material ambient
                                currentMtl->ambient = Color4((float)atof(colorList[0].c_str()), (float)atof(colorList[1].c_str()), (float)atof(colorList[2].c_str()));
                                break;
                            case 'd':
                                // set material diffuse
                                currentMtl->diffuse = Color4((float)atof(colorList[0].c_str()), (float)atof(colorList[1].c_str()), (float)atof(colorList[2].c_str()));
                                break;
                            case 's':
                                // set material specular
                                currentMtl->specular = Color4((float)atof(colorList[0].c_str()), (float)atof(colorList[1].c_str()), (float)atof(colorList[2].c_str()));
                                break;
                        }
                    }
                        break;
                    case 'm':
                    {
                        std::vector<std::string> mtlList = Utils::split(iter->c_str(), ' ');
                        if (mtlList[0].compare("map_Kd") == 0 && mtlList.size() >= 2) {
                            std::string relativePath = mtlList[1].substr(mtlList[1].rfind('/') == std::string::npos ? 0 : mtlList[1].rfind('/') + 1);
                            ITexture* tex = ResourceManager::getSingleton().addTexture(relativePath.c_str());
                            if (!tex && dir.size() > 0){
                                // read mtl from absolute path
                                tex = ResourceManager::getSingleton().addTexture((dir + relativePath).c_str());
                            }
                            // add texture to material
                            currentMtl->textureList.push_back(tex);
                        }
                    }
                        break;
                }
            }
            // release material
            if (currentMtl) {
                // set material to object
                setMaterialToObject(currentMtl, dMesh, objInfoMap);
                
                delete currentMtl;
                currentMtl = nullptr;
            }
            
            free(fileData);
        }
    }
    
    void ObjLoader::setMaterialToObject(Material* mat, Mesh* dMesh, const std::map<std::string, ObjDataInfo*>& objInfoMap)
    {
        // set material to object
        dMesh->tranverseObjects([&](const char* name, IObject* obj) {
            for (auto iter=objInfoMap.begin(); iter!=objInfoMap.end(); ++iter) {
                // set object material
                if ((*iter).first.compare(name) == 0) {
                    // set texture wrap
                    for (auto tex : mat->textureList) {
                        tex->setWrap((*iter).second->texWrap);
                    }
                    obj->setMaterial(*mat);
                    break ;
                }
            }
        });
    }
}