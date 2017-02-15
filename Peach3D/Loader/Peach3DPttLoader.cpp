//
//  Peach3DPttLoader.cpp
//  Peach3DLib
//
//  Created by singoon.he on 09/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#include "Peach3DPttLoader.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DUtils.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    Terrain* PttLoader::pttTerrainParse(const char* file)
    {
        bool loadState = false;
        uchar *fileData = nullptr;
        Terrain* loadTer = nullptr;
        float* highData = nullptr;
        uint* uvData = nullptr;
        do {
            ulong fileLength = 0;
            uchar *fileData = ResourceManager::getSingleton().getFileData(file, &fileLength);
            IF_BREAK(!fileData || fileLength == 0, "Can't find terrain file");
            
            // read terrain file data
            XMLDocument readDoc;
            auto docRes = readDoc.Parse((const char*)fileData, fileLength);
            IF_BREAK(docRes != XML_SUCCESS, "Terrain file parse error");
            // parse terrain params
            XMLElement* rootEle = readDoc.FirstChildElement("Terrain");
            IF_BREAK(!rootEle, "Terrain file format error");
            // origin position
            Vector3 originPos;
            auto posEle = rootEle->FirstChildElement("OrignPos");
            sscanf(posEle->GetText(), "%f,%f,%f", &originPos.x, &originPos.y, &originPos.z);
            // vertex count per line
            auto countEle = rootEle->FirstChildElement("OrignPos");
            int highCount = 0, widthCount = 0;
            sscanf(countEle->GetText(), "%f,%f", &widthCount, &highCount);
            int totalCount = widthCount * highCount;
            // distance between two vertex
            auto paceEle = rootEle->FirstChildElement("PerPace");
            float perPace = atof(paceEle->GetText());
            // height data
            highData = new float[totalCount];
            auto dataEle = rootEle->FirstChildElement("HighData");
            auto splitData = Utils::split(dataEle->GetText(), ',');
            Peach3DAssert(splitData.size()>=totalCount, "high data size not enough");
            for (auto i=0; i<totalCount; ++i) {
                highData[i] = atof(splitData[i].c_str());
            }
            // uv data
            uvData = new uint[totalCount];
            auto uvEle = rootEle->FirstChildElement("HighUV");
            auto uvsData = Utils::split(uvEle->GetText(), ',');
            Peach3DAssert(uvsData.size()>=totalCount, "uv data size not enough");
            for (auto i=0; i<totalCount; ++i) {
                uvData[i] = (uint)atoll(uvsData[i].c_str());
            }
            // used textures
            std::vector<TexturePtr> texl;
            auto texlEle = rootEle->FirstChildElement("Brushs");
            auto texEle = texlEle->FirstChildElement();
            while (texEle) {
                texl.push_back(ResourceManager::getSingleton().addTexture(texEle->GetText()));
                texEle = texEle->NextSiblingElement();
            }
            
            // create terrain
            loadTer = Terrain::create(highCount, highCount, perPace, highData, uvData, texl);
            if (loadTer) {
                loadTer->setPosition(originPos);
            }
            loadState = loadTer;
        } while (0);
        
        // release memory data
        if (fileData) {
            free(fileData);
        }
        if (highData) {
            delete[] highData;
        }
        if (uvData) {
            delete[] uvData;
        }
        return loadState ? loadTer : nullptr;
    }
}
