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
    SceneNode::SceneNode(const Vector3& pos, const Vector3& rotation, const Vector3& scale) : Node(), mMode(DrawMode::eTriangle)
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
        mOBBEnable = false;
        mIsAABBShow = false;
        mPickEnabled = false;
        mPickAlways = false;
        mDepthBias = 0.0f;
        mRotateUseVec = false;
    }
    
    void SceneNode::attachMesh(Mesh* mesh)
    {
        Peach3DAssert(mesh, "Can't attach a null Object to Node");
        mAttachedMesh = mesh;
        // new all render node for objects
        mRenderNodeMap.clear();
        mesh->tranverseObjects([&](const char* name, IObject* object) {
            mRenderNodeMap[name] = new RenderNode(mesh->getName(), object);
        });
        // init RenderNode draw mode
        setDrawMode(mMode);
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
    
    void SceneNode::setDrawMode(DrawMode mode)
    {
        mMode = mode;
        // set all RenderNode draw mode
        for (auto node : mRenderNodeMap) {
            node.second->setDrawMode(mode);
        }
    }
    
    void SceneNode::setOBBEnabled(bool enable)
    {
        mOBBEnable = enable;
        // set all RenderNode draw mode
        for (auto node : mRenderNodeMap) {
            node.second->setOBBEnabled(enable);
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
    
    void SceneNode::tranverseRenderNode(std::function<void(const char*, RenderNode*)> callFunc)
    {
        for (auto iter=mRenderNodeMap.begin(); iter!=mRenderNodeMap.end(); ++iter) {
            // tranverse all child with param func
            callFunc(iter->first.c_str(), iter->second);
        }
    }
    
    void SceneNode::prepareForRender(float lastFrameTime)
    {
        Node::prepareForRender(lastFrameTime);
        
        // update all RenderNode
        for (auto node : mRenderNodeMap) {
            node.second->prepareForRender(lastFrameTime);
        }
    }
    
    void SceneNode::updateRenderingAttributes(float lastFrameTime)
    {
        if (mIsRenderDirty) {
            // reset is need rendering
            mNeedRender = mNeedRender && mAttachedMesh;
            
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
    }
    
    bool SceneNode::isRayIntersect(const Ray& ray, IObject** outObject)
    {
        if (mAttachedMesh) {
            updateRenderingAttributes(0.0f);
            // check mesh objects
            /*
            IObject* interObject = mAttachedMesh->getRayIntersectObjectWithTranslation(mModelMatrix, ray);
            if (interObject) {
                if (outObject) {
                    *outObject = interObject;
                }
                return true;
            }*/
        }
        return false;
    }
}
