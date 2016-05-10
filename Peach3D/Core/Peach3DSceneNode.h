//
//  Peach3DSceneNode.h
//  TestPeach3D
//
//  Created by singoon.he on 12-10-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SCENENODE_H
#define PEACH3D_SCENENODE_H

#include "Peach3DNode.h"
#include "Peach3DVector3.h"
#include "Peach3DMatrix4.h"
#include "Peach3DQuaternion.h"
#include "Peach3DMesh.h"
#include "Peach3DRenderNode.h"

namespace Peach3D
{
    class PEACH3D_DLL SceneNode : public Node
    {
    public:
        //! attach Object, save object materials and skeleton
        void attachMesh(const MeshPtr& mesh);
        const MeshPtr& getAttachedMesh() { return mAttachedMesh; }
        void detachMesh() { mAttachedMesh = nullptr; }
        bool isAttachedMesh() {return mAttachedMesh != nullptr;}
        
        void bindSkeleton(const SkeletonPtr& skel) { mBindSkeleton = skel; }
        const SkeletonPtr getBindSkeleton() { return mBindSkeleton; }
        void unbindSkeleton() { mBindSkeleton = nullptr; }
        bool isBindSkeleton() { return mBindSkeleton != nullptr; }
        
        /** Run animation, repeat forever if repeat is true. */
        void runAnimate(const char* name, bool loop = true);
        void stopAnimate() { mAnimateName = ""; mAnimateTime = mAnimateTotalTime = 0.f; }
        const std::string& getRunningAnimate() { return mAnimateName; }
        void setAnimateLoop(bool loop) { mAnimateLoop = loop; }
        bool getIsAnimateLoop() { return mAnimateLoop; }
        /** Set animate run speed, default is 1.f. */
        void setAnimateSpeed(float speed) { mAnimateSpeed = speed; }
        float getAnimateSpeed() { return mAnimateSpeed; }
        /** Called after animate over, only valid if loop is false. */
        void setAnimateOverFunc(const std::function<void()>& func) { mAnimateFunc = func; }
        
        
        /** Get named RenderNode, this must call after attchMesh. */
        RenderNode* getRenderNode(const char* name);
        /** Get is need rendering. */
        virtual bool isNeedRender() {return Node::isNeedRender() && mAttachedMesh;}
        
        /** Set node draw mode, Points/Lines/Trangles. */
        void setDrawMode(DrawMode mode) { mNodeState.mode = mode; updateRenderState(); }
        DrawMode getDrawMode() {return mNodeState.mode;}
        /** Set is node need show OBB. */
        void setOBBEnabled(bool enable) { mNodeState.OBBEnable = enable; updateRenderState(); }
        bool getOBBEnabled() { return mNodeState.OBBEnable; }
        /** Set is node will bring shadow to other node. */
        void setBringShadow(bool enable) { mNodeState.isBringShadow = enable; updateRenderState(); }
        bool isBringShadow() { return mNodeState.isBringShadow; }
        /** Set is node will show shadow. */
        void setAcceptShadow(bool enable);
        bool isAcceptShadow() { return mNodeState.isAcceptShadow; }
        /** Auto set child RenderNode lighting enable. */
        void setLightingEnabled(bool enable);
        bool isLightingEnabled() { return mLightEnable; }
        
        /**
         * Set node picking enabled, auto generate OBB for all RenderNode.
         * @params always Set is picking enabled always even it will not rendering, false for default.
         */
        void setPickingEnabled(bool enable, bool always = false);
        bool isPickingEnabled() {return mPickEnabled;}
        bool isPickingAlways() {return mPickAlways;}
        /** Is ray intersect to attached mesh, return RenderNode if ray intersected. */
        RenderNode* isRayIntersect(const Ray& ray);
        
        /** Create child scene node by attris. */
        SceneNode* createChild(const Vector3& pos=Vector3Zero, const Vector3& rotation=Vector3Zero, const Vector3& scale=Vector3(1.0f,1.0f,1.0f));
        /** Create child scene node by name. */
        SceneNode* createChild(const std::string& name);
        /** Set scene node position. */
        void setPosition(const Vector3& pos);
        /** Get scene node position, default pos relative to parent. */
        const Vector3& getPosition(TranslateRelative type = TranslateRelative::eLocal);
        /** 
         * Set scene node rotation by Vector3. 
         * Rotate around parent or world will effect position.
         */
        void setRotation(const Vector3& rotation, TranslateRelative type = TranslateRelative::eLocal);
        /** Get scene node rotation by Vector3. */
        Vector3 getRotation();
        /** 
         * Set scene node rotation by Quaternion.
         * Rotate around parent or world will effect position.
         */
        void setRotateQuaternion(const Quaternion& quat, TranslateRelative type = TranslateRelative::eLocal);
        /** Get scene node rotation by Quaternion. */
        Quaternion getRotateQuaternion();
        /** Set scene node scale. */
        void setScale(const Vector3& scale);
        /** Get scene node scale, default pos relative to parent. */
        const Vector3& getScale(TranslateRelative type = TranslateRelative::eLocal);
        /** Set attach mesh alpha. */
        virtual void setAlpha(float alpha);
        
        /**
         * Default DepthBias not enabled, enabled it when depthBias not equal 0.0f.
         * If depthBias is negative number, node will looks forward to camera. It just is a factor, such as 0.75f or 1.0f.
         */
        void setDepthBias(float depthBias) { mDepthBias = depthBias; }
        float getDepthBias() { return mDepthBias; }
        
        /** Lights used will be update before render, called when lights changed. */
        void setLightingStateNeedUpdate();
        /** Traverse RenderNode, will auto call lambda func. */
        void tranverseRenderNode(std::function<void(const char*, RenderNode*)> callFunc);
        /** Judge is need render. */
        void prepareForRender(float lastFrameTime);
        
    protected:
        //! init SceneNode with position, rotation, scale
        SceneNode(const Vector3& pos=Vector3Zero, const Vector3& rotation=Vector3Zero, const Vector3& scale=Vector3(1.0f,1.0f,1.0f));
        SceneNode(const std::string& name) : Node(name) { init(); }
        /** Init SceneNode some attribution. */
        void init();
        //! delete SceneNode by SceneManager, user can't call destructor function.
        virtual ~SceneNode();
        
        /* Return is light shine scene node. */
        bool isLightShineNode(const LightPtr& l);
        /* Update child RenderNode state. */
        void updateRenderState();
        /* Update rendering attributes, about world rect/rotate/scale... */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    private:
        MeshPtr         mAttachedMesh;  // attached object
        Vector3         mPosition;      // scene node position
        Vector3         mScale;         // scene node scale
        Vector3         mRotation;      // scene node rotation
        Quaternion      mRotateQuat;    // scene node rotation by Quaternion
        bool            mRotateUseVec;  // true for rotate by Vector3, false for rotate by Quaternion
        
        Vector3         mWorldPosition; // cache scene node world position
        Vector3         mWorldScale;    // cache scene node world scale
        Vector3         mWorldRotation; // cache scene node world rotation
        float           mDepthBias;     // rendering depth bias, valid if bigger than 0.0f
        NodeRenderState mNodeState;     // include OBB, shadow, mode state
        
        SkeletonPtr     mBindSkeleton;  // mesh bind skeleton
        std::string     mAnimateName;   // current running animate name
        float           mAnimateTime;   // current running animate time
        bool            mAnimateLoop;   // is animate loop
        float           mAnimateSpeed;  // run animate speed, default 1.f
        float           mAnimateTotalTime;      // current animate total time
        std::function<void()>   mAnimateFunc;   // animate over called function
        
        bool            mLightEnable;   // is lighting enabled, default is true
        bool            mIsLightingDirty;       // is lighting state need update
        std::vector<LightPtr>   mIgnoreLights;  // need ignore lights
        
        bool            mPickEnabled;   // is object picking eanbled
        bool            mPickAlways;    // is object always picking eanbled, even not render
        
        std::map<std::string, RenderNode*>  mRenderNodeMap; // scene node inclued RenderNode
        friend class SceneManager;      //! SceneManager can call destructor function.
    };
}

#endif // PEACH3D_SCENENODE_H
