//
//  Peach3DTerrain.h
//  Peach3DLib
//
//  Created by singoon.he on 09/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#ifndef PEACH3D_TERRAIN_H
#define PEACH3D_TERRAIN_H

#include "Peach3DIObject.h"
#include "Peach3DLight.h"

namespace Peach3D
{
    class PEACH3D_DLL Terrain
    {
    public:
        /**
         * @brief Create a pace of terrain
         * @params data Height data buffer.
         * @params pace Distance between two vertex.
         * @params map Brushes map texture, each 4 brushes need a map texture.
         * @params texl Texture list of terrain.
         */
        static Terrain* create(int width, int height, const float* data, float pace, const std::vector<TexturePtr>& map, const std::vector<TexturePtr>& texl);
        //! query current height info for position
        float getCurrentHeight(const Vector3& pos);
        
        void setPosition(const Vector3& pos) { mTerrainPos = pos; }
        const Vector3& getPosition() { return mTerrainPos; }
        /** Set index brush detail size, default is 35. */
        void setBrushDetailSize(int index, float size) { if (index < mBrushDetails.size()) mBrushDetails[index] = size; }
        const std::vector<float>& getBrushDetails() { return mBrushDetails; }
        /** Change render program before rendering. */
        void prepareForRender(float lastFrameTime);
        
        void setMaterial(const Material& mate) { mMaterial = mate; }
        const Material& getMaterial() { return mMaterial; }
        /** Set node draw mode, Points/Lines/Trangles. */
        void setDrawMode(DrawMode mode) { mDrawMode = mode; }
        DrawMode getDrawMode() {return mDrawMode;}
        /** Set is terrain will show shadow. */
        void setAcceptShadow(bool enable) { mAcceptShadow = enable; setLightingStateNeedUpdate();}
        bool isAcceptShadow() { return mAcceptShadow; }
        /** Auto set terrain lighting enable. */
        void setLightingEnabled(bool enable) { mLightEnable = enable; setLightingStateNeedUpdate();}
        bool isLightingEnabled() { return mLightEnable; }
        /** Lights used will be update before render, called when lights changed. */
        void setLightingStateNeedUpdate() { mIsLightingDirty = true; }
        
        ObjectPtr getObject() { return mTerrainObj; }
        const std::vector<TexturePtr>& getBrushes() { return mBrushes; }
        const std::vector<TexturePtr>& getAlphaMaps() { return mAlphaMap; }
        const std::vector<LightPtr>& getRenderLights() { return mRenderLights; }
        const std::vector<LightPtr>& getShadowLights() { return mShadowLights; }
        float getLandPace() { return mLandPace; }
        int getWidthCount() { return mWidthCount; }
        int getHeightCount() { return mHeightCount; }
        ProgramPtr getProgramForRender()const {return mRenderProgram;}
        
    protected:
        Terrain(int width, int height, float pace, const float* data);
        ~Terrain();
        //! create terrain object(vertex, normal, uv)
        void buildTerrain(const std::vector<TexturePtr>& map, const std::vector<TexturePtr>& texl);
        
    private:
        int         mWidthCount;    // width vertex count
        int         mHeightCount;   // height vertex count
        float       mLandPace;      // distance between two vertex
        float*      mHighData;      // saved data for query current height
        
        DrawMode    mDrawMode;
        ProgramPtr  mRenderProgram; // render program
        ObjectPtr   mTerrainObj;
        Vector3     mTerrainPos;    // terrain current position
        Vector3     mTerrainLength; // terrain x and z length
        std::vector<TexturePtr> mBrushes;   // terrain brush textures
        std::vector<float> mBrushDetails;   // terrain brush textures detail size
        std::vector<TexturePtr> mAlphaMap;  // terrain alpha map textures
        
        bool        mAcceptShadow;  // is terrain accept shadow
        bool        mLightEnable;   // is lighting enabled, default is true
        Material    mMaterial;      // terrain material, not contain textures
        bool        mIsLightingDirty;   // is lighting state need update
        std::vector<std::string> mIgnoreLights; // need ignore lights
        std::vector<LightPtr>   mRenderLights;  // valid lights
        std::vector<LightPtr>   mShadowLights;  // valid shadow lights, setting by parent
        friend class SceneManager;
    };
}

#endif /* PEACH3D_TERRAIN_H */
