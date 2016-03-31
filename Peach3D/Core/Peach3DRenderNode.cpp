//
//  Peach3DRenderNode.cpp
//  Peach3DLib
//
//  Created by singoon.he on 12/31/15.
//  Copyright © 2015 singoon.he. All rights reserved.
//

#include "Peach3DRenderNode.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DUtils.h"
#include "xxhash/xxhash.h"
#include "Peach3DSceneManager.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    RenderNode::RenderNode(const std::string& meshName, IObject* obj) : mRenderObj(obj), mRenderProgram(nullptr), mIsRenderCodeDirty(true), mRenderOBB(nullptr), mOBBEnable(false), mMode(DrawMode::eTriangle)
    {
        // current render obj unique name
        mObjSpliceName = meshName + obj->getName();
        // copy object template material for node
        mMaterial = obj->getMaterial();
        // copy object template program for node if exist
        if (obj->getProgram()) {
            setRenderProgram(obj->getProgram());
        }
    }
    
    RenderNode::~RenderNode()
    {
        if (mRenderOBB) {
            delete mRenderOBB;
            mRenderOBB = nullptr;
        }
    }
        
    void RenderNode::setOBBEnabled(bool enable)
    {
        if (enable) {
            // auto generate OBB for drawing
            generateOBB();
        }
        mOBBEnable = enable;
    }
    
    void RenderNode::generateOBB()
    {
        if (!mRenderOBB) {
            mRenderOBB = new OBB(mRenderObj->getBorderMin(), mRenderObj->getBorderMax());
        }
    }
    
    void RenderNode::resetTextureByIndex(int index, TexturePtr texture)
    {
        if (!texture) {
            return ;
        }
        auto texCount = mMaterial.textureList.size();
        if (index >= 0 && index < int(texCount)) {
            mMaterial.textureList[index] = texture;
            // sign render hash need update
            mIsRenderCodeDirty = true;
        }
        else {
            Peach3DLog(LogLevel::eWarn, "Can't reset invalid texture index %d in material", index);
        }
    }
    
    void RenderNode::prepareForRender(float lastFrameTime)
    {
        if (mIsRenderCodeDirty) {
            // set preset program first if needed
            auto lCount = (int)mRenderLights.size();
            if (!mRenderProgram || (mRenderProgram->getLightsCount() != lCount)) {
                mRenderProgram = ResourceManager::getSingleton().getPresetProgram(PresetProgramFeatures(true, mMaterial.getTextureCount() > 0, lCount));
            }
            // calc render unique hash code
            std::string renderState = Utils::formatString("Name:%sProgram:%uDrawMode:%d", mObjSpliceName.c_str(), mRenderProgram->getProgramId(), (int)mMode);
            for (auto tex : mMaterial.textureList) {
                renderState = renderState + tex->getName();
            }
            for (auto lName : mRenderLights) {
                renderState = renderState + lName;
            }
            mRenderHash = XXH32((void*)renderState.c_str(), (int)renderState.size(), 0);
            
            mIsRenderCodeDirty = false;
        }
    }
}