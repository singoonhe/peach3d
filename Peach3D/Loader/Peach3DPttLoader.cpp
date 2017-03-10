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
            auto paceEle = rootEle->FirstChildElement("LandPace");
            float landPace = atof(paceEle->GetText());
            // used textures
            std::vector<TexturePtr> texl;
            auto texlEle = rootEle->FirstChildElement("Brushs");
            auto texEle = texlEle->FirstChildElement();
            std::vector<float> detailList;
            while (texEle) {
                std::string texName = texEle->GetText();
                // parse texture
                auto brushTex = ResourceManager::getSingleton().addTexture(texName.c_str());
                brushTex->setWrap(TextureWrap::eRepeat);
                texl.push_back(brushTex);
                // parse texture detail size(texture2D(tex, uv * detail))
                float detail = 35.f;
                texEle->QueryFloatAttribute("detail", &detail);
                detailList.push_back(detail);
                texEle = texEle->NextSiblingElement();
            }
            // used alpha map texture
            std::vector<TexturePtr> mapTex;
            auto alphaEle = rootEle->FirstChildElement("AlphaMaps");
            auto mapTexEle = alphaEle->FirstChildElement();
            while (mapTexEle) {
                std::string texName = mapTexEle->GetText();
                mapTex.push_back(ResourceManager::getSingleton().addTexture(texName.c_str()));
                mapTexEle = mapTexEle->NextSiblingElement();
            }
            // high data
            auto highTexEle = rootEle->FirstChildElement("HighTexture");
            if (highTexEle) {
                // read high ratio
                float highRatio = 1.f;
                auto ratioEle = rootEle->FirstChildElement("HighRatio");
                if (ratioEle) {
                    highRatio = atof(ratioEle->GetText());
                }
                // use high texture
                ulong texLength = 0;
                const char* highName = highTexEle->GetText();
                uchar *texData = ResourceManager::getSingleton().getFileData(highName, &texLength);
                if (texLength > 0 && texData) {
                    TextureLoaderRes* res = ResourceManager::getSingleton().parseImageData(texData, (uint)texLength);
                    Peach3DInfoLog("Load high image %s success, with:%d height:%d", highName, res->width, res->height);
                    // convert byte data to float
                    auto widthByte = res->width * 3;
                    widthByte += 3 - ((widthByte-1) % 4);
                    float* highBuffer = new float[res->width * res->height];
                    uchar* texBuffer = (uchar*)res->buffer;
                    for (auto i=0; i<res->height; ++i) {
                        for (auto j=0; j<res->width; ++j) {
                            highBuffer[i*res->width+j] = highRatio * texBuffer[i*widthByte+j*3];
                        }
                    }
                    // create terrain
                    loadTer = Terrain::create(res->width, res->height, highBuffer, landPace, mapTex, texl);
                    // release memory data
                    delete res;
                    delete[] highBuffer;
                    free(texData);
                }
            }
            else {
                // use high data
                auto countEle = rootEle->FirstChildElement("HighCount");
                int highCount = 0, widthCount = 0;
                sscanf(countEle->GetText(), "%d,%d", &widthCount, &highCount);
                int totalCount = widthCount * highCount;
                // height data
                float* highData = new float[totalCount];
                auto dataEle = rootEle->FirstChildElement("HighData");
                auto splitData = Utils::split(dataEle->GetText(), ',');
                Peach3DAssert(splitData.size()>=totalCount, "high data size not enough");
                for (auto i=0; i<totalCount; ++i) {
                    highData[i] = atof(splitData[i].c_str());
                }
                // create terrain
                loadTer = Terrain::create(widthCount, highCount, highData, landPace, mapTex, texl);
                delete [] highData;
            }
            
            if (loadTer) {
                // set brush detail size
                for (auto i=0; i<detailList.size(); ++i) {
                    loadTer->setBrushDetailSize(i, detailList[i]);
                }
                Peach3DInfoLog("Load terrain file %s success", file);
                loadTer->setPosition(originPos);
            }
            loadState = loadTer;
        } while (0);
        
        // release memory data
        if (fileData) {
            free(fileData);
        }
        return loadState ? loadTer : nullptr;
    }
}
