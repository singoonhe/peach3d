//
//  Peach3DParticle.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DParticle.h"
#include "Peach3DWidget.h"
#include "Peach3DSceneNode.h"
#include "Peach3DPptLoader.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    /************************************** 2D particle emitter ***************************************/
    
    Particle2D* Particle2D::create(const char* file)
    {
        ulong fileLength = 0;
        uchar *fileData = ResourceManager::getSingleton().getFileData(file, &fileLength);
        if (fileLength > 0 && fileData) {
            Particle2D* resPat = new Particle2D(file);
            if (!PptLoader::pptParticleDataParse(ResourceLoaderInput(fileData, fileLength, resPat))) {
                delete resPat;
                return nullptr;
            }
            // start emit
            resPat->start();
            // release memory data
            free(fileData);
            return resPat;
        }
        return nullptr;
    }
    
    void Particle2D::start()
    {
        for (auto& emitter : mEmitters) {
            emitter.start();
        }
    }
    
    void Particle2D::updateRenderingAttributes(float lastFrameTime)
    {
        Widget* parent = dynamic_cast<Widget*>(mParentNode);
        if (parent) {
            mWorldPos = mPos + parent->getPosition(TranslateRelative::eWorld);
        }
        for (auto& emitter : mEmitters) {
            emitter.update(lastFrameTime, mWorldPos);
        }
    }
    
    /************************************** 3D particle emitter ***************************************/
    
    Particle3D* Particle3D::create(const char* file)
    {
        ulong fileLength = 0;
        uchar *fileData = ResourceManager::getSingleton().getFileData(file, &fileLength);
        if (fileLength > 0 && fileData) {
            Particle3D* resPat = new Particle3D(file);
            std::string fileName = file;
            std::string fileDir = fileName.substr(0, fileName.rfind('/') == std::string::npos ? 0 : fileName.rfind('/')+1);
            if (!PptLoader::pptParticleDataParse(ResourceLoaderInput(fileData, fileLength, resPat, fileDir.c_str()))) {
                delete resPat;
                return nullptr;
            }
            // release memory data
            free(fileData);
            return resPat;
        }
        return nullptr;
    }
    
    void Particle3D::updateRenderingAttributes(float lastFrameTime)
    {
        SceneNode* parent = dynamic_cast<SceneNode*>(mParentNode);
        if (parent) {
            mWorldPos = mPos + parent->getPosition(TranslateRelative::eWorld);
        }
    }
}
