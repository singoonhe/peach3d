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
    class PEACH3D_DLL RenderNode
    {
    public:
        RenderNode(const std::string& meshName, IObject* obj);
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
        void setDrawMode(DrawMode mode) { mMode = mode; mIsRenderCodeDirty = true; }
        DrawMode getDrawMode() { return mMode; }
        bool isLightingEnabled() { return mLightEnable; }
        void setLightingEnabled(bool enable);
        void tranverseLighting(std::function<void(const std::string& name)> callFunc);
        void updateLightingState();
        
        void setOBBEnabled(bool enable);
        bool getOBBEnabled() { return mOBBEnable && mRenderOBB; }
        /** Generate OBB for picking or drawing. */
        void generateOBB();
        OBB* getRenderOBB() const { return mRenderOBB; }
        /** Check is ray intersect to current node. */
        bool isRayIntersect(const Ray& ray) { generateOBB(); return mRenderOBB->isRayIntersect(ray); }
        
        void setAmbient(const Color3& ambient) { mMaterial.ambient = ambient; }
        void setDiffuse(const Color3& diffuse) { mMaterial.diffuse = diffuse; }
        void setSpecular(const Color3& specular) { mMaterial.specular = specular; }
        void setShininess(float shininess) { mMaterial.shininess = shininess; }
        void setAlpha(float alpha) { mMaterial.alpha = alpha; }
        /** Reset Object index texture, add texture need use "IObject::addTextureToMaterial" as template. */
        void resetTextureByIndex(int index, ITexture* texture);
        /** Set render program, preset program will be set default. */
        void setRenderProgram(IProgram* program) { mRenderProgram = program; mIsRenderCodeDirty = true; }
        IProgram* getProgramForRender() { return mRenderProgram; }
        
        IObject* getObject() const { return mRenderObj; }
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
        IObject*        mRenderObj;     // render object
        IProgram*       mRenderProgram; // render program
        OBB*            mRenderOBB;     // render OBB, only init when used(ray check or need show)
        bool            mOBBEnable;     // is OBB display enable
        bool            mLightEnable;   // is lighting enabled
        DrawMode        mMode;          // node draw mode, Points/Lines/Triangles
        std::vector<std::string>    mValidLights;
        
        std::string     mObjSpliceName; // (mesh name + object name), keep unique
        uint            mRenderHash;    // calc render hash using XXH32, accelerate sort when rendering
        bool            mIsRenderCodeDirty; // control when render hash need recalc
    };
}

#endif /* PEACH3D_RENDERNODE_H */
