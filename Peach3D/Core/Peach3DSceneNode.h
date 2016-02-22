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
#include "Peach3DRenderNode.h"

namespace Peach3D
{
    class IObject;
    class Mesh;
    class PEACH3D_DLL SceneNode : public Node
    {
    public:
        //! attach Object and save object materials
        void attachMesh(Mesh* mesh);
        //! get attached mesh
        Mesh* getAttachedMesh() { return mAttachedMesh; }
        //! detach mesh
        void detachMesh() { mAttachedMesh = nullptr; }
        //! is attached mesh
        bool isAttachedMesh() {return mAttachedMesh!=nullptr;}
        
        /** Get named RenderNode, this must call after attchMesh. */
        RenderNode* getRenderNode(const char* name);
        /** Get is need rendering. */
        virtual bool isNeedRender() {return Node::isNeedRender() && mAttachedMesh;}
        
        /** Auto set child RenderNode draw mode. */
        void setDrawMode(DrawMode mode);
        DrawMode getDrawMode() {return mMode;}
        /** Auto set child RenderNode OBB enable. */
        void setOBBEnabled(bool enable);
        bool getOBBEnabled() { return mOBBEnable; }
        
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
        virtual ~SceneNode() {}
        
        /* Update rendering attributes, about world rect/rotate/scale... */
        virtual void updateRenderingAttributes(float lastFrameTime);
        
    private:
        Mesh*           mAttachedMesh;  // attached object
        Vector3         mPosition;      // scene node position
        Vector3         mScale;         // scene node scale
        Vector3         mRotation;      // scene node rotation
        Quaternion      mRotateQuat;    // scene node rotation by Quaternion
        bool            mRotateUseVec;  // true for rotate by Vector3, false for rotate by Quaternion
        
        Vector3         mWorldPosition; // cache scene node world position
        Vector3         mWorldScale;    // cache scene node world scale
        Vector3         mWorldRotation; // cache scene node world rotation
        float           mDepthBias;     // rendering depth bias, valid if bigger than 0.0f
        DrawMode        mMode;          // node draw mode, Points/Lines/Triangles
        
        bool            mOBBEnable;     // is OBB display enable
        bool            mPickEnabled;   // is object picking eanbled
        bool            mPickAlways;    // is object always picking eanbled, even not render
        
        std::map<std::string, RenderNode*>  mRenderNodeMap; // scene node inclued RenderNode
        friend class SceneManager;      //! SceneManager can call destructor function.
    };
}

#endif // PEACH3D_SCENENODE_H
