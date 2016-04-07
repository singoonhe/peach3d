//
//  Peach3DRenderNode.h
//  Peach3DLib
//
//  Created by singoon.he on 12/31/15.
//  Copyright © 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_RENDERNODE_H
#define PEACH3D_RENDERNODE_H

#include "Peach3DMatrix4.h"
#include "Peach3DMaterial.h"
#include "Peach3DIObject.h"
#include "Peach3DNode.h"

namespace Peach3D
{
    struct PEACH3D_DLL NodeRenderState
    {
        NodeRenderState() : OBBEnable(false), isBringShadow(false), isAcceptShadow(false), mode(DrawMode::eTriangle) {}
        bool OBBEnable;     // is OBB display enable
        bool isBringShadow; // node will draw when shadow rendering
        bool isAcceptShadow;// node will mix shadow when rendering
        DrawMode mode;      // node draw mode, Points/Lines/Triangles
    };
    
    class PEACH3D_DLL RenderNode
    {
    public:
        RenderNode(const std::string& meshName, const ObjectPtr& obj);
        ~RenderNode();
        
        /*
        void setWorldPosition(const Vector3& pos) { mWorldPosition = pos; }
        const Vector3& getWorldPosition() { return mWorldPosition; }
        void setWorldScale(const Vector3& scale) { mWorldScale = scale; }
        const Vector3& getWorldScale() { return mWorldScale; }
        void setWorldRotation(const Vector3& rot) { mWorldRotation = rot; }
        const Vector3& getWorldRotation() { return mWorldRotation; }
         */
        void setModelMatrix(const Matrix4& mat) { mModelMatrix = mat; }
        const Matrix4& getModelMatrix() { return mModelMatrix; }
        void setMaterial(const Material& mate) { mMaterial = mate; mIsRenderCodeDirty = true; }
        const Material& getMaterial() { return mMaterial; }
        /** Reset render lights name, called by parent in preparing render. */
        void setRenderLights(const std::vector<std::string>& ls) { mRenderLights = ls; mIsRenderCodeDirty = true;}
        const std::vector<std::string>& getRenderLights() { return mRenderLights; }
        
        void setRenderState(const NodeRenderState& state);
        DrawMode getDrawMode() { return mNodeState.mode; }
        bool getOBBEnabled() { return mNodeState.OBBEnable && mRenderOBB; }
        /** Generate OBB for picking or drawing. */
        void generateOBB();
        OBB* getRenderOBB() const { return mRenderOBB; }
        /** Check is ray intersect to current node. */
        bool isRayIntersect(const Ray& ray) { generateOBB(); return mRenderOBB->isRayIntersect(ray); }
        /** Set node will rendering for shadow, using special program. */
        void setRenderShadow(bool shadow) { mIsRenderShadow = shadow; }
        bool isRenderShadow() { return mIsRenderShadow; }
        
        void setAmbient(const Color3& ambient) { mMaterial.ambient = ambient; }
        void setDiffuse(const Color3& diffuse) { mMaterial.diffuse = diffuse; }
        void setSpecular(const Color3& specular) { mMaterial.specular = specular; }
        void setShininess(float shininess) { mMaterial.shininess = shininess; }
        void setEmissive(const Color3& emissive) { mMaterial.emissive = emissive; }
        void setAlpha(float alpha) { mMaterial.alpha = alpha; }
        /** Reset Object index texture, add texture need use "IObject::addTextureToMaterial" as template. */
        void resetTextureByIndex(int index, TexturePtr texture);
        /** Set render program, preset program will be set default. */
        void setRenderProgram(const ProgramPtr& program) { mRenderProgram = program; mIsRenderCodeDirty = true; }
        const ProgramPtr& getProgramForRender() { return mRenderProgram; }
        
        const ObjectPtr& getObject() const { return mRenderObj; }
        uint getRenderHash() { return mRenderHash; }
        /** Recalc render code if needed. */
        void prepareForRender(float lastFrameTime);
        
    private:
        /*
        Vector3         mWorldPosition; // cache scene node world position
        Vector3         mWorldScale;    // cache scene node world scale
        Vector3         mWorldRotation; // cache scene node world rotation
         */
        Matrix4         mModelMatrix;   // render node model matrix
        Material        mMaterial;      // render object material
        ObjectPtr       mRenderObj;     // render object
        ProgramPtr      mRenderProgram; // render program
        OBB*            mRenderOBB;     // render OBB, only init when used(ray check or need show)
        
        std::vector<std::string>    mRenderLights;  // valid lights name, setting by parent
        NodeRenderState mNodeState;     // include OBB, shadow, mode state
        std::string     mObjSpliceName; // (mesh name + object name), keep unique
        uint            mRenderHash;    // calc render hash using XXH32, accelerate sort when rendering
        bool            mIsRenderShadow;    // is rendering for shadow
        bool            mIsRenderCodeDirty; // control when render hash need recalc
    };
}

#endif /* PEACH3D_RENDERNODE_H */
