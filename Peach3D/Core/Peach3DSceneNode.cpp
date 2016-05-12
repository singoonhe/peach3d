//
//  Peach3DSceneNode.cpp
//  TestPeach3D
//
//  Created by singoon.he on 12-10-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DSceneNode.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DMesh.h"
#include "Peach3DIPlatform.h"
#include "Peach3DSceneManager.h"

namespace Peach3D
{
    SceneNode::SceneNode(const Vector3& pos, const Vector3& rotation, const Vector3& scale) : Node()
    {
        init();
        mPosition = pos;
        mRotation = rotation;
        mScale = scale;
    }
    
    void SceneNode::init()
    {
        mScale = Vector3(1.0f, 1.0f, 1.0f);
        mAttachedMesh = nullptr;
        mPickEnabled = false;
        mPickAlways = false;
        mDepthBias = 0.0f;
        mRotateUseVec = false;
        // set default lighting enabled
        mLightEnable = true;
        mIsLightingDirty = true;
        // init skeleton values
        mAnimateTime = 0.f;
        mAnimateLoop = true;
        mAnimateSpeed = 1.f;
        mAnimateTotalTime = 0.f;
        mAnimateFunc = nullptr;
    }
    
    void SceneNode::attachMesh(const MeshPtr& mesh)
    {
        Peach3DAssert(mesh, "Can't attach a null Object to Node");
        mAttachedMesh = mesh;
        // new all render node for objects
        mRenderNodeMap.clear();
        mesh->tranverseObjects([&](const char* name, const ObjectPtr& object) {
            mRenderNodeMap[name] = new RenderNode(mesh->getName(), object);
        });
        bindSkeleton(mesh->getBindSkeleton());
        // init RenderNode render status, so functions can called after attachMesh
        updateRenderState();
        setPickingEnabled(mPickEnabled, mPickAlways);
        setAlpha(mAlpha);
        // default enable lighting
        setLightingEnabled(mLightEnable);
    }
    
    void SceneNode::bindSkeleton(const SkeletonPtr& skel)
    {
        Peach3DAssert(skel, "Can't bind a null skeleton to Node");
        mBindSkeleton = skel;
        for (auto node : mRenderNodeMap) {
            node.second->bindSkeleton(skel);
        }
    }
    
    void SceneNode::unbindSkeleton()
    {
        mBindSkeleton = nullptr;
        for (auto node : mRenderNodeMap) {
            node.second->unbindSkeleton();
        }
    }
    
    void SceneNode::runAnimate(const char* name, bool loop)
    {
        Peach3DAssert(mBindSkeleton, "No skeleton bind");
        mAnimateTotalTime = mBindSkeleton->getAnimateTime(name);
        if (mBindSkeleton && mAnimateTotalTime > FLT_EPSILON) {
            mAnimateName = name;
            mAnimateLoop = loop;
            mAnimateTime = 0.f;
        }
    }
    
    RenderNode* SceneNode::getRenderNode(const char* name)
    {
        if (mRenderNodeMap.find(name) != mRenderNodeMap.end()) {
            return mRenderNodeMap[name];
        }
        else {
            Peach3DLog(LogLevel::eError, "Can't find RenderNode, is attchMesh forget to call?");
            return nullptr;
        }
    }
    
    void SceneNode::updateRenderState()
    {
        // set all RenderNode draw mode
        for (auto node : mRenderNodeMap) {
            node.second->setRenderState(mNodeState);
        }
    }
    
    void SceneNode::setAcceptShadow(bool enable)
    {
        if (enable) {
            Peach3DAssert((mAttachedMesh->getAnyVertexType() & VertexType::Normal) && mLightEnable, "Vertex must contain normal and light should be open");
        }
        mNodeState.isAcceptShadow = enable;
        updateRenderState();
    }
    
    void SceneNode::setLightingEnabled(bool enable)
    {
        mLightEnable = enable;
        // set used lights need update
        setLightingStateNeedUpdate();
    }
    
    void SceneNode::setPickingEnabled(bool enable, bool always)
    {
        if (enable) {
            // autl generate all OBB for picking
            for (auto node : mRenderNodeMap) {
                node.second->generateOBB();
            }
        }
        mPickEnabled = enable;
        mPickAlways = always;
    }
    
    void SceneNode::setAlpha(float alpha)
    {
        Peach3DAssert(alpha >= 0.f && alpha <= 1.f, "Alpha must clamp in (0, 1)");
        Node::setAlpha(alpha);
        // set all RenderNode alpha
        for (auto node : mRenderNodeMap) {
            node.second->setAlpha(alpha);
        }
    }
    
    SceneNode* SceneNode::createChild(const Vector3& pos, const Vector3& rotation, const Vector3& scale)
    {
        SceneNode* newNode = new SceneNode(pos, rotation, scale);
        // add to current scene node
        addChild(newNode);
        return newNode;
    }
    
    SceneNode* SceneNode::createChild(const std::string& name)
    {
        SceneNode* newNode = new SceneNode(name);
        // add to current scene node
        addChild(newNode);
        return newNode;
    }
    
    void SceneNode::setPosition(const Vector3& pos)
    {
        if (mPosition != pos) {
            mPosition = pos;
            // also update children matrix
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector3& SceneNode::getPosition(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldPosition;
        }
        else {
            return mPosition;
        }
    }
    
    void SceneNode::setRotation(const Vector3& rotation, TranslateRelative type)
    {
        if (type == TranslateRelative::eLocal) {
            if (mRotation != rotation) {
                mRotateUseVec = true;
                mRotation = rotation;
                // also update children matrix
                setNeedUpdateRenderingAttributes();
            }
        }
        else {
            // TODO: calc new position
        }
    }
    
    Vector3 SceneNode::getRotation()
    {
        if (!mRotateUseVec) {
            // convert Quaternion to Vector3
            return mRotateQuat.getEulerAngle();
        }
        else {
            return mRotation;
        }
    }

    void SceneNode::setRotateQuaternion(const Quaternion& quat, TranslateRelative type)
    {
        if (type == TranslateRelative::eLocal) {
            if (mRotateQuat != quat) {
                mRotateUseVec = false;
                mRotateQuat = quat;
                // also update children matrix
                setNeedUpdateRenderingAttributes();
            }
        }
        else {
            Quaternion curQuat = quat;
            setRotation(curQuat.getEulerAngle(), type);
        }
    }

    Quaternion SceneNode::getRotateQuaternion()
    {
        if (!mRotateUseVec) {
            return mRotateQuat;
        }
        else {
            // convert Vector3 to Quaternion
            return Quaternion::createByEulerAngle(mRotation);
        }
    }

    void SceneNode::setScale(const Vector3& scale)
    {
        if (mScale != scale) {
            if (FLOAT_EQUAL_0(scale.x) || FLOAT_EQUAL_0(scale.y) || FLOAT_EQUAL_0(scale.z)) {
                Peach3DLog(LogLevel::eWarn, "Node set scale 0 will draw nothing");
            }
            mScale = scale;
            // also update children matrix
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector3& SceneNode::getScale(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldScale;
        }
        else {
            return mScale;
        }
    }
    
    void SceneNode::setLightingStateNeedUpdate()
    {
        mIsLightingDirty = true;
        // auto set children lighting state update
        for (auto cn : mChildNodeList) {
            static_cast<SceneNode*>(cn)->setLightingStateNeedUpdate();
        }
    }
    
    void SceneNode::tranverseRenderNode(std::function<void(const char*, RenderNode*)> callFunc)
    {
        for (auto iter=mRenderNodeMap.begin(); iter!=mRenderNodeMap.end(); ++iter) {
            // tranverse all child with param func
            callFunc(iter->first.c_str(), iter->second);
        }
    }
    
    bool SceneNode::isLightShineNode(const LightPtr& l)
    {
        bool isShine = true;
        // light can't be ignored
        for (auto igL : mIgnoreLights) {
            if (igL == l) {
                return false;
            }
        }
        if (l->getType() != LightType::eDirection) {
            // calc light accumulate for node, ignore light if too far
            auto lenVector = mWorldPosition - l->getPosition();
            float calLen = lenVector.length();
            auto attenV = l->getAttenuate();
            float accuAtten = attenV.x + attenV.y * calLen + attenV.z * calLen * calLen;
            if (accuAtten > 100.f) {
                isShine = false;
            }
        }
        return isShine;
    }
    
    void SceneNode::prepareForRender(float lastFrameTime)
    {
        Node::prepareForRender(lastFrameTime);
        
        // update animate time
        bool isAnimate = (mAnimateName.length() > 0 && lastFrameTime > FLT_EPSILON);
        if (isAnimate) {
            // calc current animate running time
            mAnimateTime += lastFrameTime * mAnimateSpeed;
            // repeat animate or call over function
            if (mAnimateTime > mAnimateTotalTime) {
                if (mAnimateLoop) {
                    mAnimateTime -= mAnimateTotalTime;
                }
                else if (mAnimateFunc) {
                    stopActions();
                    mAnimateFunc();
                }
            }
        }
        
        // update all RenderNode
        if (isNeedRender()) {
            for (auto node : mRenderNodeMap) {
                node.second->prepareForRender(lastFrameTime);
                if (isAnimate) {
                    //node.second->setAnimateTime(mAnimateTime);
                }
            }
        }
    }
    
    void SceneNode::updateRenderingAttributes(float lastFrameTime)
    {
        if (mIsRenderDirty) {            
            // update world position and world scale
            mWorldPosition = mPosition;
            mWorldScale = mScale;
            mWorldRotation = mRotation;
            SceneNode* parent = static_cast<SceneNode*>(mParentNode);
            SceneNode* rootNode = SceneManager::getSingletonPtr()->getRootSceneNode();
            if (parent && parent != rootNode) {
                mWorldPosition = mWorldPosition + parent->getPosition(TranslateRelative::eWorld);
                mWorldScale = mWorldScale * parent->getScale(TranslateRelative::eWorld);
            }
            
            Matrix4 rotateMatrix;
            // update matrix. Sequence: scale, rotation, translation
            Matrix4 scaleMat = Matrix4::createScaling(mWorldScale.x, mWorldScale.y, mWorldScale.z);
            Matrix4 translateMat = Matrix4::createTranslation(mWorldPosition.x, mWorldPosition.y, mWorldPosition.z);
            if (mRotateUseVec) {
                rotateMatrix = Matrix4::createRotationPitchYawRoll(mRotation.x, mRotation.y, mRotation.z);
            }
            else {
                rotateMatrix = Matrix4::createRotationQuaternion(mRotateQuat);
            }
            Matrix4 modelMatrix = translateMat * rotateMatrix * scaleMat;
            // set model matrix to all child RenderNode
            for (auto node : mRenderNodeMap) {
                node.second->setModelMatrix(modelMatrix);
                OBB* renderOBB = node.second->getRenderOBB();
                if (renderOBB) {
                    renderOBB->setModelMatrix(translateMat, rotateMatrix, scaleMat);
                }
            }
            
            mIsRenderDirty = false;
        }
        
        if (mIsLightingDirty && mAttachedMesh) {
            std::vector<LightPtr> validLights, shadowLights;
            // is attached mesh contain vertex normal
            bool isNormal = bool(mAttachedMesh->getAnyVertexType() & VertexType::Normal);
            if (mLightEnable && isNormal) {
                // save enabled lights name
                SceneManager::getSingleton().tranverseLights([&](const std::string& name, const LightPtr& l){
                    if (isLightShineNode(l)) {
                        validLights.push_back(l);
                        // is shadow valid
                        if (isAcceptShadow() && l->getShadowTexture()) {
                            shadowLights.push_back(l);
                        }
                    }
                }, true);
            }
            // set all RenderNode lights
            for (auto node : mRenderNodeMap) {
                node.second->setRenderLights(validLights, shadowLights);
            }
            mIsLightingDirty = false;
        }
    }
    
    RenderNode* SceneNode::isRayIntersect(const Ray& ray)
    {
        if (mRenderNodeMap.size() > 0) {
            updateRenderingAttributes(0.0f);
            
            // check all RenderNode
            for (auto node : mRenderNodeMap) {
                if (node.second->isRayIntersect(ray)) {
                    return node.second;
                }
            }
        }
        return nullptr;
    }
    
    SceneNode::~SceneNode()
    {
        // release all render node
        for (auto rn : mRenderNodeMap) {
            delete rn.second;
        }
        mRenderNodeMap.clear();
    }
}
