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
    RenderNode::RenderNode(const std::string& meshName, const ObjectPtr& obj) : mRenderObj(obj), mRenderProgram(nullptr), mIsRenderCodeDirty(true), mRenderOBB(nullptr), mIsRenderShadow(false)
    {
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

    void RenderNode::setRenderState(const NodeRenderState& state)
    {
        if (state.mode != mNodeState.mode) {
            mIsRenderCodeDirty = true;
        }
        if (state.OBBEnable) {
            // auto generate OBB for drawing
            generateOBB();
        }
        mNodeState = state;
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
    
    void RenderNode::resetRenderObject(const ObjectPtr& obj)
    {
        mRenderObj = obj;
        // copy object template material for node
        mMaterial = obj->getMaterial();
        // recalc OBB if need
        if (mRenderOBB) {
            delete mRenderOBB;
            mRenderOBB = nullptr;
            generateOBB();
        }
        // need recalc render state
        mIsRenderCodeDirty = true;
    }
    
    void RenderNode::updateSkeletonAnimate()
    {
        if (mAnimateSkeleton) {
            mAnimateSkeleton->fillAnimateBuffer(mAnimateName, mAnimateTime);
        }
    }
    
    void RenderNode::prepareForRender(float lastFrameTime)
    {
        if (mIsRenderCodeDirty) {
            // set preset program first if needed
            auto lCount = (int)mRenderLights.size();
            auto sCount = (int)mShadowLights.size();
            int bCount = mAnimateName.size() > 0 ? int(mRenderObj->getUsedBones().size()) : 0;
            if (!mRenderProgram || (mRenderProgram->getLightsCount() != lCount) || (mRenderProgram->getShadowCount() != sCount)) {
                mRenderProgram = ResourceManager::getSingleton().getPresetProgram({{PROGRAM_FEATURE_POINT3, 1}, {PROGRAM_FEATURE_UV, mMaterial.getTextureCount()}, {PROGRAM_FEATURE_LIGHT, lCount}, {PROGRAM_FEATURE_SHADOW, sCount}, {PROGRAM_FEATURE_BONE, bCount}});
            }
            // calc render unique hash code(Name:Program:DrawMode)
            std::string renderState = Utils::formatString("N:%sP:%uDM:%d", mRenderObj->getUniqueName().c_str(), mRenderProgram->getProgramId(), (int)mNodeState.mode);
            renderState += "MT"; // Material Texture
            for (auto& tex : mMaterial.textureList) {
                renderState = renderState + tex->getName();
            }
            renderState += "RL"; // Render Lights
            for (auto& l : mRenderLights) {
                renderState = renderState + l->getName();
            }
            renderState += "SL"; // Shadow Lights
            for (auto& l : mShadowLights) {
                renderState = renderState + l->getName();
            }
            // animate skeleton
            if (mAnimateName.size() > 0) {
                renderState += "SK"; // Skeleton
                renderState = renderState + Utils::formatString("%s%s%.4f", mAnimateSkeleton->getName().c_str(), mAnimateName.c_str(), mAnimateTime);
            }
            mRenderHash = XXH32((void*)renderState.c_str(), (int)renderState.size(), 0);
            
            mIsRenderCodeDirty = false;
        }
        
        // make object used RTT active
        for (auto& tex : mMaterial.textureList) {
            if (tex->getType() == TextureType::eRTT) {
                tex->setActived(true);
            }
        }
    }
}
