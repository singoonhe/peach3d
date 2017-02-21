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
            // distance between two vertex
            auto paceEle = rootEle->FirstChildElement("PerPace");
            float perPace = atof(paceEle->GetText());
            // used textures
            std::vector<TexturePtr> texl;
            auto texlEle = rootEle->FirstChildElement("Brushs");
            auto texEle = texlEle->FirstChildElement();
            while (texEle) {
                texl.push_back(ResourceManager::getSingleton().addTexture(texEle->GetText()));
                texEle = texEle->NextSiblingElement();
            }
            // used alpha map texture
            std::vector<TexturePtr> mapTex;
            auto alphaEle = rootEle->FirstChildElement("AlphaMaps");
            auto mapTexEle = alphaEle->FirstChildElement();
            while (mapTexEle) {
                mapTex.push_back(ResourceManager::getSingleton().addTexture(mapTexEle->GetText()));
                mapTexEle = mapTexEle->NextSiblingElement();
            }
            // high data
            auto highTexEle = rootEle->FirstChildElement("HighTexture");
            if (highTexEle) {
                // use high texture
                auto highTex = ResourceManager::getSingleton().addTexture(highTexEle->GetText());
                // create terrain
                loadTer = Terrain::create(highTex, perPace, texl, mapTex);
            }
            else {
                // use high data
                auto countEle = rootEle->FirstChildElement("HighCount");
                int highCount = 0, widthCount = 0;
                sscanf(countEle->GetText(), "%d,%d", &widthCount, &highCount);
                int totalCount = widthCount * highCount;
                // height data
                highData = new float[totalCount];
                auto dataEle = rootEle->FirstChildElement("HighData");
                auto splitData = Utils::split(dataEle->GetText(), ',');
                Peach3DAssert(splitData.size()>=totalCount, "high data size not enough");
                for (auto i=0; i<totalCount; ++i) {
                    highData[i] = atof(splitData[i].c_str());
                }
                // create terrain
                loadTer = Terrain::create(widthCount, highCount, highData, perPace, texl, mapTex);
                delete [] highData;
            }
            
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
