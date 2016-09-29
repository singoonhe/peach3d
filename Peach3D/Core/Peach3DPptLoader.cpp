//
//  Peach3DPptLoader.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DPptLoader.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DTypes.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DResourceManager.h"
#include "base64/base64.h"

namespace Peach3D
{
    void* PptLoader::pptParticleDataParse(const ResourceLoaderInput& input)
    {
        bool loadRes = false;
        XMLDocument readDoc;
        if (readDoc.Parse((const char*)input.data, input.length) == XML_SUCCESS) {
            XMLElement* p2dEle = readDoc.FirstChildElement("Particle2D");
            XMLElement* p3dEle = readDoc.FirstChildElement("Particle3D");
            if (p2dEle) {
                Particle2D* p2d = static_cast<Particle2D*>(input.handler);
                XMLElement* emitterEle = p2dEle->FirstChildElement();
                if (emitterEle) {
                    PptLoader::emitter2DDataParse(emitterEle, p2d);
                    emitterEle = emitterEle->NextSiblingElement();
                }
                loadRes = true;
            }
            if (p3dEle) {
                Particle3D* p3d = static_cast<Particle3D*>(input.handler);
                XMLElement* emitterEle = p3dEle->FirstChildElement();
                if (emitterEle) {
                    PptLoader::emitter3DDataParse(emitterEle, p3d);
                    emitterEle = emitterEle->NextSiblingElement();
                }
                loadRes = true;
            }
        }
        return loadRes ? input.handler : nullptr;
    }
    
    void PptLoader::emitter2DDataParse(XMLElement* emitterEle, Particle2D* handler)
    {
        Emitter2D emitter;
        XMLElement* attrEle = emitterEle->FirstChildElement();
        std::string texFile;
        unsigned char* texData = nullptr;
        if (attrEle) {
            auto attrName = attrEle->Name();
            if (strcmp(attrName, "MaxCount") == 0) {
                emitter.maxCount = atoi(attrEle->GetText());
            }
            else if (strcmp(attrName, "Angle") == 0) {
                emitter.emitAngle = DEGREE_TO_RADIANS(atof(attrEle->GetText()));
            }
            else if (strcmp(attrName, "AngleVariance") == 0) {
                emitter.emitAngleVariance = DEGREE_TO_RADIANS(atof(attrEle->GetText()));
            }
            else if (strcmp(attrName, "PosVariance") == 0) {
                sscanf(attrEle->GetText(), "%f,%f", &emitter.emitPosVariance.x, &emitter.emitPosVariance.y);
            }
            else if (strcmp(attrName, "Duration") == 0) {
                emitter.duration = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "StartColor") == 0) {
                sscanf(attrEle->GetText(), "%f,%f,%f,%f", &emitter.startColor.r, &emitter.startColor.g, &emitter.startColor.b, &emitter.startColor.a);
            }
            else if (strcmp(attrName, "StartColorVariance") == 0) {
                sscanf(attrEle->GetText(), "%f,%f,%f,%f", &emitter.startColorVariance.r, &emitter.startColorVariance.g, &emitter.startColorVariance.b, &emitter.startColorVariance.a);
            }
            else if (strcmp(attrName, "EndColor") == 0) {
                sscanf(attrEle->GetText(), "%f,%f,%f,%f", &emitter.endColor.r, &emitter.endColor.g, &emitter.endColor.b, &emitter.endColor.a);
            }
            else if (strcmp(attrName, "EndColorVariance") == 0) {
                sscanf(attrEle->GetText(), "%f,%f,%f,%f", &emitter.endColorVariance.r, &emitter.endColorVariance.g, &emitter.endColorVariance.b, &emitter.endColorVariance.a);
            }
            else if (strcmp(attrName, "Gravity") == 0) {
                sscanf(attrEle->GetText(), "%f,%f", &emitter.gravity.x, &emitter.gravity.y);
            }
            else if (strcmp(attrName, "LifeTime") == 0) {
                emitter.lifeTime = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "LifeTimeVariance") == 0) {
                emitter.lifeTimeVariance = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "Accelerate") == 0) {
                sscanf(attrEle->GetText(), "%f,%f", &emitter.accelerate.x, &emitter.accelerate.y);
            }
            else if (strcmp(attrName, "AccelerateVariance") == 0) {
                sscanf(attrEle->GetText(), "%f,%f", &emitter.accelerateVariance.x, &emitter.accelerateVariance.y);
            }
            else if (strcmp(attrName, "StartRotate") == 0) {
                emitter.startRotate = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "StartRotateVariance") == 0) {
                emitter.startRotateVariance = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "EndRotate") == 0) {
                emitter.endRotate = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "EndRotateVariance") == 0) {
                emitter.endRotateVariance = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "Speed") == 0) {
                emitter.speed = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "SpeedVariance") == 0) {
                emitter.speedVariance = atof(attrEle->GetText());
            }
            else if (strcmp(attrName, "StartSize") == 0) {
                emitter.startSize = atoi(attrEle->GetText());
            }
            else if (strcmp(attrName, "StartSizeVariance") == 0) {
                emitter.startSizeVariance = atoi(attrEle->GetText());
            }
            else if (strcmp(attrName, "EndSize") == 0) {
                emitter.endSize = atoi(attrEle->GetText());
            }
            else if (strcmp(attrName, "EndSizeVariance") == 0) {
                emitter.endSizeVariance = atoi(attrEle->GetText());
            }
            else if (strcmp(attrName, "TextureName") == 0) {
                texFile = attrEle->GetText();
            }
            else if (strcmp(attrName, "TextureData") == 0) {
                texFile = attrEle->GetText();
            }
            attrEle = attrEle->NextSiblingElement();
        }
        if (texFile.size() > 0) {
            if (texData) {
                unsigned char *buffer = nullptr;
                ulong decodeSize = base64Decode(texData, strlen((const char *)texData), &buffer);
                TexturePtr base64Tex = ResourceManager::getSingleton().createTexture((const char*)texFile.size(), buffer, decodeSize);
                if (base64Tex) {
                    emitter.texFrame = TextureFrame(base64Tex);
                }
            }
            else {
                TextureFrame pTexFrame;
                if (ResourceManager::getSingleton().getTextureFrame(texFile.c_str(), &pTexFrame)) {
                    emitter.texFrame = pTexFrame;
                }
            }
        }
    }
    
    void PptLoader::emitter3DDataParse(XMLElement* emitterEle, Particle3D* handler)
    {
    }
}
